#include "tcp.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

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
};

bool check_address_valid(char buf[], size_t n)
    {
        int d = 3;
        uint32_t acc = 0;
        for (int i = 0; i < n; ++i)
        {
            if (buf[i] == '.')
            {
                d--;
                acc = 0;
            }
            else if (buf[i] >= '0' && buf[i] <= '9')
            {
                acc = acc * 10 + (buf[i] - '0');
            }else{
                return false;
            }
            if (d < 0 || acc > 255 || acc < 0)
            {
                return false;
            }
        }
        return true;
    }

int client_tcp_pipeline(
    std::string ip_address,
    uint16_t port,
    std::function<void(file_descriptor)> pipeline)
{
    struct sockaddr_in server;
    struct hostent *server_ip = gethostbyname(ip_address.c_str());

    // creating the socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        return socket_fd;
    }

    // AF INET for Internet domain
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, server_ip->h_addr, server_ip->h_length);

    // TCP port number
    server.sin_port = htons(port);

    // std::cout << "server.sin_addr" << server.sin_addr.s_addr << " " << server.sin_port << "\n"; 

    // std::cout << "Try to connect" << "\n";
    // connect to the server
    if (connect(socket_fd, (sockaddr *)&server, sizeof(server)) < 0)
    {
        return -1;
    }
    // std::cout << "Try to eoc" << "\n";
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    // std::cout << "Try to cont" << "\n";
    pipeline(socket_fd);
    return 0;
};

int server_tcp_pipeline(
    std::string ip_address,
    uint16_t port,
    std::function<void(file_descriptor)> pipeline)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        return socket_fd;
    }

    // to pass the struct in bind
    struct sockaddr_in server;

    // address family is AF INET in the Internet domain
    server.sin_family = AF_INET;

    // address can be a specific IP or INADDR ANY (special address (0.0.0.0) meaning “any address”)
    struct hostent *server_ip = gethostbyname(ip_address.c_str());
    memcpy(&server.sin_addr, server_ip->h_addr, server_ip->h_length);
    // server.sin_addr.s_addr = htonl(ip_convert(ip_address));

    // TCP port number
    server.sin_port = htons(port);
    // std::cout << "server.sin_addr" << server.sin_addr.s_addr << " " << server.sin_port << "\n"; 

    // binding socket socketFD to address
    bind(socket_fd, (sockaddr *)&server, sizeof(server));

    if (-1 == listen(socket_fd, MAX_PING_CONNECTIONS)){return -1;}
    // fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    pipeline(socket_fd);

    close(socket_fd);
    
    return 0;
};