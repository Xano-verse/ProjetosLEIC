// ------------------------------------------------
//                1ª parte projeto SO
// ------------------------------------------------
/**
 * Instituto Superior Tecnico - Campus Alameda
 * Curso: Licenciatura em Engenharia Informatica e de Computadores (LEIC - A)
 * Cadeira: Sistemas Operativos (SO)
 * Corpo Docente responsavel: Joao Garcia
 * Ano letivo 2024/2025
 * Alunos: Alexandre Delgado | ist1109441 
 * 		   Madalena Yang | ist1110206
 */

#include "constants.h"
#include "operations.h"
#include "parser.h"

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


pthread_mutex_t lock;


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

  
/// Reads the contents of a file and executes the commands in it
/// @param args Structure with arguments from the read_files() function
/// @param in_fd File descriptor of the file we're reading from
/// @param out_fd File descriptor of the output file, the one we'll write into
/// @param path Path to the input file. It will be modified to create the backup file
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
					
					pthread_mutex_lock(&lock);

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
					pthread_mutex_unlock(&lock);

					if (pid == 0) {

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
void* read_files(void* args_arg) {

	ReadFilesArgs* args = args_arg;

	struct dirent *dir_entry;

	// readdir automaitcally progresses to the next entry in the directory every time it is called. It keeps a global pointing
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


/// Initializes the kvs hashtable and some variables and starts the threads to run the program
/// @param argc Number of arguments passed as input
/// @param argv Array with the arguments passed as input 
/// @return 0 if the program runs successfully, 1 otherwise
int main(int argc, char* argv[]) {

	if (argc != 4) {
		fprintf(stderr, "Invalid number of arguments\n");
		return EXIT_FAILURE;
	}
	
	if (pthread_mutex_init(&lock, NULL) != 0) {
		fprintf(stderr, "Mutex failed to init\n");
		return EXIT_FAILURE;
	}
	
	ReadFilesArgs args;
	
	args.og_path = argv[1];
	args.max_processes = atoi(argv[2]);
	args.pending_bck = 0;

	int max_threads = atoi(argv[3]);
	pthread_t threads[max_threads];

	args.directory = opendir(args.og_path);
	
	if (args.directory == NULL) {
		fprintf(stderr, "Failed to open directory.\n");
		return EXIT_FAILURE;
	}

	if (kvs_init()) {
		fprintf(stderr, "Failed to initialize KVS.\n");
		return EXIT_FAILURE;
  	}

	// We create the maximum number of threads right away.
	// They will all keep running until all files have been processed
	// When a thread finished a file it passes to the next
	for (int i = 0; i < max_threads; i++) {
		if (pthread_create(&threads[i], NULL, read_files, (void*) &args) != 0) {
			fprintf(stderr, "Failed to create thread %d\n", i);
			return EXIT_FAILURE;
		}

	}

	// For each thread we create we need to join it, i.e., wait for it to finish
	for (int i = 0; i < max_threads; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			fprintf(stderr, "Failed to join thread %d (%ld)\n", i, threads[i]);
		}
	}
	
	// For each child process still pending, we need to retrieve it. We might need to wait for it to finish
	for (int i = 0; i < args.pending_bck; i++) {
		wait(NULL);
	}	

	// Frees the hashtable
	kvs_terminate();

	closedir(args.directory);

	pthread_mutex_destroy(&lock);
	
	return EXIT_SUCCESS;
}

