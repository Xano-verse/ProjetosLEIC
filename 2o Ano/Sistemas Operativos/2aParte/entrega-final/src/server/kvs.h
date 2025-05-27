#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#define TABLE_SIZE 26

#include <pthread.h>
#include <stddef.h>

typedef struct ClientNode {
	int fd_notif;
    struct ClientNode *next;
} ClientNode;

typedef struct KeyNode {
    char *key;
    char *value;
    struct KeyNode *next;
    ClientNode *clients;
} KeyNode;

// Each entry of the hashtable has a lock assigned to it
typedef struct HashTable {
    KeyNode *table[TABLE_SIZE];
	pthread_rwlock_t rwlock[TABLE_SIZE];
} HashTable;


/// Creates a new event hash table.
/// @return Newly created hash table, NULL on failure
struct HashTable *create_hash_table();

/// Hash function based on key initial.
/// @param key Lowercase alphabetical string.
/// @return hash.
int hash(const char *key);

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

/// Adds a new client node to the key node.
/// @param ht Hash table to be added to.
/// @param key Key of the pair to be added to.
/// @param fd_notif File descriptor of the notification pipe.
/// @return 0 if the client was added successfully, 1 otherwise.
int add_client_node(HashTable *ht, char *key, int fd_notif);

/// Removes a client node from the key node.
/// @param ht Hash table to be removed from.
/// @param key Key of the pair to be removed from.
/// @param fd_notif File descriptor of the notification pipe.
/// @return 0 if the client was removed successfully, 1 otherwise.
int remove_client_node(HashTable *ht, char *key, int fd_notif);

/// Destroys all clients of a key node.
/// @param clients Clients to be destroyed.
void destroy_clients(ClientNode *clients);

/// Gets the clients of a key node.
/// @param ht Hash table to be searched.
/// @param key Key of the pair to be searched.
/// @return Clients of the key node, NULL if the key is not found.
ClientNode* get_clients(HashTable *ht, char *key);

/// Sets the global client to the clients of a key node.
/// @param ht Hash table to be searched.
/// @param key Key of the pair to be searched.
/// @return 1 if the the global client was set successfully, 0 if there are no clients.
int set_global_client(HashTable *ht, char *key);

/// Advances the global client pointer to the next client.
/// @return 1 if the global client was advanced successfully, 0 if there are no more clients.
int advance_global_client();

/// Gets the file descriptor of the global client.
/// @return File descriptor of the global client.
int* get_fd_notif();

/// Removes all subscriptions from the hash table.
/// @param ht Hash table to be searched.
void remove_all_subscriptions(HashTable *ht);

#endif  // KVS_H

