#pragma once

#include <vector>
#include "datatypes.h"

namespace onix
{
    bool ResolversList(std::vector<Resolver> &resolvers);
    void ShowAppLog();
    bool TokensList(GTTable &gt);
}