#pragma once

#include <cstdint>
#include <vector>

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
    union ticket
    {
        char p[256];
        uint16_t d[256];
    };
    Proxy proxy;
};

using GTTable = std::vector<GoldentTicket>;
