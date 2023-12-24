#include "tcp.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

const int MAX_PING_CONNECTIONS = 10;

uint32_t ip_convert(const std::string &s)
{
    uint32_t r = 0;
    for (auto &c : s)
    {
        if (c == '.')
        {
            r <<= 8;
        }
        else
        {
            r = r * 10 + c - '0';
        }
    }
    return r;
}

int client_tcp_pipeline(
    std::string ip_addess,
    uint16_t port,
    std::function<void(file_descriptor)> pipeline)
{
    struct sockaddr_in server;

    // creating the socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        return socket_fd;
    }

    // AF INET for Internet domain
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, ip_addess.c_str(), ip_addess.length());

    // TCP port number
    server.sin_port = htons(port);

    // connect to the server
    if (connect(socket_fd, (sockaddr *)&server, sizeof(server)) < 0)
    {
        return -1;
    }
    pipeline(socket_fd);
    return 0;
};

int server_tcp_pipeline(
    std::string ip_address,
    uint16_t port,
    std::function<void(file_descriptor)>)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        return socket_fd;
    }

    // to pass the struct in bind
    struct sockaddr_in server;

    // address family is AF INET in the Internet domain
    server.sin_family = AF_INET;

    // address can be a specific IP or INADDR ANY (special address (0.0.0.0) meaning “any address”)
    server.sin_addr.s_addr = htonl(ip_convert(ip_address));

    // TCP port number
    server.sin_port = htons(port);

    // binding socket socketFD to address
    if (bind(socket_fd, (sockaddr *)&server, sizeof(server)) < 0)
    {
        return socket_fd;
    }
    listen(socket_fd, MAX_PING_CONNECTIONS);
    return 0;
};