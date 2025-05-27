#include "api.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/common/constants.h"
#include "src/common/io.h"
#include "src/common/protocol.h"


int fd_req = -1, fd_resp = -1, fd_notif = -1, fd_reg = -1;

char req_pipe_saved[MAX_PIPE_PATH_LENGTH];
char resp_pipe_saved[MAX_PIPE_PATH_LENGTH];
char notif_pipe_saved[MAX_PIPE_PATH_LENGTH];


/// @brief Creates the pipes with the given paths.
/// If the pipes already exist, it will unlink it and create a new one
/// @param req_pipe_path Path to the request pipe
/// @param resp_pipe_path Path to the response pipe
/// @param notif_pipe_path Path to the notification pipe
int make_pipes(char const *req_pipe_path, char const *resp_pipe_path,
               char const *notif_pipe_path) {

    // Creates the pipes
    if (mkfifo(req_pipe_path, 0666) == -1) {
        if (errno == EEXIST) {
			fprintf(stderr, "Pipe %s already exists and may be in use, provide another one\n", req_pipe_path);
        } else {
            fprintf(stderr, "Failed to create request pipe\n");
        }
        return EXIT_FAILURE;
    }
    
    if (mkfifo(resp_pipe_path, 0666) == -1) {
        if (errno == EEXIST) {
			fprintf(stderr, "Pipe %s already exists and may be in use, provide another one\n", resp_pipe_path);
        } else {
            fprintf(stderr, "Failed to create response pipe\n");
        }
        if (unlink(req_pipe_path) != 0 && errno != ENOENT) {
            fprintf(stderr, "Failed to unlink request pipe");
        }
        return EXIT_FAILURE;
    }
    
    if (mkfifo(notif_pipe_path, 0666) == -1) {
        if (errno == EEXIST) {
			fprintf(stderr, "Pipe %s already exists and may be in use, provide another one\n", notif_pipe_path);
        } else {
            fprintf(stderr, "Failed to create notification pipe\n");
        }
        if ((unlink(req_pipe_path) != 0 || unlink(resp_pipe_path)) && (errno != ENOENT)) {
            fprintf(stderr, "Failed to unlink pipes\n");
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}


/// @brief Opens the pipes with the given paths.
/// @param req_pipe_path Path to the request pipe
/// @param resp_pipe_path Path to the response pipe
/// @param notif_pipe_path Path to the notification pipe
int open_pipes(char const *req_pipe_path, char const *resp_pipe_path,
               char const *notif_pipe_path) {

    fd_req = open(req_pipe_path, O_WRONLY);
    if (fd_req == -1) {
        fprintf(stderr, "Failed to open request pipe\n");
        return EXIT_FAILURE;
    }

    fd_resp = open(resp_pipe_path, O_RDONLY);
    if (fd_resp == -1) {
        fprintf(stderr, "Failed to open response pipe\n");
        return EXIT_FAILURE;
    }

    fd_notif = open(notif_pipe_path, O_RDONLY);
    if (fd_notif == -1) {
        fprintf(stderr, "Failed to open notification pipe\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/// @brief Prints the response of the server to the client
/// @param op_code op_code of the operation
/// @param result Result of the operation
void print_server_response(char op_code, char result) {
	char* operation[] = {"connect", "disconnect", "subscribe", "unsubscribe"};
	
	printf("Server returned %c for operation: %s\n", result, operation[op_code - '0' - 1]);
}


int kvs_connect(char const *req_pipe_path, char const *resp_pipe_path,
                char const *server_pipe_path, char const *notif_pipe_path) {
    
    if (make_pipes(req_pipe_path, resp_pipe_path, notif_pipe_path) != 0) {
        return EXIT_FAILURE;
    }

	fd_reg = open(server_pipe_path, O_WRONLY);
    if (fd_reg == -1) {
        fprintf(stderr, "Failed to open server pipe\n");
        if ((unlink(req_pipe_path) != 0 || unlink(resp_pipe_path) != 0 ||
            unlink(notif_pipe_path) != 0) && (errno != ENOENT)) {
            fprintf(stderr, "Failed to unlink pipes\n");
        }
        return EXIT_FAILURE;
    }

    // Save the pipe paths to unlink them later in kvs_disconnect
    strcpy(req_pipe_saved, req_pipe_path);
    strcpy(resp_pipe_saved, resp_pipe_path);
    strcpy(notif_pipe_saved, notif_pipe_path);

    char op_code_connect = OP_CODE_CONNECT;

    size_t buffer_size = 1 + strlen(req_pipe_path) + 1 + strlen(resp_pipe_path) +
                         1 + strlen(notif_pipe_path) + 1;
    char msg_sent[buffer_size];
	msg_sent[0] = op_code_connect;

    // Copy the strings into the buffer, including the null terminators
    char *ptr = msg_sent + 1;

    memcpy(ptr, req_pipe_path, strlen(req_pipe_path) + 1);  // + 1 to include the '\0'
    ptr += strlen(req_pipe_path) + 1;

    memcpy(ptr, resp_pipe_path, strlen(resp_pipe_path) + 1);  // + 1 to include the '\0'
    ptr += strlen(resp_pipe_path) + 1;

    memcpy(ptr, notif_pipe_path, strlen(notif_pipe_path) + 1);  // + 1 to include the '\0'
	

    if (write_all(fd_reg, msg_sent, sizeof(msg_sent)) == -1) {
        fprintf(stderr, "Failed to register client\n");
        return EXIT_FAILURE;
    }

    if (open_pipes(req_pipe_path, resp_pipe_path, notif_pipe_path) != 0) {
        return EXIT_FAILURE;
    }
	
    char msg_received[2];
    char result, op_code;
    if (read_all(fd_resp, msg_received, sizeof(msg_received), NULL) == -1) {
        fprintf(stderr, "Failed to read response from server\n");
        return EXIT_FAILURE;
    }

    sscanf(msg_received, "%c%c", &op_code, &result);
	
	print_server_response(op_code, result);
    if (op_code == OP_CODE_CONNECT && result == '0') {
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }

}


int kvs_disconnect(void) {

    char msg_sent[1];
    msg_sent[0] = OP_CODE_DISCONNECT;

    if (write_all(fd_req, msg_sent, sizeof(msg_sent)) == -1) {
        fprintf(stderr, "Failed to send disconnect message to server\n");
        return EXIT_FAILURE;
    }
    
    char msg_received[2];
    if (read_all(fd_resp, msg_received, sizeof(msg_received), NULL) == -1) {
        fprintf(stderr, "Failed to read response from server\n");
        return EXIT_FAILURE;
    }
	
    char op_code, result;
    sscanf(msg_received, "%c%c", &op_code, &result);

	print_server_response(op_code, result);
    if (op_code == OP_CODE_DISCONNECT && result == '0') {
        
        // close pipes
        close(fd_req);
        close(fd_resp);
        close(fd_notif);
        close(fd_reg);

        // unlink pipes
        unlink(req_pipe_saved);
        unlink(resp_pipe_saved);
        unlink(notif_pipe_saved);

        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }

}


int kvs_subscribe(const char *key) {

    char msg_sent[MAX_STRING_SIZE + 1] = {0};
    sprintf(msg_sent, "%c%s", OP_CODE_SUBSCRIBE, key);

    if (write_all(fd_req, msg_sent, sizeof(key) + 1) == -1) {
        fprintf(stderr, "Failed to send subscription message to server\n");
        return 0;
    }
    
    char msg_received[2];
    if (read_all(fd_resp, msg_received, sizeof(msg_received), NULL) == -1) {
        fprintf(stderr, "Failed to read response from server\n");
        return 0;
    }

    char op_code, result;
    sscanf(msg_received, "%c%c", &op_code, &result);

	print_server_response(op_code, result);
    if (op_code == OP_CODE_SUBSCRIBE && result == '1') {
        return EXIT_FAILURE;
    }
    else {
        return EXIT_SUCCESS;
    }

}


int kvs_unsubscribe(const char *key) {

    char msg_sent[MAX_STRING_SIZE + 1] = {0};
    sprintf(msg_sent, "%c%s", OP_CODE_UNSUBSCRIBE, key);

    if (write_all(fd_req, msg_sent, sizeof(key) + 1) == -1) {
        fprintf(stderr, "Failed to send unsubscription message to server\n");
        return EXIT_FAILURE;
    }

    char msg_received[2];
    if (read_all(fd_resp, msg_received, sizeof(msg_received), NULL) == -1) {
        fprintf(stderr, "Failed to read response from server\n");
        return EXIT_FAILURE;
    }

    char op_code, result;
    sscanf(msg_received, "%c%c", &op_code, &result);

	print_server_response(op_code, result);
    if (op_code == OP_CODE_UNSUBSCRIBE && result == '0') {
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }

}

