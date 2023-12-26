#pragma once
#include <functional>
#include <cstdint>
#include <string>

using file_descriptor = int; 

bool check_address_valid(char buf[], size_t n);

int client_tcp_pipeline(
    std::string ip_addess="127.0.0.1",
    uint16_t port=4000,
    std::function<void(file_descriptor)> = [](file_descriptor){}
);


int server_tcp_pipeline(
    std::string ip_address="0.0.0.0",
    uint16_t port=4000,
    std::function<void(file_descriptor)> = [](file_descriptor){}
);

