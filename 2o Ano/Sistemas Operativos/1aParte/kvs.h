#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#define TABLE_SIZE 26

#include <pthread.h>
#include <stddef.h>


typedef struct KeyNode {
    char *key;
    char *value;
    struct KeyNode *next;
} KeyNode;

// Each entry of the hashtable has a lock assigned to it
typedef struct HashTable {
    KeyNode *table[TABLE_SIZE];
	pthread_rwlock_t rwlock[TABLE_SIZE];
} HashTable;

/// Creates a new event hash table.
/// @return Newly created hash table, NULL on failure
struct HashTable *create_hash_table();

/// Appends a new key value pair to the hash table.
/// @param ht Hash table to be modified.
/// @param key Key of the pair to be written.
/// @param value Value of the pair to be written.
/// @return 0 if the node was appended successfully, 1 otherwise.
int write_pair(HashTable *ht, const char *key, const char *value);

/// Reads a value associated with a given key.
/// @param ht Hash table to read from.
/// @param key Key of the pair to read.
/// @return value associated with the key, NULL if the key is not found
char* read_pair(HashTable *ht, const char *key);

/// Deletes the value of given key.
/// @param ht Hash table to delete from.
/// @param key Key of the pair to be deleted.
/// @return 0 if the node was deleted successfully, 1 otherwise.
int delete_pair(HashTable *ht, const char *key);

/// Frees the hashtable.
/// @param ht Hash table to be deleted.
void free_table(HashTable *ht);


#endif  // KVS_H