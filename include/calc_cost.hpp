#pragma once


#include <vector>
#include <string>

namespace procon { namespace calc_exchange {

size_t calc_cost(std::vector<std::string> const & ans, int select_costrate, int exchange_costrate)
{
    size_t sum = 0;

    sum += (ans.size() - 1) / 3 * select_costrate;
    for(size_t i = 3; i < ans.size(); i += 3)
        sum += ans[i].size() * exchange_costrate;

    return sum;
}

}}
