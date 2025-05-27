#include "kvs.h"

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable pointing to the current client node
ClientNode* global_client = NULL;


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

    KeyNode *keyNode = ht->table[index];

    // Search for the key node
    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
			free(keyNode->value);
            keyNode->value = strdup(value);
            
            return 0;
        }
        keyNode = keyNode->next; // Move to the next node
    }

    // Key not found, create a new key node
    keyNode = malloc(sizeof(KeyNode));
    keyNode->key = strdup(key); // Allocate memory for the key
    keyNode->value = strdup(value); // Allocate memory for the value
    keyNode->next = ht->table[index]; // Link to existing nodes
    keyNode->clients = NULL;
    ht->table[index] = keyNode; // Place new key node at the start of the list
	
    return 0;
}


char* read_pair(HashTable *ht, const char *key) {
    int index = hash(key);

    KeyNode *keyNode = ht->table[index];
    char* value;

    while (keyNode != NULL) {
        if (strcmp(keyNode->key, key) == 0) {
            value = strdup(keyNode->value);

            return value; // Return copy of the value if found
        }
        keyNode = keyNode->next; // Move to the next node
    }

    return NULL; // Key not found
}


int delete_pair(HashTable *ht, const char *key) {
    int index = hash(key);
	
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
            destroy_clients(keyNode->clients);
            free(keyNode); // Free the key node itself
			
            return 0; // Exit the function
        }
        prevNode = keyNode; // Move prevNode to current node
        keyNode = keyNode->next; // Move to the next node
    }
    
    return 1;
}


void free_table(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
		// Lock each lock for each hashtable entry (with a write lock since we'll be deleting the contents)
		pthread_rwlock_wrlock(&ht->rwlock[i]);

        KeyNode *keyNode = ht->table[i];
        while (keyNode != NULL) {
            KeyNode *temp = keyNode;
            keyNode = keyNode->next;
            free(temp->key);
            free(temp->value);
            destroy_clients(temp->clients);
			free(temp);
        }

		// Unlock and destroy each lock for each hashtable entry
		pthread_rwlock_unlock(&ht->rwlock[i]);
		pthread_rwlock_destroy(&ht->rwlock[i]);
    }
	
    free(ht);
}


int add_client_node(HashTable *ht, char *key, int fd_notif) {
    int index = hash(key);

    KeyNode *keyNode = ht->table[index];

	// In the corresponding index of the hashtable
    while (keyNode != NULL) {
		// We find the entry for the specific key
        if (strcmp(keyNode->key, key) == 0) {
            ClientNode *client = (ClientNode*) malloc(sizeof(ClientNode));
            client->fd_notif = fd_notif;
            client->next = keyNode->clients;
            keyNode->clients = client;
            return 0;
        }
        keyNode = keyNode->next;
    }

    return 1;
}


int remove_client_node(HashTable *ht, char* key, int fd_notif) {
    int index = hash(key);

    KeyNode *keyNode = ht->table[index];

	// In the corresponding index of the hashtable
    while (keyNode != NULL) {
		// We look for the entry with the key
        if (strcmp(keyNode->key, key) == 0) {
			// And in the clients of that key entry that we found
            while (keyNode->clients != NULL) {
				// We find the right client
				if (keyNode->clients->fd_notif == fd_notif) {
                    ClientNode *temp = keyNode->clients; 
                    keyNode->clients = keyNode->clients->next;
                    free(temp);
                    return 0;
                }
                keyNode->clients = keyNode->clients->next;
            }
        }
        keyNode = keyNode->next;
    }

    return 1;
}


void destroy_clients(ClientNode *clients) {
    // Iterate through the list of clients and free each node
    while (clients != NULL) {
        ClientNode *temp = clients;
        clients = clients->next;
        free(temp);
    }
}


ClientNode* get_clients(HashTable *ht, char *key) {
	int index = hash(key);

	KeyNode* keyNode = ht->table[index];

	// Searches for the entry with the key
	while (keyNode != NULL) {
        // If the key is found, return the clients of that key
		if (strcmp(keyNode->key, key) == 0) {
			return keyNode->clients;
		}
		keyNode = keyNode->next;
	}

	return NULL;
}


int set_global_client(HashTable *ht, char *key) {
	global_client = get_clients(ht, key);

	if (global_client == NULL) {
		return 0;
	}

	return 1;
}


int advance_global_client() {
    // Move the global client pointer to the next client
	global_client = global_client->next;
    
	if (global_client == NULL) {
		return 0;
	}

	return 1;
}


int* get_fd_notif() {
	return &(global_client->fd_notif);
}


void remove_all_subscriptions(HashTable *ht) {
    // Iterate through the hash table
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyNode *keyNode = ht->table[i];
        // Iterate through the key nodes
        while (keyNode != NULL) {
            destroy_clients(keyNode->clients);
            keyNode->clients = NULL;
            keyNode = keyNode->next;
        }
    }
}

