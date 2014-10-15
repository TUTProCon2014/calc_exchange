#pragma once

#include "../../inout/include/inout.hpp"
#include "../../utils/include/template.hpp"
#include "../../utils/include/types.hpp"
#include "../../utils/include/range.hpp"

#include <vector>
#include <queue>
#include <string>
#include <map>
#include <cmath>
#include <cstdio>
#include <set>

namespace procon { namespace calc_exchange{

using namespace utils;

//prototype宣言
ImageID makeImageID(size_t i, size_t j);

class Node{
private:
    std::vector<std::vector<ImageID>> _state;   //インデックスの状態
    std::string _op;    //このノードへ来たときに行った操作
    Index2D _select;    //現在選択中のインデックス
    int _g; //スタートからこのノードまでのコスト
    int _select_num;    //選択数
    int _exchange_num;  //交換数
	unsigned long long int _state_num;	//状態の番号
    double _h;  //このノードからゴールまでの推定コスト
    std::shared_ptr<Node> _parent;  //親ノードのポインタ
    static size_t _max_select_times; //最大選択可能数
    static int _select_costrate;    //選択コストレート
    static int _exchange_costrate;  //交換コストレート
	//階乗の値のリスト
	const static unsigned long long int fact[16];

public:

    Node(std::vector<std::vector<ImageID>> state, std::string op, Index2D select, int g, int select_num, int exchange_num) : _state(state), _op(op), _select(select), _g(g), _select_num(select_num), _exchange_num(exchange_num){
		//state_numの計算
		_state_num = calc_state_num(_state);
		//std::cout << _state_num << std::endl;
    }

    //最大選択可能数をセットする
    void set_max_select_times(size_t max_select){
        _max_select_times = max_select;
    }

    //選択コストレートをセットする
    void set_select_costrate(size_t x){
        _select_costrate = x;
    }

    //交換コストレートをセットする
    void set_exchange_costrate(size_t x){
        _exchange_costrate = x;
    }

    //操作をセットする
    void set_op(std::string op){
        _op = op;
    }

    //親ノードをセットする
    void set_parent(std::shared_ptr<Node> parent){
        _parent = parent;
    }

	//state_numの作成
	unsigned long long int calc_state_num(std::vector<std::vector<ImageID>> state){
		unsigned long long int state_num = 0;	//状態番号
		std::vector<int> perm;					//順列

		//順列の生成
		for(size_t i=0; i < _state.size(); i++){
			for(size_t j=0; j < _state[0].size(); j++){
				Index2D idx = state[i][j].get_index();
				perm.push_back(idx[0] * i + idx[1]);
			}
		}

		//state_numの作成
		for(int i=perm.size()-1; i >=0; i--){
			state_num += perm[i] * fact[i];
			//計算した番号より大きい番号のものを１小さくする
			for(int j = i-1; j >= 0; j--){
				if(perm[j] > perm[i]) perm[j]--;
			}
		}

		return state_num;
	}

    //状態pを整列した番号にかえる対応付けの生成
    std::map<ImageID, ImageID> renumbering(std::shared_ptr<Node> p){
        std::map<ImageID, ImageID> targetzero;  //ターゲットの状態を整列された番号にする
        for(size_t i=0; i < _state.size(); i++){
            for(size_t j=0; j < _state[0].size(); j++){
                targetzero[p->state()[i][j]] = makeImageID(i, j);
            }
        }

        return targetzero;
    }

    //pまでの推定コストhを計算
    double calc_cost_h(std::map<ImageID, ImageID> targetzero){
        
        //現状態を番号付けし直し、コストを計算
        auto changed = _state;
        double cost = 0;
        for(size_t i=0; i < _state.size(); i++){
            for(size_t j=0; j < _state[0].size(); j++){
                changed[i][j] = targetzero[changed[i][j]];
                auto temp = changed[i][j].get_index();
                cost += std::abs(static_cast<std::ptrdiff_t>(temp[0] - i));
                cost += std::abs(static_cast<std::ptrdiff_t>(temp[1] - j));
            }
        }

        //マンハッタン距離の合計値の表示用
        //std::cout << cost << std::endl;

        //_h = cost * 2.0 * exchange_costrate() + 2.5*cost* select_num() / max_select_times();
        //_h = 2.0 * cost * exchange_costrate();
        _h = 0.8*cost;
        //_h = cost * 0.9 * exchange_costrate();
        //_h = cost * 35.5 * exchange_costrate();
        return _h;
    }

    //合計コストg+hを返す
    double get_totalcost(){
        //return  select_num() * select_costrate() + _h;
        //return exchange_num() + select_num() * select_costrate() + _h;
        //return select_num() * select_costrate() + exchange_num() * exchange_costrate() + _h;
        //return select_num() * select_costrate() + exchange_num() * exchange_costrate() + _h;
        return select_num() + exchange_num() + _h;
    }

    //次の状態ノードリストの生成
    //(マンハッタン距離が一番大きい断片を選択する)
    /*
    std::vector<std::shared_ptr<Node>> get_next_state(std::shared_ptr<Node> goal){
        int dx[] = {1, 0, -1, 0, 0};
        int dy[] = {0, -1, 0, 1, 0};
        std::string operation[] = {"R", "U", "L", "D", ""};
        std::vector<std::shared_ptr<Node>> next;
        for(int i=0; i<5; i++){
            int nx = _select[1] + dx[i];
            int ny = _select[0] + dy[i];

            //範囲外でなければ状態を変更してリストに追加
            if(nx < _state[0].size() && nx >= 0 && ny < _state.size() && ny >= 0){
                std::vector<std::vector<ImageID>> nstate = _state;  //変更後の状態
                if(i < 4){ //方向移動なら移動

                    ImageID temp = nstate[_select[0]][_select[1]];
                    nstate[_select[0]][_select[1]] = nstate[ny][nx];
                    nstate[ny][nx] = temp;
                }else if(select_num() < max_select_times()){ //選択断片のチェンジ
                    std::vector<std::vector<ImageID>> nstate = _state;  //変更後の状態

                    //ゴール状態を番号付けし直す
                    std::map<ImageID, ImageID> targetzero = renumbering(goal);

                    //マンハッタン距離が一番大きいものを見つける
                    auto changed = nstate;
                    size_t max = std::numeric_limits<size_t>::min();
                    for(size_t i=0; i < _state.size(); i++){
                        for(size_t j=0; j < _state[0].size(); j++){
                            //状態を番号付けし直し、コストを計算

                            size_t cost = 0;
                            changed[i][j] = targetzero[changed[i][j]];
                            auto temp = changed[i][j].get_index();
                            cost += std::abs(static_cast<std::ptrdiff_t>(temp[0] - i));
                            cost += std::abs(static_cast<std::ptrdiff_t>(temp[1] - j));

                            if(cost > max){ 
                                max = cost;
                                ny = i;
                                nx = j;
                            }
                        }
                    }
                    //operationに選択座標を代入
                    std::stringstream ss;
                    ss << ny << nx;
                    operation[4] = ss.str();
                }

                if(i == 4){
                    if(select_num() < max_select_times()){
                        std::shared_ptr<Node> temp(new Node(nstate, operation[i], makeIndex2D(ny, nx), _g+1, _select_num+1, _exchange_num));
                        next.push_back(temp);
                    }
                }else{
                    std::shared_ptr<Node> temp(new Node(nstate, operation[i], makeIndex2D(ny, nx), _g+1, _select_num, _exchange_num+1));
                    next.push_back(temp);
                }
            }
        }

        
        _next_state_list = next;
        return next;
    }
    */

    //全選択を生成
    std::vector<std::shared_ptr<Node>> get_next_state(std::shared_ptr<Node> goal){
        int dx[] = {1, 0, -1, 0};
        int dy[] = {0, -1, 0, 1};
        std::string operation[] = {"R", "U", "L", "D"};
        std::vector<std::shared_ptr<Node>> next;

        //交換ノードの生成
        for(int i=0; i<4; i++){
            int nx = _select[1] + dx[i];
            int ny = _select[0] + dy[i];

            //範囲外でなければ状態を変更してリストに追加
            if(nx < _state[0].size() && nx >= 0 && ny < _state.size() && ny >= 0){
                std::vector<std::vector<ImageID>> nstate = _state;  //変更後の状態


                ImageID t = nstate[_select[0]][_select[1]];
                nstate[_select[0]][_select[1]] = nstate[ny][nx];
                nstate[ny][nx] = t;


                std::shared_ptr<Node> tmp(new Node(nstate, operation[i], makeIndex2D(ny, nx), _g+1, _select_num, _exchange_num+1));
                next.push_back(tmp);
            }
        }

        //選択ノードの生成
        for(size_t i=0; i < _state.size(); i++){
            for(size_t j=0; j < _state[0].size(); j++){
                std::vector<std::vector<ImageID>> nstate = _state;  //変更後の状態
                
                //operationに選択座標を代入
                std::stringstream selop;
                selop << j << i;

                //次状態にセット
                if(select_num() < max_select_times()){
                    std::shared_ptr<Node> tmp(new Node(nstate, selop.str(), makeIndex2D(i, j), _g+1, _select_num+1, _exchange_num));
                    next.push_back(tmp);
                }
            }
        }
        
        return next;
    }

    //start_ptrのみが呼び出す
    void init(){
        _op = "";
        _select = makeIndex2D(0, 0);
        _select_num = 0;
    }


    //大小比較 g+hによる比較
    bool operator< (Node &node){
        return this->get_totalcost() < node.get_totalcost();
    }

    bool operator> (Node &node){
        return this->get_totalcost() > node.get_totalcost();
    }

    //_max_select_timesのgetter
    size_t max_select_times() const{
        return _max_select_times;
    }

    //_select_costrateのgetter
    int select_costrate() const{
        return _select_costrate;
    }

    //_exchange_costrateのgetter
    int exchange_costrate() const{
        return _exchange_costrate;
    }

    //_stateのgetter
    std::vector<std::vector<ImageID>> state() const{
        return _state;
    }

    //_opのgeter
    std::string op() const{
        return _op;
    }

    //_parentのgetter
    std::shared_ptr<Node> parent() const{
        return _parent;
    }

    //_selectのgetter
    Index2D select() const{
        return _select;
    }

    //_select_numのgetter
    int select_num() const{
        return _select_num;
    }

    //_exchange_numのgetter
    int exchange_num() const{
        return _exchange_num;
    }

	//_state_numのgetter
	unsigned long long int state_num() const{
		return _state_num;
	}
};

//Nodeの静的メンバ変数の実体
size_t Node::_max_select_times;
int Node::_select_costrate;
int Node::_exchange_costrate;
const unsigned long long int Node::fact[16] = {
    1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800,
    39916800, 479001600, 6227020800, 87178291200, 1307674368000
};

//状態pを整列した番号にかえる対応付けの生成
std::map<ImageID, ImageID> renumbering(std::shared_ptr<Node> p){
    std::map<ImageID, ImageID> targetzero;  //ターゲットの状態を整列された番号にする
    for(size_t i=0; i < p->state().size(); i++){
        for(size_t j=0; j < p->state()[0].size(); j++){
            targetzero[p->state()[i][j]] = makeImageID(i, j);
        }
    }

    return targetzero;
}

//リストから最も評価の高いノードを選ぶ
std::shared_ptr<Node> get_best_node(std::vector<std::shared_ptr<Node>> openlist){
    double min = std::numeric_limits<double>::max();
    std::shared_ptr<Node> ret;  

    for(auto p : openlist){
        auto t = p->get_totalcost();
        if(t < min){
            min = t;
            ret = p;
        }
    }

    return ret;
}

//リストの中にpが含まれているかどうか
size_t num_in_list(Node x, std::vector<std::shared_ptr<Node>> l){
    for(size_t i=0; i<l.size(); i++){
        if(l[i]->state() == x.state() && l[i]->select() == x.select())
            return i;
    }
    return -1;
}

//解答のフォーマットで操作列を作成
std::vector<std::string> op_format(std::shared_ptr<Node> start_ptr, std::shared_ptr<Node> goal_ptr){
    std::vector<std::string> ret;

    std::string ops = "";

    std::shared_ptr<Node> trace = goal_ptr;
    while(trace != nullptr){
        if(trace->op() == ""){
            break;
        }else if(trace->op() == "R" || trace->op() == "U" || trace->op() == "L" || trace->op() == "D"){ //交換操作のとき
            ops += trace->op();
        }else{ //選択操作のとき
            if(ops.size() > 0){ //1文字以上なら交換操作をpush_back
                std::reverse(ops.begin(), ops.end());
                std::stringstream ss;
                ss << ops.size();

                ret.push_back(ops); //交換操作
                ret.push_back(ss.str()); //交換回数
                ops = "";
            }

            //選択画像位置
            std::stringstream ss;
            ss << trace->op();
            ret.push_back(ss.str());

        }
        trace = trace->parent();
    }
    if(ops.size() > 0){ //1文字以上なら交換操作をpush_back
        std::reverse(ops.begin(), ops.end());
        std::stringstream ss;
        ss << ops.size();

        ret.push_back(ops); //交換操作
        ret.push_back(ss.str()); //交換回数
    }

    std::stringstream ss;
    ss << goal_ptr->select_num();
    ret.push_back(ss.str());

    std::reverse(ret.begin(), ret.end());

    return ret;
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

//不一致個数を返す
int num_state_diff(const std::vector<std::vector<ImageID>> a, const std::vector<std::vector<ImageID>> b){
    int cnt = 0;    //不一致個数
    
    //サイズが異なっていたら 10000
    if(a.size() != b.size() || a[0].size() != b[0].size()) return 10000;

    //要素を1つずつ調べていく
    for(size_t i=0; i < a.size(); i++){
        for(int j=0; j < a[0].size(); j++){

            Index2D t1 = a[i][j].get_index();
            Index2D t2 = b[i][j].get_index();
            
            if(t1[0] != t2[0] || t1[1] != t2[1]) cnt++;            
        }
    }

    return cnt; 
}

//ImageIDの生成
ImageID makeImageID(size_t i, size_t j){
    ImageID t(i, j);
    return t;
}

//openlistとclosedlistで用いるstd::setの比較用構造体
struct LessNode{
    bool operator()(std::shared_ptr<Node> const & ln, std::shared_ptr<Node> const & rn) const {
        if (ln->get_totalcost() < rn->get_totalcost())
            return true;
        else if (ln->get_totalcost() > rn->get_totalcost())
            return false;
 
        //状態の配列すべての要素について比較
        if(ln->state_num() < rn->state_num()){
            return true;
        }else if(ln->state_num() > rn->state_num()){
            return false;
        }

        //selectについての比較
        if(opCmp(ln->select(), rn->select()) < 0){
            return true;
        }else if(ln->select() > rn->select() > 0){
            return false;
        }

        //一致したとき
        return false;
    }
};

//リストの中にpが含まれているかどうか
std::multiset<std::shared_ptr<Node>>::iterator num_in_list(std::shared_ptr<Node> x, std::multiset<std::shared_ptr<Node>, LessNode> const & l){
    for(auto it = l.begin(); it != l.end(); it++){
        if((*it)->state() == x->state() && (*it)->select() == x->select())
            return it;
    }
    return l.end();
}

std::vector<std::string>  calc_exchange(std::vector<std::vector<ImageID>> const & target, int select_costrate, int exchange_costrate, size_t max_select_times){

	//断片数１６以上の画像がきたら終わらせる
	if(target.size() * target[0].size() > 16){
		PROCON_ENFORCE(0, "でかい");
	}

    const size_t max_openlist_size = 500000;    //openlistの最大数

    //スタートの状態の生成
    std::vector<std::vector<ImageID>> ss(target.size(), std::vector<ImageID>(target[0].size()));
    for(size_t i=0; i < target.size(); i++){
        for(size_t j=0; j < target[0].size(); j++){
            ss[i][j] = makeImageID(i, j);
        }
    }

    std::shared_ptr<Node> goal_ptr(new Node(target, "", makeIndex2D(0, 0), std::numeric_limits<int>::max(), 0, 0)); //ゴール状態のポインタ

    std::shared_ptr<Node> start_ptr(new Node(ss, "no", makeIndex2D(0, 0), 0, 0, 0));    //スタート状態のポインタ
    std::shared_ptr<Node> start_ptr2(new Node(ss, "no", makeIndex2D(0, 0), 0, 0, 0));   //スタート状態のポインタ

    std::map<ImageID, ImageID> targetzero = renumbering(goal_ptr);

    start_ptr->init(); //初期選択断片を決める
    start_ptr->set_max_select_times(max_select_times); //最大選択可能数のset
    start_ptr->set_select_costrate(select_costrate); //選択コストレートのset
    start_ptr->set_exchange_costrate(exchange_costrate); //交換コストレートのset

    std::set<std::shared_ptr<Node>, LessNode> openlist;
    std::set<std::shared_ptr<Node>, LessNode> closedlist;

    //スタートノードのコスト計算と親ノードの設定
    start_ptr->calc_cost_h(targetzero);
    start_ptr2->calc_cost_h(targetzero);
    start_ptr->set_parent(nullptr);

    //スタートノードの後続ノードをopenlistに入れる
    for(auto st : start_ptr->get_next_state(goal_ptr)){
        if(is_state_same(st->state(), start_ptr->state()) ){
            st->calc_cost_h(targetzero);
            st->set_parent(start_ptr);

            //各選択からの交換操作がちゃんとopenlistにはじめに入るようにする
            //こうすることで、探索される選択の偏りをなくす
            for(auto st2 : st->get_next_state(goal_ptr)){
                st2->calc_cost_h(targetzero);
                st2->set_parent(st);
                openlist.insert(st2);
            }
        }
        closedlist.insert(st);
    }

    openlist.insert(start_ptr);

    while(openlist.size() > 0){
        //メモリを食い過ぎる前に終わらせる
        //std::cout << "size:" << openlist.size() << std::endl;
        if(openlist.size() > max_openlist_size){
            std::cout << "(´・ω・｀)" << std::endl;
            break;
        }

        //最も評価の高いノードを選ぶ
        std::shared_ptr<Node> p = *(openlist.begin());

        /*
        //コストと現在の選択回数の表示用
        //std::cout << p->get_totalcost() << " " << p->select_num() << std::endl;

        //openlistとclosedlistの表示用
        std::cout << "open:";
        for(auto a : openlist){
            std::cout << a->get_totalcost() << " ";
        }
        puts("");
        std::cout << "closed:";
        for(auto a : closedlist){
            std::cout << a->get_totalcost() << " ";
        }
        puts("");
        */
        
        if(is_state_same(p->state(), goal_ptr->state())){
            goal_ptr = p;
            break;
        }

        for(auto st : p->get_next_state(goal_ptr)){
            if(st->state() == p->state() && st->select() == p->select()) continue;

            st->calc_cost_h(targetzero);
            auto oi = openlist.find(st);
            auto ci = closedlist.find(st);

            if(oi != openlist.end()){
                //オープンリストに入っているとき
                if((*oi)->get_totalcost() > st->get_totalcost()){
                    st->set_parent(p);
                    openlist.erase(oi);
                    openlist.insert(st);
                }

            }else if(ci != closedlist.end()){
                if((*ci)->get_totalcost() > st->get_totalcost()){
                    st->set_parent(p);
                    closedlist.erase(ci);
                    openlist.insert(st);
                }
            }else{
                //まだリストに入っていないのでオープンリストに追加
                st->set_parent(p);
                st->calc_cost_h(targetzero);
                openlist.insert(st);
            }
        }
        closedlist.insert(p);
        openlist.erase(p);
    }

    //結果の出力
    std::cout << "select_cost:" << goal_ptr->select_num() * select_costrate << std::endl;
    std::cout << "exchange_cost:" << goal_ptr->exchange_num() * exchange_costrate << std::endl;
    std::cout << "total_cost:" << goal_ptr->select_num() * select_costrate + goal_ptr->exchange_num() * exchange_costrate << std::endl;

    //解答の生成
    std::vector<std::string> ans;
    ans = op_format(start_ptr, goal_ptr);
    return ans;

}

}}
