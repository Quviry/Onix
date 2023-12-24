#pragma once

#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/in.h>

namespace onix
{
    void run_tun();

#ifndef TUN_FUNCS

#define TUN_FUNCS

    int tun_alloc();

    int tun_set_queue(int fd, int enable);

#endif
}
