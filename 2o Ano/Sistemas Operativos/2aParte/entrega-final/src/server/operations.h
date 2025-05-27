#ifndef KVS_OPERATIONS_H
#define KVS_OPERATIONS_H

#include <stddef.h>


/// Ensure that all data is written to the file descriptor.
/// @param fd File descriptor to write to.
/// @param buffer Buffer with the data to write.
/// @return 0 if all data was written, 1 otherwise.
int safeWrite (int fd, char *buffer);

/// Sortes the keys in alphabetical order.
/// @param keys Array of keys' strings to be sorted
/// @param num Length of the keys' array
/// @param values Array of values' strings to be reordered in parallel with keys'
void sortStrings(char keys[][MAX_STRING_SIZE], int num, char values[][MAX_STRING_SIZE]);

/// Initializes the KVS state.
/// @return 0 if the KVS state was initialized successfully, 1 otherwise.
int kvs_init();

/// Destroys the KVS state.
/// @return 0 if the KVS state was terminated successfully, 1 otherwise.
int kvs_terminate();

/// Writes a key value pair to the KVS. If key already exists, it is updated.
/// @param num_pairs Number of pairs being written.
/// @param keys Array of keys' strings.
/// @param values Array of values' strings.
/// @return 0 if the pairs were written successfully, 1 otherwise.
int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE]);

/// Reads values from the KVS.
/// @param num_pairs Number of pairs to read.
/// @param keys Array of keys' strings.
/// @param fd File descriptor to write the (successful) output.
/// @return 0 if the key reading, 1 otherwise.
int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], int fd);

/// Deletes key value pairs from the KVS.
/// @param num_pairs Number of pairs to read.
/// @param keys Array of keys' strings.
/// @param fd File descriptor to write the (successful) output.
/// @return 0 if the pairs were deleted successfully, 1 otherwise.
int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], int fd);

/// Writes the state of the KVS.
/// @param fd File descriptor to write the output.
/// @return 0 if the pairs were shown successfully, 1 otherwise.
int kvs_show(int fd);

/// Creates a backup of the KVS state and stores it in the correspondent backup file
/// @param path Path to the backup file.
/// @param total_backups Number of backups to keep.
/// @return 0 if the backup was successful, -1 otherwise.
int kvs_backup(char* path, int total_backups);

/// Waits for a given amount of time.
/// @param delay_us Delay in milliseconds.
void kvs_wait(unsigned int delay_ms);

/// Adds a new client to the KVS.
/// @param key Key to be added to.
/// @param fd_notif File descriptor of the notification pipe.
/// @return 0 if the client was added successfully, 1 otherwise.
int add_client(char *key, int fd_notif);

/// Removes a client from the KVS.
/// @param key Key to be removed from.
/// @param fd_notif File descriptor of the notification pipe.
/// @return 0 if the client was removed successfully, 1 otherwise.
int remove_client(char *key, int fd_notif);

/// Sets the global client to the clients of a key.
/// @param key Key to be set to the global client.
/// @return 1 if the global client was set successfully, 0 if there are no clients.
int kvs_set_global_client(char* key);

/// Advances the global client to the next client.
/// @return 1 if the global client was advanced successfully, 0 if there are no more clients.
int kvs_advance_global_client(char* key);

/// Gets the file descriptor of the global client.
/// @return File descriptor of the global client.
int* kvs_get_fd_notif(); 

/// Removes all subscriptions from the KVS.
void kvs_remove_all_subscriptions();

#endif  // KVS_OPERATIONS_H

