#include "kvs.h"

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// Hash function based on key initial.
/// @param key Lowercase alphabetical string.
/// @return hash.
/// NOTE: This is not an ideal hash function, but is useful for test purposes of the project
int hash(const char *key) {
    int firstLetter = tolower(key[0]);
    if (firstLetter >= 'a' && firstLetter <= 'z') {
        return firstLetter - 'a';
    } else if (firstLetter >= '0' && firstLetter <= '9') {
        return firstLetter - '0';
    }

    return -1; // Invalid index for non-alphabetic or number strings
}


struct HashTable* create_hash_table() {
	HashTable *ht = malloc(sizeof(HashTable));
	if (!ht) return NULL;
	for (int i = 0; i < TABLE_SIZE; i++) {
		ht->table[i] = NULL;
		if (pthread_rwlock_init(&ht->rwlock[i], NULL) != 0) {
			fprintf(stderr, "Failed to initialize RW lock of hashtable\n");
			return NULL;
		}
  	}
	
  	return ht;
}


int write_pair(HashTable *ht, const char *key, const char *value) {
    int index = hash(key);

	// We lock the corresponding hashtable entry with a write lock since we'll be writing to it
	if (pthread_rwlock_wrlock(&ht->rwlock[index]) != 0) {
        fprintf(stderr, "Failed to write-lock index %d\n", index);
    }

    KeyNode *keyNode = ht->table[index];

    // Search for the key node
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
			free(keyNode->value);
            keyNode->value = strdup(value);

			pthread_rwlock_unlock(&ht->rwlock[index]);
            return 0;
        }
        keyNode = keyNode->next; // Move to the next node
    }

    // Key not found, create a new key node
    keyNode = malloc(sizeof(KeyNode));
    keyNode->key = strdup(key); // Allocate memory for the key
    keyNode->value = strdup(value); // Allocate memory for the value
    keyNode->next = ht->table[index]; // Link to existing nodes
    ht->table[index] = keyNode; // Place new key node at the start of the list
	
	pthread_rwlock_unlock(&ht->rwlock[index]);
    return 0;
}


char* read_pair(HashTable *ht, const char *key) {
    int index = hash(key);

	// We lock the corresponding hashtable entry with a read lock since we'll only be reading it 
	if (pthread_rwlock_rdlock(&ht->rwlock[index]) != 0) {
        fprintf(stderr, "Failed to read-lock index %d\n", index); 
    }

    KeyNode *keyNode = ht->table[index];
    char* value;

    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            value = strdup(keyNode->value);

			pthread_rwlock_unlock(&ht->rwlock[index]);
            return value; // Return copy of the value if found
        }
        keyNode = keyNode->next; // Move to the next node
    }

	pthread_rwlock_unlock(&ht->rwlock[index]);
    return NULL; // Key not found
}


int delete_pair(HashTable *ht, const char *key) {
    int index = hash(key);
	
	// We lock the corresponding hashtable entry with a write lock since we'll be deleting and freeing contents
	if (pthread_rwlock_wrlock(&ht->rwlock[index]) != 0) {
        fprintf(stderr, "Failed to lock index %d\n", index); 
    }
    
	KeyNode *keyNode = ht->table[index];
    KeyNode *prevNode = NULL;

    // Search for the key node
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            // Key found; delete this node
            if (prevNode == NULL) {
                // Node to delete is the first node in the list
                ht->table[index] = keyNode->next; // Update the table to point to the next node
            } else {
                // Node to delete is not the first; bypass it
                prevNode->next = keyNode->next; // Link the previous node to the next node
            }
            // Free the memory allocated for the key and value
            free(keyNode->key);
            free(keyNode->value);
            free(keyNode); // Free the key node itself
			
			pthread_rwlock_unlock(&ht->rwlock[index]);
            return 0; // Exit the function
        }
        prevNode = keyNode; // Move prevNode to current node
        keyNode = keyNode->next; // Move to the next node
    }
    
	pthread_rwlock_unlock(&ht->rwlock[index]);
    return 1;
}


void free_table(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
		//Lock each lock for each hashtable entry (with a write lock since we'll be deleting the contents)
		pthread_rwlock_wrlock(&ht->rwlock[i]);

        KeyNode *keyNode = ht->table[i];
        while (keyNode != NULL) {
            KeyNode *temp = keyNode;
            keyNode = keyNode->next;
            free(temp->key);
            free(temp->value);
			free(temp);
        }

		//Unlock and destroy each lock for each hashtable entry
		pthread_rwlock_unlock(&ht->rwlock[i]);
		pthread_rwlock_destroy(&ht->rwlock[i]);
    }
	
    free(ht);
}

