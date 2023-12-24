#pragma once

#include <cstdint>
#include <vector>

#include "datatypes.h"

namespace onix
{
    struct proxy_state
    {
        bool orphan = false;
        uint8_t connection_types;  // JOIN OF ConnectionType
        std::vector<Resolver> resolvers;
        GTTable gt_table;
    };
    
    void run_proxy();
}