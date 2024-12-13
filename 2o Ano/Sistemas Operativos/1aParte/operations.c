#include "constants.h"
#include "kvs.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


static struct HashTable* kvs_table = NULL;

/// Ensure that all data is written to the file descriptor.
/// @param fd File descriptor to write to.
/// @param buffer Buffer with the data to write.
/// @return 0 if all data was written, 1 otherwise.
int safeWrite (int fd, char *buffer) {
    size_t len = strlen(buffer);
    size_t done = 0;
	
	while (len > done) {
        ssize_t chars_written = write(fd, buffer + done, len - done);

        if (chars_written < 0) {
            fprintf(stderr, "Write error\n");
            return EXIT_FAILURE;
        }

        done += (size_t) chars_written;
    }

    return EXIT_SUCCESS;
}


/// Sorts an array of strings alphabetically.
/// @param keys The array of keys we want to sort alphabetically
/// @param num The number of keys to sort
/// @param values The array of values that will be sorted according to the sorting of the keys
void sortStrings(char keys[][MAX_STRING_SIZE], int num, char values[][MAX_STRING_SIZE]) {
    char temp[MAX_STRING_SIZE];

    // Sorting strings using bubble sort
    for (int i = 0; i < num - 1; i++) {
        for (int j = 0; j < num - 1 - i; j++) {
            if (strcmp(keys[j], keys[j + 1]) > 0) {
                // Swap keys[j] with keys[j + 1]
				strcpy(temp, keys[j]);
                strcpy(keys[j], keys[j + 1]);
                strcpy(keys[j + 1], temp);
				
				if (values != NULL) {
					// Swap the corresponding values as well
					strcpy(temp, values[j]);
					strcpy(values[j], values[j + 1]);
					strcpy(values[j + 1], temp);
				}
            }
        }
    }
}


/// Calculates a timespec from a delay in milliseconds.
/// @param delay_ms Delay in milliseconds.
/// @return Timespec with the given delay.
static struct timespec delay_to_timespec(unsigned int delay_ms) {
    return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}


int kvs_init() {
    if (kvs_table != NULL) {
        fprintf(stderr, "KVS state has already been initialized\n");
        return 1;
    }

    kvs_table = create_hash_table();
    return kvs_table == NULL;
}


int kvs_terminate() {
    if (kvs_table == NULL) {
        fprintf(stderr, "KVS state must be initialized\n");
        return 1;
    }

    free_table(kvs_table);
	kvs_table = NULL;
    return 0;
}


int kvs_write(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE]) {
    if (kvs_table == NULL) {
        fprintf(stderr, "KVS state must be initialized\n");
        return 1;
    }
	
    // Sort the keys and their corresponding values to ensure a consistent order and
    // avoid potential deadlocks due to thread contention.
    sortStrings(keys, (int) num_pairs, values);

    for (size_t i = 0; i < num_pairs; i++) {
        if (write_pair(kvs_table, keys[i], values[i]) != 0) {
            fprintf(stderr, "Failed to write keypair (%s,%s)\n", keys[i], values[i]);
        }
    }

    return 0;
}


int kvs_read(size_t num_pairs, char keys[][MAX_STRING_SIZE], int fd) {
    if (kvs_table == NULL) {
        fprintf(stderr, "KVS state must be initialized\n");
        return 1;
    }

	// Sort the keys and their corresponding values to ensure a consistent order and
    // avoid potential deadlocks due to thread contention.
	sortStrings(keys, (int) num_pairs, NULL);

    char buf1[] = "[";
    if (safeWrite(fd, buf1)) {
        return 1;
    }

    for (size_t i = 0; i < num_pairs; i++) {
        char* result = read_pair(kvs_table, keys[i]);
        if (result == NULL) {
            char buf2[BUFSIZ];
			sprintf(buf2, "(%s,KVSERROR)", keys[i]);
            if (safeWrite(fd, buf2)) {
                return 1;
            }
        } else {
            char buf2[BUFSIZ];
            sprintf(buf2, "(%s,%s)", keys[i], result);
            if (safeWrite(fd, buf2)) {
                return 1;
            }
        }
        free(result);
    }

    char buf3[] = "]\n";
    if (safeWrite(fd, buf3)) {
        return 1;
    }

    return 0;
}


int kvs_delete(size_t num_pairs, char keys[][MAX_STRING_SIZE], int fd) {
    if (kvs_table == NULL) {
        fprintf(stderr, "KVS state must be initialized\n");
        return 1;
    }

    int aux = 0;

    // Sort the keys and their corresponding values to ensure a consistent order and
    // avoid potential deadlocks due to thread contention.
	sortStrings(keys, (int) num_pairs, NULL);

    for (size_t i = 0; i < num_pairs; i++) {
        if (delete_pair(kvs_table, keys[i]) != 0) {
            if (!aux) {
                char buf1[] = "[";
                if (safeWrite(fd, buf1)) {
                    return 1;
                }
                aux = 1;
            }
            char buf2[BUFSIZ];
            sprintf(buf2, "(%s,KVSMISSING)", keys[i]);
            if (safeWrite(fd, buf2)) {
                return 1;
            }
        }
    }
    if (aux) {
        char buf3[] = "]\n";
        if (safeWrite(fd, buf3)) {
            return 1;
        }
    }

    return 0;
}


int kvs_show(int fd) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyNode *keyNode = kvs_table->table[i];
        while (keyNode != NULL) {
            char buf[BUFSIZ];
            sprintf(buf, "(%s, %s)\n", keyNode->key, keyNode->value);
            if (safeWrite(fd, buf)) {
                return 1;
            }
            keyNode = keyNode->next; // Move to the next node
        }
    }
    return 0;
}


int kvs_backup(char* path, int bck_number) {
	char bck_name[BUFSIZ];

	// Path has the .job cut out from it already
	sprintf(bck_name, "%s-%d.bck", path, bck_number);
	
	int bck_fd = open(bck_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (bck_fd == -1) {
		fprintf(stderr, "Failed to open file\n");
		return -1;
	}

	// The backup file shows the contents of the hashtable just like show does
	kvs_show(bck_fd);

	close(bck_fd);
	
    return 0;
}


void kvs_wait(unsigned int delay_ms) {
    struct timespec delay = delay_to_timespec(delay_ms);
    nanosleep(&delay, NULL);
}

