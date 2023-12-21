#pragma once

#include <string>

namespace onix
{
    class Client
    {
        char *dev;
        const int queues = 3;
        int *file_descriptors;
        char buffer[1024];
        std::string state;

    public:
        Client();
        ~Client();
        void start();
    };
}