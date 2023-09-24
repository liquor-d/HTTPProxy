#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include <string>

/**
 * This class contains the utility functions used when the proxy is the server
 * that accepts requests from the true client.
 */
class ServerUtils {
    public:
    int init_server(const char * port);
    int accept_connection(int socket_fd, std::string & ip);
};

#endif