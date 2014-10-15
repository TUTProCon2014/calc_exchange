#include "../include/calc_exchange.hpp"
#include "../include/line_greedy_calc_exchange.hpp"
#include "../../guess_img/include/blocked_guess.hpp"
#include "../../guess_img/include/correlation.hpp"
#include "../../inout/include/inout.hpp"
#include "../../utils/include/types.hpp"
#include "../../utils/include/dwrite.hpp"

using namespace procon;

int main(){
    auto p_opt = utils::Problem::get("img1.ppm");

    if(!p_opt)
        p_opt = inout::get_problem_from_test_server(1);

    if(p_opt){
        const utils::Problem& p = *p_opt;

        // 復元に使うための、2つの画像のくっつき度合いを返す関数
        auto pred = guess::Correlator(p);

        // 復元
        auto idxs = blocked_guess::guess(p, pred);

        std::cout << std::endl;

        //計算した交換操作の文字列の表示
        for(auto s : calc_exchange::calc_exchange(idxs, p.select_cost(), p.change_cost(), p.max_select_times())){
            std::cout << s << std::endl;
        }

        
    }else
        std::cout << "死" << std::endl;

    return 0;
}
