// ------------------------------------------------
//                   Projeto SO                   
// ------------------------------------------------
/**
 * Instituto Superior Tecnico - Campus Alameda
 * Curso: Licenciatura em Engenharia Informatica e de Computadores (LEIC - A)
 * Cadeira: Sistemas Operativos (SO)
 * Corpo Docente responsavel: Joao Garcia
 * Ano letivo 2024/2025
 * Alunos: Alexandre Delgado | ist1109441 
 * 		   Madalena Yang 	 | ist1110206
 */


// Needed for signal handling
#define _POSIX_SOURCE 

#include "constants.h"
#include "operations.h"
#include "parser.h"
#include "src/common/constants.h"
#include "src/common/protocol.h"
#include "src/common/io.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>


pthread_mutex_t backup_lock;
pthread_mutex_t global_client_lock;
pthread_mutex_t current_client_lock;

sem_t pending_clients;
sem_t free_spots;

sigset_t mask_sigusr1;


/// @brief Structure to store the file descriptors of a client's pipes
/// Members of the structure:
/// - fd_req: File descriptor of the client's request pipe
/// - fd_resp: File descriptor of the client's response pipe
/// - fd_notif: File descriptor of the client's notification pipe
typedef struct Client {
	int fd_req;
	int fd_resp;
	int fd_notif;
} Client;

// Host thread is producer, worker threads are consumers
// Host thread will put clients in the buffer and worker threads will take them and handle them
Client* clients_to_handle[MAX_SESSION_COUNT];

int current_client_to_store = 0;
int current_client_to_handle = 0;

/// @brief Structure to pass the arguments to the read_files() function
/// since pthread_create only allows 1 argument functions
/// Members of the structure:
/// - directory: Directory we received as input, already opened
/// - og_path: Original path to the directory we received as input
/// - max_processes: Maximum number of child processes that can be running at the same time
/// - pending_bck: Number of pending processes (doing a backup), whether they're zombies or still running 
typedef struct ReadFilesArgs {
	DIR* directory;
	char* og_path;
	int max_processes;
	int pending_bck;
} ReadFilesArgs;



int notify_clients(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE], int command);

int parse_file(ReadFilesArgs* args, int in_fd, int out_fd, char* path);

void* read_files(void* args_arg);

char unsubscribe_key(char* key, int fd_notif, char* client_keys[MAX_NUMBER_SUB], int *subscribed);

int wait_for_connection(int fd_reg);

char establish_connection(int fd_reg, Client* client);

char unsubscribe_all_keys(int fd_notif, char *client_keys[MAX_NUMBER_SUB], int *subscribed);

char subscribe_key(char* key, int fd_notif, char* client_keys[MAX_NUMBER_SUB], int *subscribed);

void terminate_all_clients(int sig);

void close_all_clients(int is_child);

void* clients_host_thread(void* arg);

void handle_client(Client* client);

void *retrieve_client();




/// Notify clients all clients subscribed to the given keys
/// @param num_pairs Number of key-value pairs passed to the function
/// @param keys Array of keys that were written/deleted and whose clients we must notify
/// @param values Array of values corresponding to the keys
/// @param command Is 0 if the command done to the keys was a DELETE, is 1 if the command was a WRITE
/// @return 0 on success, 1 on error
int notify_clients(size_t num_pairs, char keys[][MAX_STRING_SIZE], char values[][MAX_STRING_SIZE], int command) {

	sortStrings(keys, (int) num_pairs, values); 

	// For each key
	for (size_t i = 0; i < num_pairs; i++) {
		// We notify each subscribed client
		
		// But if there are writes of the same key, we only notify the last write (this is why we sorted the keys)
		// i.e we only notify if the next key is different from the current one
		if (i != num_pairs && strcmp(keys[i], keys[i+1]) != 0) {	
			char msg[MAX_STRING_SIZE*2 + 3] = {0};

			if (command == 0) {
				sprintf(msg, "(%s,DELETED)", keys[i]);
			} else {
				sprintf(msg, "(%s,%s)", keys[i], values[i]);
			}

			// To notify the clients, we go through every client of the current key
			// This is done through a global pointer that we need to lock while we haven't gone through all of the clients 
			pthread_mutex_lock(&global_client_lock);

			if (kvs_set_global_client(keys[i])) { 
			
				do {
					if (write_all((*kvs_get_fd_notif()), msg, sizeof(msg)) == -1) {
						fprintf(stderr, "Failed to write to notification path\n");
						pthread_mutex_unlock(&global_client_lock);
						return EXIT_FAILURE;
					}
				} while (kvs_advance_global_client(keys[i]));
				// advance_global_client will return 0 when global_client reaches NULL
			}

			pthread_mutex_unlock(&global_client_lock);
		}
	}
	
	return EXIT_SUCCESS;
}


/// Reads the contents of a file and executes the commands in it
/// @param args Structure with arguments from the read_files() function
/// @param in_fd File descriptor of the file we're reading from
/// @param out_fd File descriptor of the output file, the one we'll write into
/// @param path Path to the input file. It will be modified to create the backup file
/// @return 0 on success, 1 on error
int parse_file(ReadFilesArgs* args, int in_fd, int out_fd, char* path) {

		int bck_number = 1;

		int reachedEOC = 0;
		// Each iteration of the loop corresponds to a command
		while (reachedEOC == 0) {	

			char keys[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    		char values[MAX_WRITE_SIZE][MAX_STRING_SIZE] = {0};
    		unsigned int delay;
    		size_t num_pairs;

			switch (get_next(in_fd)) {
				case CMD_WRITE:
					num_pairs = parse_write(in_fd, keys, values, MAX_WRITE_SIZE, MAX_STRING_SIZE);

					if (num_pairs == 0) {
						fprintf(stderr, "Invalid command. See HELP for usage\n");
						continue;
					}

					if (kvs_write(num_pairs, keys, values)) {
						fprintf(stderr, "Failed to write pair\n");
					}

					// 1 means write command 
					if (notify_clients(num_pairs, keys, values, 1) != 0) {
						fprintf(stderr, "Failed to notify clients\n");
					}
							   
					break;

				case CMD_READ:
					num_pairs = parse_read_delete(in_fd, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);
					
					if (num_pairs == 0) {
						fprintf(stderr, "Invalid command. See HELP for usage\n");
						continue;
					}

					if (kvs_read(num_pairs, keys, out_fd)) {
						fprintf(stderr, "Failed to read pair\n");
					}

					break;

				case CMD_DELETE:
					num_pairs = parse_read_delete(in_fd, keys, MAX_WRITE_SIZE, MAX_STRING_SIZE);

					if (num_pairs == 0) {
						fprintf(stderr, "Invalid command. See HELP for usage\n");
						continue;
					}

					// We need to notify the clients before we delete the entries from the hashtables
					// 0 means delete command 
					if (notify_clients(num_pairs, keys, values, 0) != 0) {
						fprintf(stderr, "Failed to notify clients\n");
					}
	
					if (kvs_delete(num_pairs, keys, out_fd)) {
						fprintf(stderr, "Failed to delete pair\n");
					}

		
					break;

				case CMD_SHOW:
					if (kvs_show(out_fd)) {
						fprintf(stderr, "Failed to show pairs\n");
					}
					break;

				case CMD_WAIT:
					if (parse_wait(in_fd, &delay, NULL) == -1) {
						fprintf(stderr, "Invalid command. See HELP for usage\n");
						continue;
					}

					if (delay > 0) {
						char buf1[] = "Waiting...\n";
						if (safeWrite(out_fd, buf1)) {
							return EXIT_FAILURE;
						}
						kvs_wait(delay);
					}

					break;

				// pending_bck holds the number of pending processes, whether they're zombies or still running.
				// It is updated every time we retrieve a process
				case CMD_BACKUP: {
					pid_t pid;
					
					pthread_mutex_lock(&backup_lock);

					// If the number of pending processes is already at the maximum, we retrieve one of them
					// It is possible that all of the pending processes are still running.
					// In that case, the wait() function waits for one of them to finish
					if (args->pending_bck >= args->max_processes) {
						wait(NULL);
						args->pending_bck--;
					}

					pid = fork();				
					args->pending_bck++;

					// We can now unlock because the pending_bck variable underwent all the necessary changes already
					pthread_mutex_unlock(&backup_lock);

					if (pid == 0) {
						// The child process will also have the client's pipes opened. We close them right away to ensure proper behaviour
						close_all_clients(1);	// 1 means we're calling the function from a child process

						if (kvs_backup(path, bck_number) == -1) {
							fprintf(stderr, "Failed to create backup\n");	
						}
						_exit(0);	

					} else if (pid == -1) {
						fprintf(stderr, "Failed to create subprocess\n");
						continue;
					}
					
					bck_number++;
					break;
				}

				case CMD_INVALID:
					fprintf(stderr, "Invalid command. See HELP for usage\n");
					break;

				case CMD_HELP: {
					char buf2[] = "Available commands:\n"
						"  WRITE [(key,value)(key2,value2),...]\n"
						"  READ [key,key2,...]\n"
						"  DELETE [key,key2,...]\n"
						"  SHOW\n"
						"  WAIT <delay_ms>\n"
						"  BACKUP\n"
						"  HELP\n";
					if (safeWrite(out_fd, buf2)) {
						return EXIT_FAILURE;
					}
					break;
				}
				
				case CMD_EMPTY:
					break;

				case EOC:
					reachedEOC = 1;
				}
  		}

	return EXIT_SUCCESS;
}

 
/// Goes through the several files in the directory and reads them
/// @param args_arg Structure with the arguments we want to receive in this function
/// since pthread_create only allows calls to 1 argument functions
/// @return NULL since the return of this function doesn't matter
void* read_files(void* args_arg) {

	// Block this thread from receiving the signals SIGUSR1 and SIGINT
	if (pthread_sigmask(SIG_BLOCK, &mask_sigusr1, NULL) != 0) {
		fprintf(stderr, "Failed to block signal SIGUSR1\n");
		return NULL;
	}


	ReadFilesArgs* args = args_arg;

	struct dirent *dir_entry;

	// readdir automaitcally progresses to the next entry in the directory every time it is called. It keeps a global 
	while((dir_entry = readdir(args->directory)) != NULL) {

		// Skips the . and .. files (current directory and parent directory)
		if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, "..")) { 
			continue;
		}

		// Checks if the file is a .job file. If not, skips it
		char *job_file = strrchr(dir_entry->d_name, '.');
		if (job_file == NULL || strcmp(job_file, ".job")) {
			continue;
		}
		
		// It doesn't matter if the og_path already comes with a / since having more than one / makes no difference
		char path[strlen(args->og_path) + 1 + MAX_JOB_FILE_NAME_SIZE];

		sprintf(path, "%s/%s", args->og_path, dir_entry->d_name);
		int in_fd = open(path, O_RDONLY);

		if (in_fd == -1) {
			fprintf(stderr, "Failed to open .job file\n");
			continue;
		}

		char out_name[strlen(path) + 1];
		// We find the last . in the path and rewrite it as \0, which cuts out the .job from the path
		char *last_dot = strrchr(path, '.');
		*(last_dot) = '\0';

		sprintf(out_name, "%s.out", path);

		int out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		
		if (out_fd == -1) {
			fprintf(stderr, "Failed to open .out file\n");
			close(in_fd);
			continue;
		}

		if (parse_file(args, in_fd, out_fd, path) == 1) {
			return NULL;
		}

		close(in_fd);
		close(out_fd);

		// memset is writing all the characters in the path as null characters.
		// This resets the path and leaves the string ready to the next iteration
		// Without this, the names of the files would concatenate to each other between iterations 
		memset(path, 0, strlen(path));
	}

	return NULL;

}


/// Unsubscribes a client from the given key
/// @param key Key to be unsubscribed from
/// @param fd_notif File descriptor of the client's notification pipe so we can identify the client in the hashtable
/// @param subscribed Variable keeping track of how many keys the client is subscribed to
/// @return '0' if the subscription existed and was removed, '1' if the subscription did not exist  
char unsubscribe_key(char* key, int fd_notif, char* client_keys[MAX_NUMBER_SUB], int *subscribed) {

	// Remove the client from the (key, value) entry on the hashtable
	if (remove_client(key, fd_notif) == 0) {

		// We search for the key in client_keys
		for (int i = 0; i < MAX_NUMBER_SUB; i++) {
			if (client_keys[i] != NULL) {
				if (strcmp(key, client_keys[i]) == 0) {
					// And when we find it, we overwrite it with the last key from the array
					client_keys[i] = client_keys[(*subscribed)-1];
					// And we free the position where that last key was
					client_keys[(*subscribed)-1] = NULL;
				}
			}
		}
		(*subscribed)--;
		return '0';
	}

	// If the client was not found, the subscription did not exist
	return '1';
}


/// Reads from the register pipe and waits for a connection request
/// @param fd_reg File descriptor of the server's register pipe
/// @return 0 on success, 1 on error
int wait_for_connection(int fd_reg) {
	char op_code = '0'; 
	
	while (op_code != '1') {
		// Receives message from client trying to connect
		if (read_all(fd_reg, &op_code, 1, NULL) == -1) {
			fprintf(stderr, "Failed to read from register FIFO\n");
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}


/// Reads the three pipes' path from the register pipe and opens them
/// @param fd_reg File descriptor of the register pipe we'll read from
/// @param client Client structure to store the file descriptors of the three newly opened pipes
/// @return '0' on success, '1' on error
char establish_connection(int fd_reg, Client* client) {
	char req_pipe_path[MAX_PIPE_PATH_LENGTH];
	char resp_pipe_path[MAX_PIPE_PATH_LENGTH];
	char notif_pipe_path[MAX_PIPE_PATH_LENGTH];

	// Read the pipes' paths
	if (read_string(fd_reg, req_pipe_path) == -1) {
		fprintf(stderr, "Failed to read request pipe path from register FIFO\n");
		return '1';
	}

	if (read_string(fd_reg, resp_pipe_path) == -1) {
		fprintf(stderr, "Failed to read response pipe path from register FIFO\n");
		return '1';
	}

	if (read_string(fd_reg, notif_pipe_path) == -1) {
		fprintf(stderr, "Failed to read notification pipe path from register FIFO\n");
		return '1';
	}	

	
	// Open the pipes
	client->fd_req = open(req_pipe_path, O_RDONLY);
	if (client->fd_req == -1) {
		fprintf(stderr, "Failed to open request FIFO\n");
		return '1';
	}

	client->fd_resp = open(resp_pipe_path, O_WRONLY);
	if (client->fd_resp == -1) {
		fprintf(stderr, "Failed to open response FIFO\n");
		return '1';
	}

	client->fd_notif = open(notif_pipe_path, O_WRONLY);
	if (client->fd_notif == -1) {
		fprintf(stderr, "Failed to open notification FIFO\n");
		return '1';
	}

	printf("Client connected\n"); 
	return '0';

}


/// Unsubscribes every key of the given client
/// @param fd_notif File descriptor of the notification pipe so we can identify the client in the hashtable
/// @param client_keys Keys the client is subscribed to
/// @param subscribed Variable that keeps track of how many subscriptions the client has
/// @return '0' on success, '1' on error 
char unsubscribe_all_keys(int fd_notif, char *client_keys[MAX_NUMBER_SUB], int *subscribed) {

	// For each key the client is subscribed to, we remove that subscription
	for (int i = 0; i < MAX_NUMBER_SUB; i++) {
		if (client_keys[i] != NULL) {
			if (unsubscribe_key(client_keys[i], fd_notif, client_keys, subscribed) == '1') {
				fprintf(stderr, "Subscription not found\n");
				return '1';
			}
		}
	}
	return '0';
}


/// Adds a client subscription to a key, i.e registers the client in the key-value entry of the data structure
/// @param key Key to be subscribed to
/// @param fd_notif File descriptor of the client's notification pipe. This will be the client's identifier in the data structure
/// @param client_keys Array with the keys the client is subscribed to
/// @param subscibred Variable that keeps track of how many subscriptions the client has
/// @return '1' on success, '0' on error 
char subscribe_key(char* key, int fd_notif, char* client_keys[MAX_NUMBER_SUB], int *subscribed) {
	if (add_client(key, fd_notif) == 0) {
		client_keys[(*subscribed)] = key;
		(*subscribed)++;
		return '1';
	}

	// If add_client failed, return 0
	return '0';
}


/// Termiantes all clients, i.e removes all of their subscriptions and closes all of their pipes
/// @param sig Signal received 
void terminate_all_clients(int sig) {
	// Using the signal() function may reset the handler to the default behavior after handling a signal.
	// This ensures the handler is reinstalled to catch subsequent signals.
	if (sig == SIGUSR1) {
		if (signal(SIGUSR1, terminate_all_clients) == SIG_ERR) {
			fprintf(stderr, "Failed to set SIGUSR1 signal handler\n");
			return;
		}
	}

	kvs_remove_all_subscriptions();

	// 0 means this function is being called from the parent process
	close_all_clients(0);
}


/// Closes all of the pipes from every client currently connected
/// @param is_child Variable that determines whether the function was called from a parent or child process
void close_all_clients(int is_child) {

	for (int i = 0; i < MAX_SESSION_COUNT; i++) {
		if (clients_to_handle[i] != NULL) {
			// Even though we're accessing a "critical" section, we mustn't intrudoce a mutex lock
			// because we risk a situation where a lot of worker threads are retrieving clients at the same time
			// and this would wait for them to unlock, which would make the signal not instantaneous.
			// But the signal MUST be instantaneous and destroy everything
			close(clients_to_handle[i]->fd_req);
			close(clients_to_handle[i]->fd_resp);
			close(clients_to_handle[i]->fd_notif);
			free(clients_to_handle[i]);
		}
	}
	
	if (!is_child) {
		printf("All clients terminated\n");
	}

}


/// Host thread function that handles the clients' connections
/// @param arg Path to the register pipe
/// @return NULL since the return of this function doesn't matter
void* clients_host_thread(void* arg) {

	if ((signal(SIGUSR1, terminate_all_clients)) == SIG_ERR) {
		fprintf(stderr, "Failed to set signal handler\n");
		return NULL;
	}

	char* reg_pipe_path = strdup((char*) arg);

	// Creates register pipe and opens it (waits for the first client to open it as well)
	if (mkfifo(reg_pipe_path, 0666) != 0) {
		if (errno == EEXIST) {
			fprintf(stderr, "Pipe %s already exists and may be in use, provide another one\n", reg_pipe_path);
		} else {
			fprintf(stderr, "Failed to create register FIFO\n");
		}
		free(reg_pipe_path);
		return NULL;
	}

	int fd_reg = open(reg_pipe_path, O_RDONLY);

	if (fd_reg == -1) {
		if (unlink(reg_pipe_path) != 0 && errno != ENOENT) {
			fprintf(stderr, "Failed to unlink register pipe");
		}
		fprintf(stderr, "Failed to open register FIFO\n");
		free(reg_pipe_path);
		return NULL;
	}
	
	char msg_sent[2];
	msg_sent[0] = OP_CODE_CONNECT;

	while (1) {

		if (wait_for_connection(fd_reg) != 0) {
			close(fd_reg);
			free(reg_pipe_path);
			return NULL;	
		}

		// Wait decrements, there is one less free spot
		sem_wait(&free_spots);

		// Locks the critical section
		// This is a critical section because we're accessing the global variable current_client_to_store
		// and many threads may be trying to access it at the same time
		pthread_mutex_lock(&current_client_lock);

		Client* client = (Client*) malloc(sizeof(Client)); 

		// Connect receives message from a client and opens their three pipes
		msg_sent[1] = establish_connection(fd_reg, client);
		
		// Answers the client
		if (write_all(client->fd_resp, msg_sent, sizeof(msg_sent)) == -1) {
			fprintf(stderr, "Failed to write to response FIFO\n");
			close(fd_reg);
			free(reg_pipe_path);
			return NULL;
		}

		// Stores client
		clients_to_handle[current_client_to_store] = client;
		// Makes buffer circular
		current_client_to_store = (current_client_to_store + 1) % MAX_SESSION_COUNT;

		pthread_mutex_unlock(&current_client_lock);

		// Post increments, there is one more pending client. The corresponding wait will be woken up
		sem_post(&pending_clients);
		
	}	

	close(fd_reg);
	free(reg_pipe_path);

	return NULL;
}


/// Handles the client's requests
/// @param client Client structure with the file descriptors of the client's pipes
void handle_client(Client* client) {
	
	int disconnected = 0;
	while (disconnected == 0) {
		char* client_keys[MAX_NUMBER_SUB] = {NULL};
		char msg[BUFSIZ] = {0};
		
		// Check if the file descriptor has been closed by another thread of this program
		// (in case the SIGUSR1 is received).
		// In this case we musn't print any error messages since the program is behaving as expected.
		if (fcntl(client->fd_req, F_GETFD) == -1 && errno == EBADF) {
			printf("hello??\n");
			return;
		}
		// Read only 1 byte, the char that contains the op code
		int ret = read_all(client->fd_req, msg, 1, NULL);  
		if (ret == -1) {
			fprintf(stderr, "Failed to read from request FIFO\n");
			return;
		// If read returns 0, 0 bytes were read (EOF) therefore client closed its pipes unexpectedely
		} else if (ret == 0) {
			close(client->fd_req);
			close(client->fd_resp);
			close(client->fd_notif);
			free(client);
			printf("Client disconnected\n");
			return; 
		}

		char op_code = msg[0];

		char msg_resp[2];
		int subscribed = 0;
		switch (op_code) {

			case OP_CODE_DISCONNECT: {

				msg_resp[0] = OP_CODE_DISCONNECT;
				msg_resp[1] = unsubscribe_all_keys(client->fd_notif, client_keys, &subscribed);
				
				if (write_all(client->fd_resp, msg_resp, sizeof(msg_resp)) == -1) {
					fprintf(stderr, "Failed to write to response FIFO\n");
				}

				// After unsubcribing all keys, we close the client's pipes
				close(client->fd_req);
				close(client->fd_resp);
				close(client->fd_notif);
				free(client);
				
				printf("Client disconnected\n");
				disconnected = 1;
				
				break;
			}

			case OP_CODE_SUBSCRIBE: {

				char key[MAX_STRING_SIZE+1];

				read_string(client->fd_req, key);

				msg_resp[0] = OP_CODE_SUBSCRIBE;
				msg_resp[1] = subscribe_key(key, client->fd_notif, client_keys, &subscribed);

				if (msg_resp[1] == '1') {
					printf("Client subscribed key \"%s\"\n", key);
				}

				if (write_all(client->fd_resp, msg_resp, sizeof(msg_resp)) == -1) {
					fprintf(stderr, "Failed to write to response FIFO\n");
				}

				break;
			}

			case OP_CODE_UNSUBSCRIBE: {

				char key[MAX_STRING_SIZE+1];
				
				read_string(client->fd_req, key);

				msg_resp[0] = OP_CODE_UNSUBSCRIBE;
				msg_resp[1] = unsubscribe_key(key, client->fd_notif, client_keys, &subscribed);

				if (msg_resp[1] == '0') {
					printf("Client unsubscribed key \"%s\"\n", key);
				}

				if (write_all(client->fd_resp, msg_resp, sizeof(msg_resp)) == -1) {
					fprintf(stderr, "Failed to write to response FIFO\n");
				}

				break;
			}

			default: {
				break;
			}
		}
	}
	return;
}


/// Retrieves a client from the buffer and handles it
/// @return NULL since the return of this function doesn't matter
void *retrieve_client() {

	if (pthread_sigmask(SIG_BLOCK, &mask_sigusr1, NULL) != 0) {
		fprintf(stderr, "Failed to block signal\n");
		return NULL;
	}


	while (1) {

		Client* client;

		// Wait decrements, there is one less pending client since it'll be handled now
		sem_wait(&pending_clients);

		// Locks the critical section
		// This is a critical section because we're accessing the global variable current_client_to_handle
		// and many threads may be trying to access it at the same time
		pthread_mutex_lock(&current_client_lock);

		client = clients_to_handle[current_client_to_handle];
		current_client_to_handle = (current_client_to_handle+1) % MAX_SESSION_COUNT; 

		pthread_mutex_unlock(&current_client_lock);

		handle_client(client);

		// Only after the client disconnects, there is a new free spot
		// Post increments, there is one more free spot. The corresponding wait will be woken up
		sem_post(&free_spots);

	}
	
	return NULL;
}


/// Initializes the kvs hashtable and some variables and starts the threads to run the program
/// @param argc Number of arguments passed as input
/// @param argv Array with the arguments passed as input 
/// @return 0 if the program runs successfully, 1 otherwise
int main(int argc, char* argv[]) {

	if (argc != 5) {
		fprintf(stderr, "Invalid number of arguments\n");
		return EXIT_FAILURE;
	}
	
	// Initialization of locks and semaphores
	if (pthread_mutex_init(&backup_lock, NULL) != 0) {
		fprintf(stderr, "Backup mutex failed to init\n");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&global_client_lock, NULL) != 0) {
		fprintf(stderr, "Global client mutex failed to init\n");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&current_client_lock, NULL) != 0) {
		fprintf(stderr, "Current client mutex failed to init\n");
		return EXIT_FAILURE;
	}

	if (sem_init(&pending_clients, 0, 0) != 0) {
		fprintf(stderr, "Pending clients semaphore failed to init\n");
		return EXIT_FAILURE;
	}

	if (sem_init(&free_spots, 0, MAX_SESSION_COUNT) != 0) {
		fprintf(stderr, "Free spots semaphore failed to init\n");
		return EXIT_FAILURE;
	}
	
	// to add signal SIGUSR1 to the mask
	sigemptyset(&mask_sigusr1);
	sigaddset(&mask_sigusr1, SIGUSR1);


	ReadFilesArgs args;
	
	args.og_path = argv[1];
	args.max_processes = atoi(argv[2]);
	args.pending_bck = 0;

	int max_threads = atoi(argv[3]);
	pthread_t threads[max_threads];
	pthread_t client_host_thread;
	pthread_t client_worker_threads[MAX_SESSION_COUNT];

	// Start host thread
	// Note that server is not supposed to wait for any clients before processing the .job files
	if (pthread_create(&client_host_thread, NULL, clients_host_thread, (void*) argv[4]) != 0) {
		fprintf(stderr, "Failed to create client host thread\n");
		return EXIT_FAILURE;
	}

	args.directory = opendir(args.og_path);
	
	if (args.directory == NULL) {
		fprintf(stderr, "Failed to open directory.\n");
		return EXIT_FAILURE;
	}

	if (kvs_init()) {
		fprintf(stderr, "Failed to initialize KVS.\n");
		return EXIT_FAILURE;
  	}
	

	printf("Processing of .job files has started\n");
	
	// We create the maximum number of threads right away.
	// They will all keep running until all files have been processed
	// When a thread finished a file it passes to the next
	for (int i = 0; i < max_threads; i++) {
		if (pthread_create(&threads[i], NULL, read_files, (void*) &args) != 0) {
			fprintf(stderr, "Failed to create thread %d\n", i);
			return EXIT_FAILURE;
		}
	}


	for (int i = 0; i < MAX_SESSION_COUNT; i++) {
		if (pthread_create(&client_worker_threads[i], NULL, retrieve_client, NULL) != 0) { 
			fprintf(stderr, "Failed to create thread %d\n", i);
			return EXIT_FAILURE;
		}
	}
	
	for (int i = 0; i < MAX_SESSION_COUNT; i++) {
		if (pthread_join(client_worker_threads[i], NULL) != 0) {
			fprintf(stderr, "Failed to join thread %d\n", i);
		}
	}

	// For each thread we create we need to join it, i.e., wait for it to finish
	for (int i = 0; i < max_threads; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			fprintf(stderr, "Failed to join thread %d (%ld)\n", i, threads[i]);
		}
	}

	// Retrieve client host thread
	if (pthread_join(client_host_thread, NULL) != 0) {
		fprintf(stderr, "Failed to join client host thread\n");
		return EXIT_FAILURE;
	}
	
	// For each child process still pending, we need to retrieve it. We might need to wait for it to finish
	for (int i = 0; i < args.pending_bck; i++) {
		wait(NULL);
	}	


	// Since the server is supposed to run forever, it will not be able to free and close everything properly
	
	// Frees the hashtable
	kvs_terminate();

	closedir(args.directory);

	// Destroy locks and semaphores
	pthread_mutex_destroy(&backup_lock);
	pthread_mutex_destroy(&global_client_lock);
	pthread_mutex_destroy(&current_client_lock);
	sem_destroy(&pending_clients);
	sem_destroy(&free_spots);

	return EXIT_SUCCESS;
}

