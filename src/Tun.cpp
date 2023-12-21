#include "Tun.hpp"
#include <sys/socket.h>
#include <netdb.h>


/*
 * Translate IPv4/IPv6 addr or hostname into struct addrinfo
 * If resolve error, try again for resolve_retry_seconds seconds.
 */
int
openvpn_getaddrinfo(unsigned int flags,
                    const char *hostname,
                    const char *servname,
                    int resolve_retry_seconds,
                    struct signal_info *sig_info,
                    int ai_family,
                    struct addrinfo **res)
{
    struct addrinfo hints;
    int status;
    struct signal_info sigrec = {0};
    int msglevel = (flags & GETADDR_FATAL) ? M_FATAL : D_RESOLVE_ERRORS;
    struct gc_arena gc = gc_new();
    const char *print_hostname;
    const char *print_servname;

    ASSERT(res);

    ASSERT(hostname || servname);
    ASSERT(!(flags & GETADDR_HOST_ORDER));

    if (servname)
    {
        print_servname = servname;
    }
    else
    {
        print_servname = "";
    }

    if (flags & GETADDR_MSG_VIRT_OUT)
    {
        msglevel |= M_MSG_VIRT_OUT;
    }

    if ((flags & (GETADDR_FATAL_ON_SIGNAL|GETADDR_WARN_ON_SIGNAL))
        && !sig_info)
    {
        sig_info = &sigrec;
    }

    /* try numeric ipv6 addr first */
    CLEAR(hints);
    hints.ai_family = ai_family;
    hints.ai_flags = AI_NUMERICHOST;

    if (flags & GETADDR_PASSIVE)
    {
        hints.ai_flags |= AI_PASSIVE;
    }

    if (flags & GETADDR_DATAGRAM)
    {
        hints.ai_socktype = SOCK_DGRAM;
    }
    else
    {
        hints.ai_socktype = SOCK_STREAM;
    }

    status = getaddrinfo(hostname, servname, &hints, res);

    if (status != 0) /* parse as numeric address failed? */
    {
        const int fail_wait_interval = 5; /* seconds */
        /* Add +4 to cause integer division rounding up (1 + 4) = 5, (0+4)/5=0 */
        int resolve_retries = (flags & GETADDR_TRY_ONCE) ? 1 :
                              ((resolve_retry_seconds + 4)/ fail_wait_interval);
        const char *fmt;
        int level = 0;

        if (hostname && (flags & GETADDR_RANDOMIZE))
        {
            hostname = hostname_randomize(hostname, &gc);
        }

        if (hostname)
        {
            print_hostname = hostname;
        }
        else
        {
            print_hostname = "undefined";
        }

        fmt = "RESOLVE: Cannot resolve host address: %s:%s (%s)";
        if ((flags & GETADDR_MENTION_RESOLVE_RETRY)
            && !resolve_retry_seconds)
        {
            fmt = "RESOLVE: Cannot resolve host address: %s:%s (%s) "
                  "(I would have retried this name query if you had "
                  "specified the --resolv-retry option.)";
        }

        if (!(flags & GETADDR_RESOLVE) || status == EAI_FAIL)
        {
            // msg(msglevel, "RESOLVE: Cannot parse IP address: %s:%s (%s)",
            //     print_hostname, print_servname, gai_strerror(status));
            goto done;
        }

        /*
         * Resolve hostname
         */
        while (true)
        {
#ifndef _WIN32
            /* force resolv.conf reload */
            res_init();
#endif
            /* try hostname lookup */
            hints.ai_flags &= ~AI_NUMERICHOST;
            dmsg(D_SOCKET_DEBUG,
                 "GETADDRINFO flags=0x%04x ai_family=%d ai_socktype=%d",
                 flags, hints.ai_family, hints.ai_socktype);
            status = getaddrinfo(hostname, servname, &hints, res);

            if (sig_info)
            {
                get_signal(&sig_info->signal_received);
                if (sig_info->signal_received) /* were we interrupted by a signal? */
                {
                    /* why are we overwriting SIGUSR1 ? */
                    if (signal_reset(sig_info, SIGUSR1) == SIGUSR1) /* ignore SIGUSR1 */
                    {
                        msg(level,
                            "RESOLVE: Ignored SIGUSR1 signal received during "
                            "DNS resolution attempt");
                    }
                    else
                    {
                        /* turn success into failure (interrupted syscall) */
                        if (0 == status)
                        {
                            ASSERT(res);
                            freeaddrinfo(*res);
                            *res = NULL;
                            status = EAI_AGAIN; /* = temporary failure */
                            errno = EINTR;
                        }
                        goto done;
                    }
                }
            }

            /* success? */
            if (0 == status)
            {
                break;
            }

            /* resolve lookup failed, should we
             * continue or fail? */
            level = msglevel;
            if (resolve_retries > 0)
            {
                level = D_RESOLVE_ERRORS;
            }

            msg(level,
                fmt,
                print_hostname,
                print_servname,
                gai_strerror(status));

            if (--resolve_retries <= 0)
            {
                goto done;
            }

            management_sleep(fail_wait_interval);
        }

        ASSERT(res);

        /* hostname resolve succeeded */

        /*
         * Do not choose an IP Addresse by random or change the order *
         * of IP addresses, doing so will break RFC 3484 address selection *
         */
    }
    else
    {
        /* IP address parse succeeded */
        if (flags & GETADDR_RANDOMIZE)
        {
            msg(M_WARN,
                "WARNING: ignoring --remote-random-hostname because the "
                "hostname is an IP address");
        }
    }

done:
    if (sig_info && sig_info->signal_received)
    {
        int level = 0;
        if (flags & GETADDR_FATAL_ON_SIGNAL)
        {
            level = M_FATAL;
        }
        else if (flags & GETADDR_WARN_ON_SIGNAL)
        {
            level = M_WARN;
        }
        msg(level, "RESOLVE: signal received during DNS resolution attempt");
    }

    gc_free(&gc);
    return status;
}

/*
 * Functions related to the translation of DNS names to IP addresses.
 */
static int
get_addr_generic(sa_family_t af, unsigned int flags, const char *hostname,
                 void *network, unsigned int *netbits,
                 int resolve_retry_seconds, struct signal_info *sig_info,
                 int msglevel)
{
    char *endp, *sep, *var_host = NULL;
    struct addrinfo *ai = NULL;
    unsigned long bits;
    uint8_t max_bits;
    int ret = -1;

    if (!hostname)
    {
        // msg(M_NONFATAL, "Can't resolve null hostname!");
        goto out;
    }

    /* assign family specific default values */
    switch (af)
    {
        case AF_INET:
            bits = 0;
            max_bits = sizeof(in_addr_t) * 8;
            break;

        case AF_INET6:
            bits = 64;
            max_bits = sizeof(struct in6_addr) * 8;
            break;

        default:
            // msg(M_WARN,
            //     "Unsupported AF family passed to getaddrinfo for %s (%d)",
            //     hostname, af);
            goto out;
    }

    /* we need to modify the hostname received as input, but we don't want to
     * touch it directly as it might be a constant string.
     *
     * Therefore, we clone the string here and free it at the end of the
     * function */
    var_host = strdup(hostname);
    if (!var_host)
    {
        // msg(M_NONFATAL | M_ERRNO,
            // "Can't allocate hostname buffer for getaddrinfo");
        goto out;
    }

    /* check if this hostname has a /bits suffix */
    sep = strchr(var_host, '/');
    if (sep)
    {
        bits = strtoul(sep + 1, &endp, 10);
        if ((*endp != '\0') || (bits > max_bits))
        {
            // msg(msglevel, "IP prefix '%s': invalid '/bits' spec (%s)", hostname,
            //     sep + 1);
            goto out;
        }
        *sep = '\0';
    }

    ret = openvpn_getaddrinfo(flags & ~GETADDR_HOST_ORDER, var_host, NULL,
                              resolve_retry_seconds, sig_info, af, &ai);
    if ((ret == 0) && network)
    {
        struct in6_addr *ip6;
        in_addr_t *ip4;

        switch (af)
        {
            case AF_INET:
                ip4 = (in_addr_t *)network;
                *ip4 = ((struct sockaddr_in *)ai->ai_addr)->sin_addr.s_addr;

                if (flags & GETADDR_HOST_ORDER)
                {
                    *ip4 = ntohl(*ip4);
                }
                break;

            case AF_INET6:
                ip6 = (in6_addr*)network;
                *ip6 = ((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr;
                break;

            default:
                /* can't get here because 'af' was previously checked */
                // msg(M_WARN,
                //     "Unsupported AF family for %s (%d)", var_host, af);
                goto out;
        }
    }

    if (netbits)
    {
        *netbits = bits;
    }

    /* restore '/' separator, if any */
    if (sep)
    {
        *sep = '/';
    }
out:
    freeaddrinfo(ai);
    free(var_host);

    return ret;
}

in_addr_t
getaddr(unsigned int flags,
        const char *hostname,
        int resolve_retry_seconds,
        bool *succeeded,
        struct signal_info *sig_info)
{
    in_addr_t addr;
    int status;

    status = get_addr_generic(AF_INET, flags, hostname, &addr, NULL,
                              resolve_retry_seconds, sig_info,
                              M_WARN);
    if (status==0)
    {
        if (succeeded)
        {
            *succeeded = true;
        }
        return addr;
    }
    else
    {
        if (succeeded)
        {
            *succeeded = false;
        }
        return 0;
    }
}

bool
get_ipv6_addr(const char *hostname, struct in6_addr *network,
              unsigned int *netbits, int msglevel)
{
    if (get_addr_generic(AF_INET6, GETADDR_RESOLVE, hostname, network, netbits,
                         0, NULL, msglevel) < 0)
    {
        return false;
    }

    return true;                /* parsing OK, values set */
}

namespace onix{
    TapTun::TapTun(){

    }
}