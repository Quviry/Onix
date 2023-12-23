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
    class TapTun
    {
        std::string actual_name;

        in_addr_t local;
        in_addr_t remote_netmask;

        int fd;

    public:
    TapTun();
    ~TapTun() = default;

    };
}

#ifndef TUN_FUNCS

#define TUN_FUNCS

// int tun_alloc()
// {
//     struct ifreq ifr;
//     int fd, err;
//     char *clonedev = "/dev/net/tun";

//     if ((fd = open(clonedev, O_RDWR)) < 0)
//     {
//         return fd;
//     }
//     memset(&ifr, 0, sizeof(ifr));

//     strncpy(ifr.ifr_name, "tap0", IFNAMSIZ);
//     ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

//     if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
//     {
//         close(fd);
//         return err;
//     }
//     printf("Opent tun/tap device %s for reading \n", ifr.ifr_name);

//     fcntl(fd, F_SETFL, O_NONBLOCK);
//     fcntl(fd, F_SETFD, O_CLOEXEC);

//     return fd;

//     // if (!dev)
//     //     return -1;

//     // memset(&ifr, 0, sizeof(ifr));
//     // /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
//     //  *        IFF_TAP   - TAP device
//     //  *
//     //  *        IFF_NO_PI - Do not provide packet information
//     //  *        IFF_MULTI_QUEUE - Create a queue of multiqueue device
//     //  */
//     // ifr.ifr_flags = IFF_TAP;
//     // strcpy(ifr.ifr_name, dev);

//     // for (i = 0; i < queues; i++)
//     // {
//     //     if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
//     //     {
//     //         for (--i; i >= 0; i--)
//     //             close(fds[i]);
//     //         return err;
//     //     }
//     //     err = ioctl(fd, TUNSETIFF, (void *)&ifr);
//     //     if (err)
//     //     {
//     //         for (; i >= 0; i--)
//     //             close(fds[i]);
//     //         return err;
//     //     }
//     //     fds[i] = fd;
//     // }
//     // return 0;
// }

// int tun_set_queue(int fd, int enable)
// {
//     struct ifreq ifr;

//     memset(&ifr, 0, sizeof(ifr));

//     if (enable)
//         ifr.ifr_flags = IFF_ATTACH_QUEUE;
//     else
//         ifr.ifr_flags = IFF_DETACH_QUEUE;

//     return ioctl(fd, TUNSETQUEUE, (void *)&ifr);
// }

#endif