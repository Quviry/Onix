#pragma execution_character_set('utf-8')

#include "src/client.h"
#include "src/proxy.hpp"
#include "src/resolver.hpp"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

void print_help()
{
    printf(
        "Usage: Onix [MODE [OPTION]...] \n"
        "MODE: \n"
        "--client, -c Client mode, default \n"
        "--proxy, -p Proxy mode \n"
        "--resolver, -r Resolver mode \n");
}

int main(int argc, char *argv[])
{

    if (argc == 0)
    {
        printf("Wrong arguments number");
        return -1;
    }

    if (argc == 1)
    {
        onix::run_client();
    }

    else
    {
        if (!strcmp(argv[1], "--client") || !strcmp(argv[1], "-c"))
        {
            (void)daemon(0, 0);

            onix::run_client();
        }
        if (!strcmp(argv[1], "--proxy") || !strcmp(argv[1], "-p"))
        {
            (void)daemon(0, 0);

            onix::run_proxy();
        }

        if (!strcmp(argv[1], "--resolver") || !strcmp(argv[1], "-r"))
        {
            (void)daemon(0, 0);

            onix::Resolver resolver{};
            resolver.start();
        }
        if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
        {
            print_help();
            return 0;
        }
        printf("Onix: invalid option %s\n", argv[1]);
        printf("Try 'Onyx --help' or 'Onix -h' for more information.\n");
        return -1;
    }

    return 0;
}