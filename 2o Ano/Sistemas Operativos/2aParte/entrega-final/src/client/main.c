#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "src/client/api.h"
#include "src/common/constants.h"
#include "src/common/io.h"

// Global variable to check if the client is disconnected
int disconnected = 0;


// Each client has two threads: 
// thread[0] reads commands from stdin
// thread[1] receives notifications from the server and print the result in stdout
pthread_t threads[2];


/// Reads commands entered from stdin in a loop and executes the corresponding code
void *read_commands() {

    char keys[MAX_NUMBER_SUB][MAX_STRING_SIZE] = {0};
    unsigned int delay_ms;
    size_t num;

    while (1) {

        switch (get_next(STDIN_FILENO)) {

            case CMD_DISCONNECT:
                // After receiving the disconnect command, we set the global variable to 1
				disconnected = 1;

                if (kvs_disconnect() != 0) {
                    fprintf(stderr, "Failed to disconnect to the server\n");
                    return NULL;
                }
                printf("Disconnected from server\n");

                return NULL;

            case CMD_SUBSCRIBE:
                num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
                if (num == 0) {
                    fprintf(stderr, "Invalid command. See HELP for usage\n");
                    continue;
                }

                if (!kvs_subscribe(keys[0])) {
                    fprintf(stderr, "Command subscribe failed\n");
                } else {
					printf("Subscribe successful\n");
				}

                break;

            case CMD_UNSUBSCRIBE:
                num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
                if (num == 0) {
                    fprintf(stderr, "Invalid command. See HELP for usage\n");
                    continue;
                }

                if (kvs_unsubscribe(keys[0])) {
                    fprintf(stderr, "Command unsubscribe failed\n");
                } else {
					printf("Unsubscribe successful\n");
				}

                break;

            case CMD_DELAY:
                if (parse_delay(STDIN_FILENO, &delay_ms) == -1) {
                    fprintf(stderr, "Invalid command. See HELP for usage\n");
                    continue;
                }

                if (delay_ms > 0) {
                    printf("Waiting...\n");
                    delay(delay_ms);
                }
                break;

            case CMD_INVALID:
                fprintf(stderr, "Invalid command. See HELP for usage\n");
                break;

            case CMD_EMPTY:
                break;

            case EOC:
                break;
        }
    }
}


/// Reads from the notification pipe in a loop and prints the notifications to the client
/// @return NULL since the return of this function is not used
void *receive_notif() {

	while (1) {
        // The maximum size of a message received is
        // 2*MAX_STRING_SIZE + '(' + ',' + ')', so we add 3 to the size
		char msg_received[MAX_STRING_SIZE*2 + 3];
        
        // If the client is disconnected, he won't receive notifications anymore
		if (disconnected == 1) {
			break;
		}
	
		int ret = read_all(fd_notif, msg_received, sizeof(msg_received), NULL);
        // If read_all returns 0 means that it reached the end of the file (EOF)
		if (ret == 0) {
			if (disconnected == 0) {
				printf("Server terminated the connection\n");
			}

            // Close the file descriptors and unlink the pipes
			close(fd_req);
			close(fd_notif);
			close(fd_resp);
			unlink(req_pipe_saved);
			unlink(resp_pipe_saved);
			unlink(notif_pipe_saved);

			// We want to terminate the program. If we just did exit() there would be memory leaks,
			// so we cancel the other thread so it doesn't block the program
			pthread_cancel(threads[0]);
			return NULL;
		}
        // If read_all returns -1 means that it failed to read from the file descriptor
		else if (ret == -1) {
			fprintf(stderr, "Failed to read from notification FIFO\n");
			return NULL;
		}

		if (msg_received[0] == '(' && msg_received[strlen(msg_received) - 1] == ')') {	
			printf("%s\n", msg_received);
		}

	}

	return NULL;
}


/// Initializes the pipes's paths and the two threads that will manage this client
/// @return 0 on success, 1 on error
int main(int argc, char *argv[]) {
    if (argc < 3) {
    	fprintf(stderr, "Usage: %s <client_unique_id> <register_pipe_path>\n", argv[0]);
    	return EXIT_FAILURE;
	}

    char req_pipe_path[256] = "/tmp/req";
    char resp_pipe_path[256] = "/tmp/resp";
    char notif_pipe_path[256] = "/tmp/notif";

	// In case we don't have permissions to use /tmp/
    // char req_pipe_path[256] = "./req";
    // char resp_pipe_path[256] = "./resp";
    // char notif_pipe_path[256] = "./notif";
	
    // Add client id to the pipe paths
	strncat(req_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
    strncat(resp_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
    strncat(notif_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));


	printf("Connecting...\n");

    if (kvs_connect(req_pipe_path, resp_pipe_path, argv[2], notif_pipe_path) != 0) {
        fprintf(stderr, "Failed to connect to the server\n");
        return EXIT_FAILURE;
    }

    printf("Connected to server\n");


    // Create threads of the client
    if (pthread_create(&threads[0], NULL, read_commands, NULL) != 0) {
        fprintf(stderr, "Failed to create thread to read commands\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&threads[1], NULL, receive_notif, NULL) != 0) {
         fprintf(stderr, "Failed to create thread to receive notifications\n");
         return EXIT_FAILURE;
    }


    // Wait for threads to finish
    if (pthread_join(threads[0], NULL) != 0) {
        fprintf(stderr, "Failed to join thread to read commands\n");
        return EXIT_FAILURE;
    }

    if (pthread_join(threads[1], NULL) != 0) {
        fprintf(stderr, "Failed to join thread to receive notifications\n");
        return EXIT_FAILURE;
    }
	
    return EXIT_SUCCESS;
}

