#ifndef CLIENT_API_H
#define CLIENT_API_H

#include <stddef.h>

#include "src/common/constants.h"


/// File descriptors for the pipes
extern int fd_req, fd_resp, fd_notif, fd_reg;
/// Paths to the pipes saved
extern char req_pipe_saved[MAX_PIPE_PATH_LENGTH],
            resp_pipe_saved[MAX_PIPE_PATH_LENGTH],
            notif_pipe_saved[MAX_PIPE_PATH_LENGTH];

/// Connects to a KVS server.
/// @param req_pipe_path Path to the name pipe to be created for requests.
/// @param resp_pipe_path Path to the name pipe to be created for responses.
/// @param server_pipe_path Path to the name pipe where the server is listening.
/// @return 0 if the connection was established successfully, 1 otherwise.
int kvs_connect(char const *req_pipe_path, char const *resp_pipe_path,
                char const *server_pipe_path, char const *notif_pipe_path);

/// Disconnects from a KVS server.
/// @return 0 in case of success, 1 otherwise.
int kvs_disconnect(void);

/// Requests a subscription for a key.
/// @param key Key to be subscribed
/// @return 1 if the key was subscribed successfully (key existing), 0 otherwise.
int kvs_subscribe(const char *key);

/// Remove a subscription for a key.
/// @param key Key to be unsubscribed
/// @return 0 if the key was unsubscribed successfully  (subscription existed
/// and was removed), 1 otherwise.
int kvs_unsubscribe(const char *key);

#endif // CLIENT_API_H
