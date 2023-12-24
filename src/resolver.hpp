#pragma once

#include <map>
#include <vector>
#include <string>
#include "datatypes.h"
#include <chrono>

namespace onix
{
    using AccessMap = std::map<std::string, Proxy>;
    using PingTable = std::vector<std::pair<Proxy, std::chrono::time_point<std::chrono::system_clock>>>;

    struct resolver_state
    {
        int32_t max_proxy_num;
        uint32_t max_am_size;
        int32_t pingtable_expiration_time_seconds;
        uint8_t connection_types;  // JOIN OF ConnectionType
        PingTable ping_table;
        AccessMap gt_table;
    };


    void run_resolver();
}