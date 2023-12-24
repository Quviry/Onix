#pragma once

#include "datatypes.h"
#include <optional>

namespace onix
{
    struct client_state
    {
        bool auto_connection_enabled;
        bool fast_connection_enabled;
        bool gt_priority;
        ConnectionType con_type;
        std::vector<Resolver> resolvers;
        GTTable gt_table;
        std::optional<Proxy> active_proxy = std::nullopt;
    };
    
    
    void run_client();

}