#include "tlb.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "constants.h"
#include "log.h"
#include "memory.h"
#include "page_table.h"

typedef struct {
  bool valid;
  bool dirty;
  uint64_t last_access;
  va_t virtual_page_number;
  pa_dram_t physical_page_number;
} tlb_entry_t;

tlb_entry_t tlb_l1[TLB_L1_SIZE];
tlb_entry_t tlb_l2[TLB_L2_SIZE];

uint64_t newest_access_time = 0;

uint64_t tlb_l1_hits = 0;
uint64_t tlb_l1_misses = 0;
uint64_t tlb_l1_invalidations = 0;

uint64_t tlb_l2_hits = 0;
uint64_t tlb_l2_misses = 0;
uint64_t tlb_l2_invalidations = 0;

uint64_t get_total_tlb_l1_hits() { return tlb_l1_hits; }
uint64_t get_total_tlb_l1_misses() { return tlb_l1_misses; }
uint64_t get_total_tlb_l1_invalidations() { return tlb_l1_invalidations; }

uint64_t get_total_tlb_l2_hits() { return tlb_l2_hits; }
uint64_t get_total_tlb_l2_misses() { return tlb_l2_misses; }
uint64_t get_total_tlb_l2_invalidations() { return tlb_l2_invalidations; }


void tlb_init() {
  // Initializes all entries of TLBs to constant 0
  memset(tlb_l1, 0, sizeof(tlb_l1));
  memset(tlb_l2, 0, sizeof(tlb_l2));
  tlb_l1_hits = 0;
  tlb_l1_misses = 0;
  tlb_l1_invalidations = 0;
  tlb_l2_hits = 0;
  tlb_l2_misses = 0;
  tlb_l2_invalidations = 0;
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  Invalidate  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -


// -  Auxilary function
// Iterates trough L1/L2 tlb and invalidates the page number that was removed
void tlb_level_invalidate(tlb_entry_t* tlb, uint16_t tlb_size, va_t virtual_page_number, uint64_t* tlb_invalidations){
  for (int i = 0; i < tlb_size; i++){
    if (tlb[i].virtual_page_number == virtual_page_number){
      (*tlb_invalidations)++;
      tlb[i].valid = false;
      return;
    }
  }
}


// -  -  -  -  Main function
// Invalidates entries in the tlb and increments time
void tlb_invalidate(va_t virtual_page_number) {
  increment_time(TLB_L1_LATENCY_NS);
  tlb_level_invalidate(tlb_l1, TLB_L1_SIZE, virtual_page_number, &tlb_l1_invalidations);
  increment_time(TLB_L2_LATENCY_NS);
  tlb_level_invalidate(tlb_l2, TLB_L2_SIZE, virtual_page_number, &tlb_l2_invalidations);
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  Translate  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

// -  Auxilary function
// Checks for hits in L1/L2
tlb_entry_t* check_if_valid(tlb_entry_t* tlb, uint16_t tlb_size, va_t virtual_page_number, uint64_t* tlb_hits, uint64_t* tlb_misses, op_t op) {
  for (int i = 0; i < tlb_size; i++) {
    if (tlb[i].valid && tlb[i].virtual_page_number == virtual_page_number) {
      if (tlb_hits != NULL)
        (*tlb_hits)++;

      if (op == OP_WRITE)
        tlb[i].dirty = true;
      newest_access_time++;
      tlb[i].last_access = newest_access_time;

      return (&tlb[i]);
    }
  }

  if (tlb_misses != NULL)
    (*tlb_misses)++;
  return NULL; // Indicates a miss
}

// -  Auxilary function
// Initializes a new entry in the TLB at a given index
// When injecting a new entry (writting to invalid or on top of a evict)
void tlb_entry_init(int index, pa_dram_t physical_page_number, va_t virtual_page_number, op_t op, tlb_entry_t* tlb) {
  tlb_entry_t new_tlb_entry;
  new_tlb_entry.physical_page_number = physical_page_number;
  new_tlb_entry.virtual_page_number = virtual_page_number;
  new_tlb_entry.valid = true;
  if (op == OP_WRITE)
    new_tlb_entry.dirty = true;
  else
    new_tlb_entry.dirty = false;
  newest_access_time++;
  new_tlb_entry.last_access = newest_access_time;
  tlb[index] = new_tlb_entry;
}


int find_lru_or_invalid(
    tlb_entry_t* tlb,
    uint16_t tlb_size,
    pa_dram_t physical_page_number,
    va_t virtual_page_number,
    op_t op
) {
  int lru_index = -1;
  uint64_t last_access = tlb[0].last_access;

  // Find least recently used entry or an invalid one
  for (int i = 0; i < tlb_size; i++) {
    tlb_entry_t tlb_entry = tlb[i];

    // If the entry is valid, update LRU info (last access)
    if (tlb_entry.valid) {
      // Allow lru to be the first element of the array
      if (tlb_entry.last_access <= last_access) {
        last_access = tlb_entry.last_access;
        lru_index = i;
      }
    }

	  // If it finds an invalid entry, we can replace that one right away
    else {
	    lru_index = i;
      tlb_entry_init(lru_index, physical_page_number, virtual_page_number, op, tlb);
      return -1;
    }
  }

  return lru_index;
}

// -  Auxilary function
// Finding a new spot to inject an entry in the L1/L2 tlb after both L1 and L2 misses
void house_new_entry(
    tlb_entry_t* tlb,
    uint16_t tlb_size,
    pa_dram_t physical_page_number,
    va_t virtual_page_number,
    op_t op
) {
  int lru_index = find_lru_or_invalid(tlb, tlb_size, physical_page_number, virtual_page_number, op);

  // If all TLB entries are valid, and we found the LRU one
  if (lru_index != -1) {
    tlb_entry_t victim = tlb[lru_index];

    if (victim.dirty && tlb_size == TLB_L2_SIZE) {
      // compute page base physical address for write-back
      pa_dram_t victim_page_base = (victim.physical_page_number << PAGE_SIZE_BITS);
      // write-back of whole page if we're taking it out of L2
      write_back_tlb_entry(victim_page_base);
    }

    // In a dirty evict of L1, write to L2 
    if (tlb_size == TLB_L1_SIZE){
      op_t victim_op = victim.dirty ? OP_WRITE : OP_READ;
      // Look for the victim entry in L2
      tlb_entry_t* l2_entry_ptr = check_if_valid(tlb_l2, TLB_L2_SIZE, victim.virtual_page_number, NULL, NULL, victim_op);
      // If it's not in L2, we add it
      if (l2_entry_ptr == NULL){
        house_new_entry(tlb_l2, TLB_L2_SIZE, victim.physical_page_number, victim.virtual_page_number, victim_op);
      }
    }

    // Replace oldest entry with the new one
    tlb_entry_init(lru_index, physical_page_number, virtual_page_number, op, tlb);
  }
}


// -  -  -  -  Main function
// Translates tlb to be able to access the pages
pa_dram_t tlb_translate(va_t virtual_address, op_t op) {
  // Clean bits beyond the virtual address
  virtual_address &= VIRTUAL_ADDRESS_MASK;
  // Extract virtual page number and virtual offset for that page
  va_t virtual_page_number = (virtual_address >> PAGE_SIZE_BITS) & PAGE_INDEX_MASK;
  va_t virtual_page_offset = virtual_address & PAGE_OFFSET_MASK;

  pa_dram_t physical_address = 0;

  // Percorre Tlb_l1 e Tlb_l2 para ver se encontra os valores
  increment_time(TLB_L1_LATENCY_NS);
  tlb_entry_t* tlb_entry_l1 = check_if_valid(tlb_l1, TLB_L1_SIZE, virtual_page_number, &tlb_l1_hits, &tlb_l1_misses, op);
  if (tlb_entry_l1 != NULL) {
    pa_dram_t l1_physical_page_number = tlb_entry_l1->physical_page_number;
    physical_address = (l1_physical_page_number << PAGE_SIZE_BITS) | virtual_page_offset;
    return physical_address;
  }

  increment_time(TLB_L2_LATENCY_NS);
  tlb_entry_t* tlb_entry_l2 = check_if_valid(tlb_l2, TLB_L2_SIZE, virtual_page_number, &tlb_l2_hits, &tlb_l2_misses, op);
  if (tlb_entry_l2 != NULL){
    pa_dram_t l2_physical_page_number = tlb_entry_l2->physical_page_number;
    physical_address = (l2_physical_page_number << PAGE_SIZE_BITS) | virtual_page_offset;

    // If it's a L2 hit, we bring it to L1 as well
    if (tlb_entry_l2->dirty) {
      house_new_entry(tlb_l1, TLB_L1_SIZE, l2_physical_page_number, virtual_page_number, OP_WRITE);
    } else {
      house_new_entry(tlb_l1, TLB_L1_SIZE, l2_physical_page_number, virtual_page_number, OP_READ);
    } 
    return physical_address;
  }

  // Nao esta na L1 nem na L2, vamos pedir à PageTable (ela é que trata das page faults se houver e incrementa tambem o time)
  physical_address = page_table_translate(virtual_address, op);

  // Clean bits beyond the address just to be safe
  physical_address &= DRAM_ADDRESS_MASK;
  // Extract physical page number
  pa_dram_t physical_page_number = (physical_address >> PAGE_SIZE_BITS) & PAGE_INDEX_MASK;

  // Bring data to TLB L2 and TLB L1
  house_new_entry(tlb_l2, TLB_L2_SIZE, physical_page_number, virtual_page_number, op);
  house_new_entry(tlb_l1, TLB_L1_SIZE, physical_page_number, virtual_page_number, op);

  return physical_address;
}
