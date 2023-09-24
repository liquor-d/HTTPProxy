#ifndef __PROXY_HPP__
#define __PROXY_HPP__

#include "client.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

class Proxy
{
private:
    const char *port;

public:
    Proxy(const char *port);
    void start();
    static void handle_request(const Client &client);
    static void handle_connect(const Client &client, int socket_fd, HttpRequest &request);
    // void handle_get(const Client &client, int socket_fd, const HttpRequest &request, char *request_buf, int byte_count);
    static bool transfer_data(int fd1, int fd2, char *buf, size_t buf_size);
    static void handle_get(const Client &client, int socket_fd, char *request_buf, int byte_count, HttpRequest &request);
    static void req_server(int socket_fd, char *request_buf, int byte_count);
    static void handle_get_resp(const Client &client, int socket_fd, HttpRequest &request);
    static int get_content_length(char *server_buf, int mbyte_count);
    static std::string get_full_msg(int send_fd, char *server_msg, int mes_len, int content_len);
    static void send_response(HttpResponse &cache_response, const Client &client, int socket_fd);
    static void handle_post(const Client &client, int socket_fd, char *request_buf, int byte_count);
    static void send_400(const Client &client);
    static void send_502(const Client &client);
    static std::string get_current_timestr();
    static std::string time_to_str(time_t time_struct);
};

#endif