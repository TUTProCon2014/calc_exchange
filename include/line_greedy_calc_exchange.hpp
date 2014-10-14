#pragma once

#include "../../inout/include/inout.hpp"
#include "../../utils/include/template.hpp"
#include "../../utils/include/types.hpp"
#include "../../utils/include/range.hpp"

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <stack>
#include <unordered_map>

namespace procon { namespace line_greedy_calc_exchange{

using namespace utils;


//解答生成クラス
class Answer{
private:
    std::vector<std::string> _ans;      //解答
    Index2D _select;                    //選択中の断片座標
    ImageID _select_id;                 //選択中の断片id
    int _select_num;                    //選択数
	int _exchange_num;					//交換数
    std::stack<Direction> _final_move;      //最後のselect移動処理

public:
    //コンストラクタ
    Answer(){
        _ans.push_back("");                 //選択回数用にあけておく
        _select = makeIndex2D(100, 100);    //100,100にしておく
        _select_num = 0;
		_exchange_num = 0;
    }

    //選択操作
    void set_select(Index2D s){
        _select = s;                        //選択断片のセット
        _select_num++;                      //選択数をカウント

        //選択操作を解答に追加
        std::stringstream ss;
        ss << std::hex << std::uppercase << _select[1] << _select[0]; //16進数変換
        _ans.push_back(ss.str());                   //選択画像位置を解答に追加
        _ans.push_back("");                         //交換回数用にあけておく
        _ans.push_back("");                         //交換操作格納用
    }

    //選択idのセット
    void set_selectID(ImageID id){
        _select_id = id;
    }

    //交換操作
    void exchange(std::string dir){
        _ans[_ans.size()-1] += dir;
		_exchange_num++;
    }

    //選択断片位置のチェンジ(解答には追加しない)
    void move_select(Index2D s){
        _select = s;
    }

    //交換操作文字列の最後の文字をクリア
    void remove(){
		_exchange_num--;
        //_ans[_ans.size()-1].erase(--_ans[_ans.size()-1].end());
        auto& str = _ans[_ans.size()-1];
        if(str.size())
            str.resize(str.size()-1);
    }

	//これから入れようとしている交換操作に意味があるかのチェック
	bool is_meaningless(Direction dir){
		auto& str = _ans[_ans.size()-1];

		//これから入れようとしている交換操作と反対方向の交換が入っているならtrueを返す
        switch((size_t)dir){
            case (size_t)Direction::up :
				if(str.size() > 0 && str[str.size()-1] == 'D'){
					return true;
				}
                break;
            case (size_t)Direction::right :
				if(str.size() > 0 && str[str.size()-1] == 'L'){
					return true;
				}
                break;
            case (size_t)Direction::down :
				if(str.size() > 0 && str[str.size()-1] == 'U'){
					return true;
				}
                break;
            case (size_t)Direction::left :
				if(str.size() > 0 && str[str.size()-1] == 'R'){
					return true;
				}
                break;
        }

		return false;
	}

    //最終処理
    void finish(){

        _ans[0] += std::to_string(_select_num); //選択回数の代入

        size_t idx = 1;             //ansの行番号
        for(size_t i=0; i < _select_num; i++){
            idx++;                                              //選択断片位置の行
            _ans[idx] = std::to_string(_ans[idx+1].size()); //交換回数の代入
            idx++;
            idx++;                                              //交換操作の行
        }

    }

    //最後のselect移動処理にpush
    void final_move_push(Direction dir){
        _final_move.push(dir);
    }

    //最後のselect移動処理のpop
    Direction final_move_pop(){
        auto ret = _final_move.top();
        _final_move.pop();

        return ret;
    }

    //ゲッター
    const std::vector<std::string> ans() const {
        return _ans;
    }

    Index2D select() const {
        return _select;
    }

    ImageID select_id() const {
        return _select_id;  
    }

    std::stack<Direction> final_move() const {
        return _final_move;
    }

	int select_num() const {
		return _select_num;
	}

	int exchange_num() const {
		return _exchange_num;
	}
};

typedef struct _Rect{
    size_t left;    //左上隅のx座標
    size_t top;     //左上隅のy座標
    size_t right;   //右下隅のx座標
    size_t bottom;  //右下隅のy座標

    _Rect(size_t l, size_t t, size_t r, size_t b) : left(l), top(t), right(r), bottom(b){

    }
}Rect;


//prototype宣言
void move_piece(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>const & used, std::vector<std::vector<ImageID>>& target, const Index2D& tgt, const Index2D& dst, const Rect& scope);
std::vector<std::vector<ImageID>> make_start_state(const size_t & height, const size_t & width);
Index2D search_piece(ImageID piece, const std::vector<std::vector<ImageID>> & from);
void target_piece_clear(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt, Index2D dist, const Direction dir, const Rect& scope);
void exchange(Answer& ans, std::vector<std::vector<ImageID>>& state, const Index2D moved, Direction dir);
void parking_in_garage(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, ImageID tgt1id, ImageID tgt2id, const Rect& scope, bool prev_f);
ImageID makeImageID(size_t i, size_t j);
void first_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const Rect& scope, Direction next);
void second_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width);
void final_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const std::vector<std::vector<ImageID>>& true_target, const Rect& scope);
bool is_state_same(const std::vector<std::vector<ImageID>>& a, const std::vector<std::vector<ImageID>>& b);
Direction eval_edge(Answer& ans, std::vector<std::vector<ImageID>>& state, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width);
void transform_target(Answer& ans, std::vector<std::vector<ImageID>>& target, const Rect scope, Direction dir);
void move_target(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, int disty, int distx, Index2D tgt, const Rect& scope);
void special_move(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, const Rect& scope);
void div_tgts(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, const Rect& scope);
void solver(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const std::vector<std::vector<ImageID>>& true_target, const size_t height, const size_t width);
void lux_move(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>const & used, std::vector<std::vector<ImageID>>& target, int max_select_times, const int height, const int width);


//ImageIDの生成
ImageID makeImageID(size_t i, size_t j){
    ImageID t(i, j);
    return t;
}

//スタート状態の生成
std::vector<std::vector<ImageID>> make_start_state(const size_t & height, const size_t & width){
    std::vector<std::vector<ImageID>> start(height, std::vector<ImageID>(width)); //スタート状態
    for(size_t i=0; i < height; i++){
        for(size_t j=0; j < width; j++){
            start[i][j] = makeImageID(i, j);
        }
    }

    return start;
}

//pieceをfromから探し、その座標を返す
Index2D search_piece(ImageID piece, const std::vector<std::vector<ImageID>> & from){
    for(size_t i=0; i < from.size(); i++){
        for(size_t j=0; j < from[0].size(); j++){
            if(from[i][j].get_index() == piece.get_index()){
                return makeIndex2D(i, j);
            }
        }
    }
}

//tgtの移動
void move_target(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, int disty, int distx, Index2D tgt, const Rect& scope){
                auto sel = ans.select();
                //tgtの位置を通れないようにする(そうしないとせっかくselectを移動させてもそこにtgtがいなくなってしまう)
                used[tgt[0]][tgt[1]] = true;    
                //selectをtgtの移動したい位置にもっていく
                move_piece(ans, state, used, target, tgt, makeIndex2D(disty, distx), scope);    
                //usedをもとにもどす
                used[tgt[0]][tgt[1]] = false;   
}

//tgtの断片をdistの位置まで持っていく
//dir:今つめている方向
void target_piece_clear(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt, Index2D dist, const Direction dir, const Rect& scope){

    size_t dy = std::abs((int)tgt[0] - (int)dist[0]);       //目的位置までのy距離
    size_t dx = std::abs((int)tgt[1] - (int)dist[1]);       //目的位置までのx距離
    Direction ydir = (dist[0] < tgt[0]) ? Direction::up : Direction::down;      //移動するべき方向の決定
    Direction xdir = (dist[1] < tgt[1]) ? Direction::left : Direction::right;
    int my[4] = {0, -1, 0, 1};  //移動量
    int mx[4] = {1, 0, -1, 0};
    size_t move_cnt = 0;        //xかyかどちらに移動するかを決定するためのカウント値
    //目的位置までの距離を示すポインタ
    size_t* d1;
    size_t* d2;
    //進む方向のポインタ
    Direction* dir1;
    Direction* dir2;
    //はじめにx移動かどうかのフラグ
    bool first_x = false;

    //方向によってどちらの座標を先に合わせるかを変える
    switch((size_t)dir){
        case (size_t)Direction::up :
            d1 = &dx;
            d2 = &dy;
            first_x = true;
            break;
        case (size_t)Direction::right :
            d1 = &dy;
            d2 = &dx;
            first_x = false;
            break;
        case (size_t)Direction::down :
            d1 = &dx;
            d2 = &dy;
            first_x = true;
            break;
        case (size_t)Direction::left :
            d1 = &dy;
            d2 = &dx;
            first_x = false;
            break;
    }

    //移動処理
    //目的地のy座標+1までx移動、y移動、x移動、y移動・・・の順に動いたほうが交換回数減る
    while(*d2 > 1){ //目的地までのy距離=1になるまでこの動作

        if(*d1 > 0 && move_cnt % 2 == 0){   //d1方向への移動
            (*d1)--;    //あとで使うので移動した分減らしておく
            
            //tgtの移動
            if(first_x){
                move_target(ans, state, used, target, (int)tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt, scope); 
            }else{
                move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], (int)tgt[1], tgt, scope); 
            }

            //selectとtgtの交換
            Index2D moved = tgt;    //selectの移動先
            tgt = ans.select();     //tgtの座標の変更

            if(first_x){    //最初にx移動するとき
                if(xdir == Direction::left){
                    exchange(ans, state, moved, Direction::right);      //xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
                }else{
                    exchange(ans, state, moved, Direction::left);
                }
            }else{          //最初にy移動するとき
                if(ydir == Direction::up){
                    exchange(ans, state, moved, Direction::down);       //ydirはtgtから見た時の方向なのでselectからみると逆
                }else{
                    exchange(ans, state, moved, Direction::up);
                }
            }

        }else if(*d2 > 0){      //d2方向への移動
            (*d2)--;    //あとで使うので移動した分減らしておく
            
            //tgtの移動
            if(first_x){    
                //最初にx移動だからこのときy移動
                move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], (int)tgt[1], tgt, scope); 
            }else{
                //最初にy移動だからこのときx移動
                move_target(ans, state, used, target, (int)tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt, scope); 
            }

            //selectとtgtの交換
            Index2D moved = tgt;    //selectの移動先
            tgt = ans.select();     //tgtの座標の変更
            if(first_x){    //最初にx移動するとき
                //このときy移動
                if(ydir == Direction::up){
                    exchange(ans, state, moved, Direction::down);       //ydirはtgtから見た時の方向なのでselectからみると逆
                }else{
                    exchange(ans, state, moved, Direction::up);
                }
            }else{          //最初にy移動するとき
                //このときx移動
                if(xdir == Direction::left){
                    exchange(ans, state, moved, Direction::right);      //xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
                }else{
                    exchange(ans, state, moved, Direction::left);
                }
            }

        }
        move_cnt++;     //count

    }

    //残りの座標を合わせる(d1方向からやることで一度も壁にぶつからずに持っていける)
    for(size_t i=0; i < *d1; i++){

        //tgtの移動
        if(first_x){
            move_target(ans, state, used, target, (int)tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt, scope); 
        }else{
            move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], (int)tgt[1], tgt, scope); 
        }

        //selectとtgtの交換
        Index2D moved = tgt;    //selectの移動先
        tgt = ans.select();     //tgtの座標の変更

        if(first_x){    //最初にx移動するとき
            if(xdir == Direction::left){
                exchange(ans, state, moved, Direction::right);      //xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
            }else{
                exchange(ans, state, moved, Direction::left);
            }
        }else{          //最初にy移動するとき
            if(ydir == Direction::up){
                exchange(ans, state, moved, Direction::down);       //ydirはtgtから見た時の方向なのでselectからみると逆
            }else{
                exchange(ans, state, moved, Direction::up);
            }
        }
        
    }

    //残りの座標を一致させる(d2方向)
    for(size_t i=0; i < *d2; i++){

        //tgtの移動
        if(first_x){    
            //最初にx移動だからこのときy移動
            move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], (int)tgt[1], tgt, scope); 
        }else{
            //最初にy移動だからこのときx移動
            move_target(ans, state, used, target, (int)tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt, scope); 
        }

        //selectとtgtの交換
        Index2D moved = tgt;    //selectの移動先
        tgt = ans.select();     //tgtの座標の変更
        if(first_x){    //最初にx移動するとき
            //このときy移動
            if(ydir == Direction::up){
                exchange(ans, state, moved, Direction::down);       //ydirはtgtから見た時の方向なのでselectからみると逆
            }else{
                exchange(ans, state, moved, Direction::up);
            }
        }else{          //最初にy移動するとき
            //このときx移動
            if(xdir == Direction::left){
                exchange(ans, state, moved, Direction::right);      //xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
            }else{
                exchange(ans, state, moved, Direction::left);
            }
        }
    }

}



struct DijkstraData
{
    DijkstraData()
    : answer(), state(), distance(0) {}

    DijkstraData(Answer ans, std::vector<std::vector<ImageID>> st, ptrdiff_t c)
    : answer(ans), state(st), distance(c) {}


    Answer answer;
    std::vector<std::vector<ImageID>> state;
    ptrdiff_t distance;


    bool operator==(DijkstraData const & rhs) const
    {
        return answer.select() == rhs.answer.select();
    }

    struct Index2DHash
    {
        size_t operator()(Index2D const & s) const
        {
            std::hash<size_t> h;
            return h(s[0]) + h(s[1]);
        }
    };
};

//断片の移動
//ans.select()をdstの位置へusedで移動禁止されている位置を通らずにもっていく
void move_piece(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>> const & used, std::vector<std::vector<ImageID>>& target, const Index2D& tgt, const Index2D& dst, const Rect& scope){

    using HashMap = std::unordered_map<Index2D, DijkstraData, DijkstraData::Index2DHash>;

    HashMap hashMap;
    hashMap[ans.select()] = DijkstraData(ans, state, 0);

    while(1){
        HashMap nextGen = hashMap;

        // nextGenに必要ならば追加する
        auto pushToNextGen = [&nextGen, &dst, &target, &used, &scope](DijkstraData const & e, ptrdiff_t i, ptrdiff_t j, Direction dir) -> bool
        {
            if(i < scope.top || j < scope.left || i > scope.bottom || j > scope.right)
                return false;

            if(used[i][j])
                return false;

            DijkstraData next = e;

            // 入れ替え前の、選択している位置bSelと、移動先の位置bNei
            auto bSel = next.answer.select();                                       // 移動前
            auto bNei = Index2D({static_cast<size_t>(i), static_cast<size_t>(j)});  // 移動先
            auto bNeiDST = search_piece(next.state[bNei[0]][bNei[1]], target);          // 移動先の断片が行きたい場所
            exchange(next.answer, next.state, bNei, dir);

            // 入れ替え後の、選択している位置aSelと、入れ替えされた他の断片の位置aNei
            auto aSel = bNei,       // 結局swap
                 aNei = bSel;       // ditto

            // 移動前の距離を引く
            next.distance -= std::abs(static_cast<ptrdiff_t>(dst[0] - bSel[0]))
                           + std::abs(static_cast<ptrdiff_t>(dst[1] - bSel[1]))
                           + std::abs(static_cast<ptrdiff_t>(bNeiDST[0] - bNei[0]))
                           + std::abs(static_cast<ptrdiff_t>(bNeiDST[1] - bNei[1]));

            // 移動後の距離を足す
            next.distance += std::abs(static_cast<ptrdiff_t>(dst[0] - aSel[0]))
                           + std::abs(static_cast<ptrdiff_t>(dst[1] - aSel[1]))
                           + std::abs(static_cast<ptrdiff_t>(bNeiDST[0] - aNei[0]))
                           + std::abs(static_cast<ptrdiff_t>(bNeiDST[1] - aNei[1]));

            // 入れ替え操作を一度行ったため+1
            next.distance += 1;

            // 既に探索されてるか確認する
            if (nextGen.find(aSel) == nextGen.end()){
                // 未だに無いので、新たに加える
                nextGen.emplace(aSel, std::move(next));
                return true;
            }else{
                auto& other = nextGen[aSel];

                // 見つかったので、必要なら更新する
                if(other.distance > next.distance){
                    other = std::move(next);
                    return true;
                }else
                    return false;
            }
        };

        size_t cnt = 0;
        for(auto e: hashMap){
            auto& data = e.second;
            Index2D sel = e.second.answer.select();

            if(pushToNextGen(data, static_cast<ptrdiff_t>(sel[0]+1), static_cast<ptrdiff_t>(sel[1]  ), Direction::down )) ++cnt;
            if(pushToNextGen(data, static_cast<ptrdiff_t>(sel[0]-1), static_cast<ptrdiff_t>(sel[1]  ), Direction::up   )) ++cnt;
            if(pushToNextGen(data, static_cast<ptrdiff_t>(sel[0]  ), static_cast<ptrdiff_t>(sel[1]+1), Direction::right)) ++cnt;
            if(pushToNextGen(data, static_cast<ptrdiff_t>(sel[0]  ), static_cast<ptrdiff_t>(sel[1]-1), Direction::left )) ++cnt;
        }

        hashMap = std::move(nextGen);
        if(cnt == 0)
            break;
    }

    auto it = hashMap.find(dst);
    PROCON_ENFORCE(it != hashMap.end(), "Cannot reach to dst.");

    ans = it->second.answer;
    state = it->second.state;
}

//selectと隣接断片の交換を行う(解答への交換操作追加処理含む)
void exchange(Answer& ans, std::vector<std::vector<ImageID>>& state, const Index2D moved, Direction dir){

    //遷移の表示
    /*
    puts("------------------------------------------------------------------------------");
    for(int i=0; i<state.size(); i++){
        for(int j=0; j<state[0].size(); j++){
            printf("[%lu %lu] ", state[i][j].get_index()[0], state[i][j].get_index()[1]);
        }
        puts("");
    }
    puts("");
    */

    //stateを変化
    Index2D sel = ans.select();     //一時的にselectを代入

    //状態の入れ替え
    ImageID temp = state[moved[0]][moved[1]];
    state[moved[0]][moved[1]] = state[sel[0]][sel[1]];
    state[sel[0]][sel[1]] = temp;

    //selectの座標をチェンジ
    ans.move_select(moved);

    //現在のselect座標取得
    Index2D now = ans.select();

    if(ans.is_meaningless(dir)){

        //解答の交換操作を１文字削除
        ans.remove();

    }else{

        //解答に交換操作を追加
        switch((size_t)dir){
            case (size_t)Direction::up :
                ans.exchange("U");
                break;
            case (size_t)Direction::right :
                ans.exchange("R");
                break;
            case (size_t)Direction::down :
                ans.exchange("D");
                break;
            case (size_t)Direction::left :
                ans.exchange("L");
                break;
        }
    }
}


//車庫入れできない状況になったときにそれを回避する
void special_move(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, const Rect& scope){
    //dist1にこれらの数値を足せば角の座標になる
    int corner_addy[4] = {1, 0, -1, 0};
    int corner_addx[4] = {0, 1, 0, -1};
    //dist1にこれらの数値を足せば最初にselectを持っていく座標になる
    int fselpos_addy[4] = {1, 1, -1, -1};
    int fselpos_addx[4] = {-1, 1, 1, -1};
    //special move時の移動方向
    Direction sp_move[4] = {
                         Direction::right,		//Right
                         Direction::up,			//UP
                         Direction::left,		//Left
                         Direction::down		//Down
                        };
    //select移動後座標を格納する変数
    Index2D moved;

    //selectを移動
    used[dist1[0]][dist1[1]] = true;
    used[(int)dist1[0] + (int)corner_addy[(size_t)dir]][(int)dist1[1] + (int)corner_addx[(size_t)dir]] = true;

    move_piece(ans, state, used, target, tgt2,  makeIndex2D((int)dist1[0] + (int)fselpos_addy[(size_t)dir], (int)dist1[1] + (int)fselpos_addx[(size_t)dir]), scope);        //右上隅の１つした

    used[dist1[0]][dist1[1]] = false;
    used[(int)dist1[0] + corner_addy[(size_t)dir]][(int)dist1[1] + (int)corner_addx[(size_t)dir]] = false;

    //special move
    //第1移動
    moved = makeIndex2D((int)dist1[0] + (int)corner_addy[(size_t)dir], (int)dist1[1] + (int)corner_addx[(size_t)dir]);
    tgt2 = ans.select();
    exchange(ans, state, moved, sp_move[(size_t)dir]);

	//第2移動以降
    div_tgts(ans, state, used, target, tgt1, tgt2, dist1, dist2, dir, scope);
}

//tgt2つを引き離す
void div_tgts(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, const Rect& scope){
    //dist1にこれらの数値を足せば角の座標になる
    int corner_addy[4] = {1, 0, -1, 0};
    int corner_addx[4] = {0, 1, 0, -1};
    //dist1にこれらの数値を足せばtgt2を離す座標になる
    int sep_addy[4] = {1, 2, -1, -2};
    int sep_addx[4] = {-2, 1, 2, -1};
    //special move時の移動方向
    Direction sp_move[4] = {
                         Direction::up,			//Right
                         Direction::left,		//UP
                         Direction::down,		//Left
                         Direction::right		//Down
                        };
    //select移動後座標を格納する変数
    Index2D moved;
    
    //第2移動
    moved = makeIndex2D(dist1[0], dist1[1]);
    tgt1 = ans.select();
    exchange(ans, state, moved, sp_move[(size_t)dir]);

    //tgt2を離す
    int sep_posy = (int)dist1[0] + sep_addy[(size_t)dir];
    int sep_posx = (int)dist1[1] + sep_addx[(size_t)dir];
    target_piece_clear(ans, state, used, target, tgt2, makeIndex2D(sep_posy, sep_posx), dir, scope);

    //tgt1を元の位置に戻す
    used[sep_posy][sep_posx] = true;
    target_piece_clear(ans, state, used, target, tgt1, dist1, dir, scope);
    used[sep_posy][sep_posx] = false;
}

//車庫入れ
//tgt1が先に入れる方(端にくる断片)
void parking_in_garage(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, ImageID tgt1id, ImageID tgt2id, const Rect& scope, bool prev_f){
    //問題のある状況のcheck時にみる方向
    int checky[4] = {1, 1, -1, -1};
    int checkx[4] = {-1, 1, 1, -1};


    //問題のある状況を除去(ここをコメントアウトしないほうがよくなる問題もある)
	//分割数が大きい画像のほうがここがないときの減少量が大きいが、一概に言えるものではなかった
	if(prev_f){
		for(size_t i=0; i < 2; i++){
			for(size_t j=0; j < 2; j++){
				if((int)dist1[0] + checky[(size_t)dir] * (int)i == tgt2[0] && (int)dist1[1] + checkx[(size_t)dir] * (int)j == tgt2[1]){                     //この4マスに入っている場合は、そこから断片2を追い出す
					int outy;           //問題があったときに追い出し先の座標を求めるときに使う
					int outx;

					switch((size_t)dir){
						case (size_t)Direction::up :
							outy = dist1[0] + 2;
							outx = dist1[1] + j;
							break;
						case (size_t)Direction::right :
							outy = dist1[0] + i;
							outx = dist1[1] - 2;
							break;
						case (size_t)Direction::down :
							outy = dist1[0] - 2;
							outx = dist1[1] - j;
							break;
						case (size_t)Direction::left :
							outy = dist1[0] - i;
							outx = dist1[1] + 2;
							break;
					}

					Index2D dist3 = makeIndex2D(outy, outx);            //追い出し先の座標
					target_piece_clear(ans, state, used, target, tgt2, dist3, Direction::up, scope);    //追い出す
						
				}
			}
		}

		//上の移動によりtgt1の座標が変わる場合があるのでもう一度tgt1の座標検索
		tgt1 = search_piece(tgt1id, state);
	}
    //tgt1を指定した位置に入れる
    target_piece_clear(ans, state, used, target, tgt1, dist1, dir, scope);

    //角の座標を得るための変数
    int cornery[4] = {1, 0, -1, 0};
    int cornerx[4] = {0, 1, 0, -1};
    //tgt1から斜めの位置の座標を得るための変数
    int diag_addy[4] = {1, 1, -1, -1};
    int diag_addx[4] = {-1, 1, 1, -1};
	//選択断片位置
	auto sel = ans.select();

    //tgt1の移動によりtgt2の座標が変わる場合があるのでもう一度tgt2の座標検索
    tgt2 = search_piece(tgt2id, state);
    if(tgt2[0] == (int)dist1[0] + (int)cornery[(size_t)dir] && tgt2[1] == (int)dist1[1] + (int)cornerx[(size_t)dir]){
        special_move(ans, state, used, target, tgt1, tgt2, dist1, dist2, dir, scope);
    }else if(tgt2[0] == (int)dist1[0] + (int)diag_addy[(size_t)dir] && tgt2[1] == (int)dist1[1] + (int)diag_addx[(size_t)dir] && sel[0] == (int)dist1[0] + (int)cornery[(size_t)dir] && sel[1] == (int)dist1[1] + (int)cornerx[(size_t)dir]){
        div_tgts(ans, state, used, target, tgt1, tgt2, dist1, dist2, dir, scope);
    }

    //もう一度tgt2の座標検索
    tgt2 = search_piece(tgt2id, state);
    //tgt2を指定した位置に入れる(このときtgt1を移動しないようにする)
    used[dist1[0]][dist1[1]] = true;
    target_piece_clear(ans, state, used, target, tgt2, dist2, dir, scope);
    used[dist1[0]][dist1[1]] = false;

    //tgt2の現在位置を取得
    tgt2 = search_piece(tgt2id, state);
    //指定位置まで選択断片を移動する
    used[dist2[0]][dist2[1]] = true;                                                //tgt2の位置を通れないようにする
    move_piece(ans, state, used, target, tgt2,  makeIndex2D((int)dist1[0] + (int)cornery[(size_t)dir], (int)dist1[1] + (int)cornerx[(size_t)dir]), scope);      //一番隅
    used[dist2[0]][dist2[1]] = false;                                               //usedをもとにもどしておく

    //車庫入れ交換移動の方向
    Direction pkdir[4][2] = {
                             {Direction::up, Direction::left},      //dir = Right
                             {Direction::left, Direction::down},    //dir = Up
                             {Direction::down, Direction::right},       //dir = Left
                             {Direction::right, Direction::up}      //dir = Down
                            };
    //車庫入れ交換移動
    Index2D moved = dist1;                          //selectはtgt1の位置に移動
    tgt1 = ans.select();                            //tgt1の座標の変更
    exchange(ans, state, moved, pkdir[(size_t)dir][0]); //selectの左移動

    moved = dist2;                                  //selectはtgt2の位置に移動
    tgt2 = ans.select();                            //tgt2の座標の変更
    exchange(ans, state, moved, pkdir[(size_t)dir][1]); //selectの下移動
        
}

//4つの端のラインのどれを次はつめたら良いかを評価する
Direction eval_edge(Answer& ans, std::vector<std::vector<ImageID>>& state, const std::vector<std::vector<ImageID>>& target, const Rect scope){

    Direction next; //戻り値

    //走査開始座標  (R, U, L, D)
    size_t start_h[4] = {scope.top, scope.top, scope.top, scope.bottom};
    size_t start_w[4] = {scope.right, scope.left, scope.left, scope.left};
    //走査終了座標
    size_t max_h[4] = {scope.bottom, scope.top, scope.bottom, scope.bottom};
    size_t max_w[4] = {scope.right, scope.right, scope.left, scope.right};
    //禁止方向
    bool forbid[4] = {false, false, false, false};  

    //サイズが2以下になっている方向を禁止方向に設定
    if(scope.bottom - scope.top <= 1){
        forbid[(size_t)Direction::up] = true;
        forbid[(size_t)Direction::down] = true;
    }else if(scope.right - scope.left <= 1){
        forbid[(size_t)Direction::left] = true;
        forbid[(size_t)Direction::right] = true;
    }

    //走査
    int min = std::numeric_limits<int>::max();      //最高評価値
    for(size_t k=0; k<4; k++){
        int eval = 0;       //評価値

        //評価
        for(size_t i = start_h[k]; i <= max_h[k]; i++){
            for(size_t j = start_w[k]; j <= max_w[k]; j++){
                Index2D tgt;    //target[i][j]のstateでの位置
                
                if(ans.select_id() == target[i][j]){    //ラインにselect断片が含まれているとき

                    switch (k){     //rightかdownでしかここには入らない
                        case (size_t)Direction::right:
                            //targetにおいてselectの左の断片をつめる
                            tgt = search_piece(target[i][j-1], state);      
                            break;
                        case (size_t)Direction::down:
                            //targetにおいてselectの上の断片をつめる
                            tgt = search_piece(target[i-1][j], state);      
                            break;
                    }

                    eval += std::abs((int)tgt[0] - (int)i) + std::abs((int)tgt[1] - (int)j);        //マンハッタン距離を評価値に加える
                    
                }else{

                    tgt = search_piece(target[i][j], state);        //tgtの位置をサーチ
                    eval += std::abs((int)tgt[0] - (int)i) + std::abs((int)tgt[1] - (int)j);        //マンハッタン距離を評価値に加える

                }
                
            }
        }

        //最小評価値になる方向を決める
        if(!forbid[k] && eval < min){
            min = eval;

            switch(k){
                case (size_t)Direction::right:  
                    next = Direction::right;
                    break;
                case (size_t)Direction::up:
                    next = Direction::up;
                    break;
                case (size_t)Direction::left:
                    next = Direction::left;
                    break;
                case (size_t)Direction::down:
                    next = Direction::down;
                    break;
            }
        }

        //評価値の表示
        //printf("eval:%d\n", eval);
    }
    //次のつめる方向の表示
    //printf("next:%d\n", (int)next);

    return next;
}

//１ライン完成させる
void first_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const Rect& scope, Direction dir){

    //走査開始座標  (R, U, L, D)
    size_t start_h[4] = {scope.top, scope.top, scope.bottom, scope.bottom};
    size_t start_w[4] = {scope.right, scope.left, scope.left, scope.right};
    //走査終了座標
    size_t max_h[4] = {scope.bottom - 1, scope.top, scope.top + 1, scope.bottom};
    size_t max_w[4] = {scope.right, scope.right - 1, scope.left, scope.left + 1};
    //走査終了座標
    int add_h[4] = {1, 1, -1, -1};
    int add_w[4] = {1, 1, -1, -1};
    //大小比較符号
    int comp_sign[4] = {1, 1, -1, -1};
    int sign = comp_sign[(size_t)dir];

    //つめた数
    size_t comp_num = 0;
    //つめる数
    size_t comp_size;


    //つめる数をセット
    switch((size_t)dir){
        case (size_t)Direction::up :
            comp_size = scope.right - scope.left;
            break;
        case (size_t)Direction::right :
            comp_size = scope.bottom - scope.top;
            break;
        case (size_t)Direction::down :
            comp_size = scope.right - scope.left;
            break;
        case (size_t)Direction::left :
            comp_size = scope.bottom - scope.top;
            break;
    }


    //つめていく
    for(int i = start_h[(size_t)dir]; (int)sign * i <= sign * (int)max_h[(size_t)dir]; i += add_h[(size_t)dir]){
        for(int j = start_w[(size_t)dir]; (int)sign * j <= sign * (int)max_w[(size_t)dir]; j += add_w[(size_t)dir]){
            Index2D dist = makeIndex2D(i, j);                               //目的座標
            Index2D tgt = search_piece(target[dist[0]][dist[1]], state);    //disの位置にくるべきpieceの位置を探索
            
            if(comp_num < comp_size - 1){   //愚直につめていく
                //ここから

                target_piece_clear(ans, state, used, target, tgt, dist, dir, scope);
                used[dist[0]][dist[1]] = true;                              //正しい位置に収められた断片は移動不可にする

            }else{              //車庫入れ

                //tgt2の目的座標への変換
                int dist2_tfy[4] = {0, 1, 0, -1};
                int dist2_tfx[4] = {-1, 0, 1, 0};
                //tgt2の断片画像への変換
                int tgt2_tfy[4] = {1, 0, -1, 0};
                int tgt2_tfx[4] = {0, 1, 0, -1};
                //dist2の増加分
                int dist2_addy;
                int dist2_addx;
                //tgt2の増加分
                int tgt2_addy;
                int tgt2_addx;

                //増分のセット
                dist2_addy = dist2_tfy[(size_t)dir];
                dist2_addx = dist2_tfx[(size_t)dir];
                tgt2_addy = tgt2_tfy[(size_t)dir];
                tgt2_addx = tgt2_tfx[(size_t)dir];

                //目的座標2
                Index2D dist2 = makeIndex2D(i + dist2_addy, j + dist2_addx);                        
                //目的座標1(dist)につめる断片画像の座標
                Index2D tgt2 = search_piece(target[i + tgt2_addy][j + tgt2_addx], state);           

                //車庫入れ本体
                if(tgt[0] != dist[0] || tgt[1] != dist[1] || tgt2[0] != (int)i + tgt2_addy || tgt2[1] != (int)j + tgt2_addx){
					//予防策があるのとないのどちらもやってみる
void parking_in_garage(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, ImageID tgt1id, ImageID tgt2id, const Rect& scope, bool prev_f);
					//各変数別個に用意しておく
					Answer ans1,ans2;
					ans1 = ans2 = ans;

					std::vector<std::vector<ImageID>> state1, state2;
					state1 = state2 = state;

					std::vector<std::vector<bool>> used1, used2;
					used1 = used2 = used;

					std::vector<std::vector<ImageID>> target1, target2;
					target1 = target2 = target;

                    parking_in_garage(ans1, state1, used1, target1, tgt2, tgt, dist, dist2, dir, target[i + tgt2_addy][j + tgt2_addx], target[i][j], scope, false);
                    parking_in_garage(ans2, state2, used2, target2, tgt2, tgt, dist, dist2, dir, target[i + tgt2_addy][j + tgt2_addx], target[i][j], scope, true);

					//良い結果な方をansとして残す
					auto v1 = ans1.ans();
					auto v2 = ans2.ans();
					auto s1 = v1[v1.size()-1];
					auto s2 = v2[v2.size()-1];
					if(s1.size() < s2.size()){
						ans = ans1;
						state = state1;
						used = used1;
						target = target1;
					}else{
						ans = ans2;
						state = state2;
						used = used2;
						target = target2;
					}
                }

                //[i, j]をusedにする
                used[i][j] = true;      
                //車庫入れ時に端につまるほうの断片をusedにする
                used[i + tgt2_addy][j + tgt2_addx] = true;  
                            
            }

            comp_num++; //つめたので数を1増やす
        }
    }

}

//右下部分の4マスを完成させる
void final_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const std::vector<std::vector<ImageID>>& true_target, const Rect& scope){

    //右下端の1つ上
    Index2D dist = makeIndex2D(scope.top, scope.right);                 //目的座標
    Index2D tgt = search_piece(target[dist[0]][dist[1]], state);        //distの位置にくるべきpieceの位置を探索

    target_piece_clear(ans, state, used, target, tgt, dist, Direction::up, scope);      //目的座標へ移動
    used[dist[0]][dist[1]] = true;                                      //正しい位置に収められた断片は移動不可にする
    
    //右下端
    move_piece(ans, state, used, target,  dist, makeIndex2D(scope.bottom, scope.right), scope); //いままで必死に働いたselectを正しい位置にもっていく
    used[scope.bottom][scope.right] = true;                                     //正しい位置に収められた断片を移動不可にする

    //stackにためておいた最後の動作を行う
    auto size = ans.final_move().size();
    for(int i=0; i < size; i++){
        Direction d = ans.final_move_pop();
        std::string s;

        switch((size_t)d){
            case (size_t)Direction::up :
                s = "U";
                break;
            case (size_t)Direction::right :
                s = "R";
                break;
            case (size_t)Direction::down :
                s = "D";
                break;
            case (size_t)Direction::left :
                s = "L";
                break;
        }
        ans.exchange(s);
    }

    //まだ答えになっていなかったら、選択を変える
    //右下端の1つ左
    if(!is_state_same(state, target)){

        dist = makeIndex2D(scope.bottom, scope.left);                       //目的座標
        tgt = search_piece(target[dist[0]][dist[1]], state);            //distの位置にくるべきpieceの位置を探索

        //選択の変更
        ans.set_select(tgt);    
        //selectを正しい位置にもっていく
        move_piece(ans, state, used, target,  makeIndex2D(scope.bottom, scope.right), makeIndex2D(scope.bottom, scope.left), scope);    
        //正しい位置に収められた断片は移動不可にする
        used[dist[0]][dist[1]] = true;      

    }

}

//状態が同じかどうかを返す
bool is_state_same(const std::vector<std::vector<ImageID>>& a, const std::vector<std::vector<ImageID>>& b){
    
    //サイズが異なっていたらfalse
    if(a.size() != b.size() || a[0].size() != b[0].size()) return false;

    //要素を1つずつ調べていく
    for(size_t i=0; i < a.size(); i++){
        for(int j=0; j < a[0].size(); j++){

            Index2D t1 = a[i][j].get_index();
            Index2D t2 = b[i][j].get_index();
            
            if(t1[0] != t2[0] || t1[1] != t2[1]) return false;      //1つでも違っていればfalse
            
        }
    }

    return true;    //1つも違っていなければtrue
}

//selectがつめるラインに会った場合ラインを完成できないのでtargetの変更
void transform_target(Answer& ans, std::vector<std::vector<ImageID>>& target, const Rect scope, Direction dir){
    //upやleftでは必要ない
    if(dir == Direction::up || dir == Direction::left) return;

    //走査開始座標  (R, U, L, D)
    size_t start_h[4] = {scope.top, scope.top, scope.top, scope.bottom};
    size_t start_w[4] = {scope.right, scope.left, scope.left, scope.left};
    //走査終了座標
    size_t max_h[4] = {scope.bottom, scope.top, scope.bottom, scope.bottom};
    size_t max_w[4] = {scope.right, scope.right, scope.left, scope.right};

    //走査
    for(size_t i = start_h[(size_t)dir]; i <= max_h[(size_t)dir]; i++){
        for(size_t j = start_w[(size_t)dir]; j <= max_w[(size_t)dir]; j++){
            
            //selectと同じ要素の発見
            if(target[i][j] == ans.select_id()){

                //targetの中身を入れ替える
                auto tmp = target[i][j];

                switch((size_t)dir){
                    case (size_t) Direction::right:
                        target[i][j] = target[i][j-1];
                        target[i][j-1] = tmp;

                        //変換したので元に戻すための最後のselect移動処理をスタックにいれておく
                        ans.final_move_push(Direction::right);
                        break;

                    case (size_t) Direction::down:
                        target[i][j] = target[i-1][j];
                        target[i-1][j] = tmp;

                        //変換したので元に戻すための最後のselect移動処理をスタックにいれておく
                        ans.final_move_push(Direction::down);
                        break;
                }
            }
        }
    }
}

//=============交換操作算出========================
void solver(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, std::vector<std::vector<ImageID>>& target, const std::vector<std::vector<ImageID>>& true_target, const size_t height, const size_t width){

    Rect scope(0, 0, width-1, height-1);

    while(scope.bottom - scope.top > 1 || scope.right - scope.left > 1){
        //つめる方向の決定
        Direction next = eval_edge(ans, state, target, scope);

        //必要であればtargetの変更
        transform_target(ans, target, scope, next);

        //つめる方向のラインを完成させる
        first_solve(ans, state, used, target, scope, next);

        //範囲の変更
        switch((size_t)next){
            case (size_t)Direction::up :
                scope.top++;
                break;
            case (size_t)Direction::right :
                scope.right--;
                break;
            case (size_t)Direction::down :
                scope.bottom--;
                break;
            case (size_t)Direction::left :
                scope.left++;
                break;
        }
        
    }

    //最後に残りの4マスを完成させる
    final_solve(ans, state, used, target, true_target, scope);

    
}

//ぜいたくに選択を使う
void lux_move(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>const & used, std::vector<std::vector<ImageID>>& target, int max_select_times, const int height, const int width){

	//selectを2だけ残してぜいたくに使う
	while(max_select_times > 2){

		max_select_times--;

		//マンハッタン距離が最大の断片を見つける
		int max_mhd = std::numeric_limits<int>::min();	//最大のマンハッタン距離
		Index2D sel;
		Index2D dst;
		for(size_t i=0; i < height; i++){
			for(size_t j=0; j < width; j++){
				if(i == 0 || i == height-1 || j == 0 || j == width-1){
						Index2D tgt;
						tgt = search_piece(target[i][j], state);        //tgtの位置をサーチ
						auto mhd = std::abs((int)tgt[0] - (int)i) + std::abs((int)tgt[1] - (int)j);        //マンハッタン距離
						if(mhd > max_mhd){
							max_mhd = mhd;	
							sel = tgt;
							dst = makeIndex2D(i, j);
						}
					
				}
			}
		}
		if(max_mhd == 0) break;		//もう一致しているなら動かす必要はない
		ans.set_selectID(makeImageID(dst[0], dst[1]));	//選択断片IDのセット
		ans.set_select(sel);							//選択
		Rect scope(0, 0, width-1, height-1);			//一応の範囲指定
		move_piece(ans, state, used, target, sel, dst, scope);    //目的位置に直接持っていく
	}
}

//かなめの関数
std::vector<std::string> line_greedy_calc_exchange(std::vector<std::vector<ImageID>> target, int select_costrate, int exchange_costrate, size_t max_select_times){

    Answer ans;                             //解答生成クラス
    size_t width = target[0].size();        //状態の幅
    size_t height = target.size();          //状態の高さ
    std::vector<std::vector<bool>> used(height, std::vector<bool>(width)) ; //使用不可フラグ

    //使用可能フラグの初期化
    for(size_t i=0; i < height; i++){
        for(size_t j=0; j < width; j++){
            used[i][j] = false;
        }
    }

    //最終状態の表示
	/*
    puts("------------------------------------------------------------------------------");
    for(int i=0; i<target.size(); i++){
        for(int j=0; j<target[0].size(); j++){
            printf("[%lu %lu] ", target[i][j].get_index()[0], target[i][j].get_index()[1]);
        }
        puts("");
    }
    puts("");
	*/
    //スタートの状態の生成
    std::vector<std::vector<ImageID>> state = make_start_state(height, width);

    //=============交換操作算出========================
    std::vector<std::vector<ImageID>> fix_target = target;                      //修正されるtarget

	//選択コスト重視と、交換コスト重視で分けるために各々２つの変数を用意
	auto ans1 = ans;
	auto ans2 = ans;
	auto state1 = state;
	auto state2 = state;
	auto used1 = used;
	auto used2 = used;
	auto fix_target1 = fix_target;
	auto fix_target2 = fix_target;

	auto mean_move = std::max(width, height) - 1;	//lux_moveで一回の選択で平均動く量の経験則上の値
	//lux_moveをするかしないかのフラグ(小さい問題であれば２つとも実行しても時間がかからないので実行する)
	bool is_lux = select_costrate + mean_move * exchange_costrate < mean_move * 5 * exchange_costrate;	//5は１つの断片を移動させるときに迂回して動かすときにかかる交換回数
	bool is_small = width * height < 81;				//断片数が81より小さい時smallだと判断

	//ぜいたくに選択を使う
	if(is_small || is_lux)lux_move(ans2, state2, used2, fix_target2, max_select_times, height, width);

    //targetでの右下にある断片の選択
    ImageID first = target[height - 1][width - 1];
    if(is_small || !is_lux) ans1.set_selectID(first);                        //選択断片IDのセット
    if(is_small || is_lux)  ans2.set_selectID(first);                        //選択断片IDのセット

    Index2D temp = first.get_index();
    if(is_small || !is_lux) ans1.set_select(search_piece(first, state1));     //最初の選択
    if(is_small || is_lux) ans2.set_select(search_piece(first, state2));     //最初の選択

	//解を求める
    if(is_small || !is_lux) solver(ans1, state1, used1, fix_target1, target, height, width);
    if(is_small || is_lux) solver(ans2, state2, used2, fix_target2, target, height, width);

    //最終的な解答へ変換する
    if(is_small || !is_lux) ans1.finish();   
	if(is_small || is_lux) ans2.finish();

	//解答の比較
	if(is_small){
		auto total_cost1 = ans1.exchange_num() * exchange_costrate + ans1.select_num() * select_costrate;
		auto total_cost2 = ans2.exchange_num() * exchange_costrate + ans2.select_num() * select_costrate;

		printf("totalcost1 = %d\n", total_cost1);
		printf("totalcost2 = %d\n", total_cost2);

		//解答を返す
		if(total_cost1 < total_cost2){
			return ans1.ans(); 
		}else{
			return ans2.ans();
		}
	}else if(is_lux){
		auto total_cost2 = ans2.exchange_num() * exchange_costrate + ans2.select_num() * select_costrate;
		printf("totalcost2 = %d\n", total_cost2);
		return ans2.ans();
	}else{
		auto total_cost1 = ans1.exchange_num() * exchange_costrate + ans1.select_num() * select_costrate;
		printf("totalcost1 = %d\n", total_cost1);
		return ans1.ans(); 
	}

}

}}


