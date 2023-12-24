#pragma once

#include "datatypes.h"

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
    };
    
    
    void run_client();

}