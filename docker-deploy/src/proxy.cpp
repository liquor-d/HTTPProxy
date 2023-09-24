#include "proxy.hpp"
#include "server_utils.hpp"
#include "client_utils.hpp"
#include "client.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "cache.hpp"
#include "log.hpp"
#include "function.hpp"

#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

Cache cache;
Log proxy_log;
std::mutex cache_lock;

Proxy::Proxy(const char *port) : port(port) {}

void Proxy::start()
{
    ServerUtils su;
    int socket_fd = su.init_server(port);
    if (socket_fd == -1)
    {
        proxy_log.writeError("Cannot initiate proxy as server");
        return;
    }
    int client_connect_fd;
    int id = 0;
    while (1)
    {
        std::string ip;
        try
        {
            client_connect_fd = su.accept_connection(socket_fd, ip);
        }
        catch (std::exception &e)
        {
            proxy_log.writeError("Cannot accept connection from client");
            continue;
        }
        Client client(id++, ip, client_connect_fd);
        std::thread(handle_request, std::ref(client)).detach();
        // handle_request(client);
        // std::cout << "reach here handle request" << std::endl;
    }
}

void Proxy::handle_request(const Client &client)
{
    char request_buf[65536] = {0};
    int byte_count = recv(client.get_fd(), request_buf, sizeof(request_buf), 0);
    if (byte_count <= 0)
    {
        // TODO: error handling
        std::cout << "Request is invalid" << std::endl;
        try
        {
            send_400(client);
        }
        catch (std::exception &e)
        {
            proxy_log.writeError(client.get_id(), e.what());
        }
        return;
    }
    HttpRequest request = HttpRequest(std::string(request_buf, byte_count));
    std::string new_req_msg1 = ": \"" + request.start_line + "\" from ";
    std::string new_req_msg2 = " @ " + get_current_timestr();
    proxy_log.writeMsg(client.get_id(), new_req_msg1, client.get_ip(), new_req_msg2);
    std::cout << "Here is the received client request:\n"
              << request_buf << std::endl;
    const char *host = request.host.c_str();
    const char *port = request.port.c_str();
    std::cout << "Here is the host and the port:\n"
              << host << ":" << port << std::endl;
    proxy_log.writeNote(client.get_id(), "Host is " + request.host + " and port is " + request.port);

    ClientUtils cu;
    int socket_fd;
    try
    {
        socket_fd = cu.init_client(host, port);
    }
    catch (std::exception &e)
    {
        proxy_log.writeError(client.get_id(), e.what());
        return;
    }
    if (request.method == "CONNECT")
    {
        handle_connect(client, socket_fd, request);
    }
    else if (request.method == "GET")
    {
        handle_get(client, socket_fd, request_buf, byte_count, request);
    }
    else if (request.method == "POST")
    {
        handle_post(client, socket_fd, request_buf, byte_count);
    }
    // close(socket_fd);
    // close(client.get_fd());
}

void Proxy::handle_connect(const Client &client, int socket_fd, HttpRequest &request)
{
    proxy_log.writeMsg(client.get_id(),
                       ": Requesting \"" + request.start_line + "\" from " + request.host);
    int sbyte_count = send(client.get_fd(), "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
    proxy_log.writeMsg(client.get_id(), ": Responding \"HTTP/1.1 200 OK\"");
    fd_set readfds;
    int nfds =
        socket_fd > client.get_fd() ? socket_fd + 1 : client.get_fd() + 1;

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        FD_SET(client.get_fd(), &readfds);

        select(nfds, &readfds, NULL, NULL, NULL);
        int fd[2] = {socket_fd, client.get_fd()};
        for (int i = 0; i < 2; i++)
        {
            char buf[65536] = {0};
            if (FD_ISSET(fd[i], &readfds))
            {
                if (!transfer_data(fd[i], fd[1 - i], buf, sizeof(buf)))
                {
                    return;
                }
            }
        }
    }
    proxy_log.writeMsg(client.get_id(), ": Tunnel closed");
}

bool Proxy::transfer_data(int fd1, int fd2, char *buf, size_t buf_size)
{
    int recv_byte_count = recv(fd1, buf, buf_size, 0);
    if (recv_byte_count <= 0)
    {
        return false;
    }
    int send_byte_count = send(fd2, buf, recv_byte_count, 0);
    if (send_byte_count <= 0)
    {
        return false;
    }
    return true;
}

void Proxy::handle_get(const Client &client, int socket_fd, char *request_buf, int byte_count, HttpRequest &request)
{
    // req_server(socket_fd, request_buf, byte_count);
    // handle_get_resp(client, socket_fd, request);
    HttpResponse cache_response = cache.find_cache(request.start_line);
    if (cache_response.get_msg() == "")
    {
        proxy_log.writeMsg(client.get_id(), ": not in cache");
        req_server(socket_fd, request_buf, byte_count);
        handle_get_resp(client, socket_fd, request);
    }
    else
    {
        if (request.raw_content.find("no-cache") != std::string::npos || cache_response.is_no_cache())
        {
            // revalidate function true or false
            send_response(cache_response, client, socket_fd);
        }
        else
        {
            // checkexpire function true or false
            send_response(cache_response, client, socket_fd);
        }
    }
}

void Proxy::req_server(int socket_fd, char *request_buf, int byte_count)
{
    int sbyte_count = send(socket_fd, request_buf, byte_count, 0);
    if (sbyte_count < 0)
    {
        return;
    }
}

void Proxy::handle_get_resp(const Client &client, int socket_fd, HttpRequest &request)
{
    char server_buf[65536] = {0};
    int mbyte_count = recv(socket_fd, server_buf, sizeof(server_buf), 0);
    HttpResponse response(server_buf, mbyte_count);

    if (response.is_chunk())
    {
        int sbyte_count = send(client.get_fd(), server_buf, mbyte_count, 0);
        if (sbyte_count < 0)
        {
            return;
        }
        char chunked_buf[65536] = {0};
        while (true)
        { // receive and send remaining message
            if (!transfer_data(socket_fd, client.get_fd(), chunked_buf, sizeof(chunked_buf)))
            {
                break;
            }
        }
    }
    else
    {
        // content-length not specified, take it as whole message has been received

        std::string server_msg_str(server_buf, mbyte_count);
        // printCacheControls(response, connection.getID());           // print cache related note
        int content_len = get_content_length(server_buf, mbyte_count); // get content length
        if (content_len != -1)
        {                                                                                    // content_len specified
            std::string msg = get_full_msg(socket_fd, server_buf, mbyte_count, content_len); // get the entire message
            const char *send_buf = msg.c_str();
            response.set_msg(msg);
            int slen = send(client.get_fd(), send_buf, msg.size() + 1, 0);
            if (slen < 0)
            {
                // printLog(connection.getID(), ": ERROR send response fails");
                return;
            }
        }
        else
        { // content-length not specified, take it as whole message has been received
        int slen = send(client.get_fd(), server_buf, mbyte_count, 0);
        if (slen < 0)
        {
            // printLog(connection.getID(), ": ERROR send response fails");
            return;
        }
        }
        // checkAndCache(response, request.start_line, connection.getID());
        if (cache.check_cache(response, request.start_line))
        {
            cache.add_cache(request.start_line, response);
            cache.print();
        }
    }
    return;
}

int Proxy::get_content_length(char *server_msg, int mes_len)
{
    std::string msg(server_msg, mes_len);
    size_t pos;
    if ((pos = msg.find("Content-Length: ")) != std::string::npos)
    {
        size_t head_end = msg.find("\r\n\r\n");

        int part_body_len = mes_len - static_cast<int>(head_end) - 8;
        size_t end = msg.find("\r\n", pos);
        std::string content_len = msg.substr(pos + 16, end - pos - 16);
        int num = 0;
        for (size_t i = 0; i < content_len.length(); i++)
        {
            num = num * 10 + (content_len[i] - '0');
        }
        return num - part_body_len - 4;
    }
    return -1;
}

std::string Proxy::get_full_msg(int send_fd,
                                char *server_msg,
                                int mes_len,
                                int content_len)
{
    int total_len = 0;
    int len = 0;
    std::string msg(server_msg, mes_len);

    while (total_len < content_len)
    {
        char new_server_msg[65536] = {0};
        if ((len = recv(send_fd, new_server_msg, sizeof(new_server_msg), 0)) <= 0)
        {
            break;
        }
        std::string temp(new_server_msg, len);
        msg += temp;
        total_len += len;
    }
    return msg;
}

void Proxy::send_response(HttpResponse &cache_response, const Client &client, int socket_fd)
{
    std::string msg = cache_response.get_msg();
    send(client.get_fd(), msg.c_str(), msg.size() + 1, 0);
    // send(client.get_fd(), msg.data(), msg.size(), 0);
}

void Proxy::handle_post(const Client &client, int socket_fd, char *request_buf, int byte_count)
{
    int content_length = get_content_length(request_buf, byte_count);
    if (content_length != -1)
    {
        std::string full_request =
            get_full_msg(client.get_fd(), request_buf, byte_count, content_length);
        char send_request[full_request.length() + 1];
        strcpy(send_request, full_request.c_str());
        int sbyte_count = send(socket_fd, send_request, sizeof(send_request), MSG_NOSIGNAL);
        if (sbyte_count < 0)
        {
            return;
        }
        char response_buf[65536] = {0};
        int rbyte_count = recv(socket_fd, response_buf, sizeof(response_buf), MSG_WAITALL);
        if (rbyte_count >= 0)
        {
            HttpResponse response;
            response.parse_start_line(request_buf, byte_count);
            std::cout << "Response received from POST request:" << response_buf << std::endl;

            if (send(client.get_fd(), response_buf, rbyte_count, MSG_NOSIGNAL) > 0)
            {
                std::cout << "Response start line: " << response.get_start_line() << std::endl;
            }
            else
            {
                std::cout << "Error in responding to POST request" << std::endl;
            }
        }
        else
        {
            std::cout << "Error in receiving from server socket" << std::endl;
        }
    }
}

void Proxy::send_400(const Client &client)
{
    int sbyte_count = send(client.get_fd(), "HTTP/1.1 400 Bad Request\r\n\r\n", 28, 0);
    if (sbyte_count == -1)
    {
        proxy_log.writeError("Cannot send a 400 response");
        throw std::runtime_error(": ERROR cannot send a 400 response");
    }
    proxy_log.writeWarning(client.get_id(), ": WARNING Invalid request");
    std::string msg = ": Responding \"" + std::string("HTTP/1.1 400 Bad Request") + "\"";
    proxy_log.writeMsg(client.get_id(), msg);
}

void Proxy::send_502(const Client &client)
{
    int sbyte_count = send(client.get_fd(), "HTTP/1.1 502 Bad Gateway\r\n\r\n", 28, 0);
    if (sbyte_count == -1)
    {
        proxy_log.writeError("Cannot send a 502 response");
        throw std::runtime_error(": ERROR cannot send a 502 response");
    }
    proxy_log.writeWarning(client.get_id(), ": WARNING Invalid response");
    std::string msg = ": Responding \"" + std::string("HTTP/1.1 502 Bad Gateway") + "\"";
    proxy_log.writeMsg(client.get_id(), msg);
}

std::string Proxy::time_to_str(time_t time_struct)
{
    struct tm *asc_time = gmtime(&time_struct);
    const char *t = asctime(asc_time);
    std::string ts(t);
    size_t p = ts.find('\n');
    return ts.substr(0, p);
}

std::string Proxy::get_current_timestr()
{
    time_t curr_time = time(0);
    return time_to_str(curr_time);
}
