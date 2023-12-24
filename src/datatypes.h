#pragma once

#include <cstdint>
#include <vector>

const int L3_COMMUNICATION_PORT = 5124;
const int L2_COMMUNICATION_PORT = 5123;
const int L1_COMMUNICATION_PORT = 5122;

struct Node{
    char name[256];
    char address[256];
};

enum class ConnectionType{
    TCP = 1,
    UDP = 2,
    ICMP = 4,
};

inline ConnectionType operator|(ConnectionType a, ConnectionType b){
    return static_cast<ConnectionType>(static_cast<uint8_t>(a)|static_cast<uint8_t>(b));
}

inline ConnectionType operator&(ConnectionType a, ConnectionType b){
    return static_cast<ConnectionType>(static_cast<uint8_t>(a)&static_cast<uint8_t>(b));
}



using Client = Node;
using Resolver = Node;
using Proxy = Node;

struct GoldentTicket
{
    union 
    {
        char p[256];
        uint16_t d[256];
    } ticket;
    Proxy proxy;
};

using GTTable = std::vector<GoldentTicket>;
