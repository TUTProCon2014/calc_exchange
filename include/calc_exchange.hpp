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

namespace procon { namespace calc_exchange{

using namespace utils;

class Node{
private:
	std::vector<std::vector<Index2D>> _state;	//インデックスの状態
	std::string _op;	//このノードへ来たときに行った操作
	Index2D _select;	//現在選択中のインデックス
	int _g;	//スタートからこのノードまでのコスト
	int _select_num;	//選択数
	int _exchange_num;	//交換数
	double _h;	//このノードからゴールまでの推定コスト
	std::vector<std::shared_ptr<Node>> _next_state_list; //そのノードからたどれるノードのリスト
	std::shared_ptr<Node> _parent;	//親ノードのポインタ
	static size_t _max_select_times; //最大選択可能数
	static int _select_costrate;	//選択コストレート
	static int _exchange_costrate;	//交換コストレート
public:

	Node(std::vector<std::vector<Index2D>> state, std::string op, Index2D select, int g, int select_num, int exchange_num) : _state(state), _op(op), _select(select), _g(g), _select_num(select_num), _exchange_num(exchange_num){
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

	//状態pを整列した番号にかえる対応付けの生成
	std::map<Index2D, Index2D> renumbering(std::shared_ptr<Node> p){
		std::map<Index2D, Index2D> targetzero;	//ターゲットの状態を整列された番号にする
		for(size_t i=0; i < _state.size(); i++){
			for(size_t j=0; j < _state[0].size(); j++){
				targetzero[p->state()[i][j]] = makeIndex2D(i, j);
			}
		}

		return targetzero;
	}

	//pまでの推定コストhを計算
	double calc_cost_h(std::shared_ptr<Node> p){
		//ゴール状態を番号付けし直す
		std::map<Index2D, Index2D> targetzero = renumbering(p);
		
		//現状態を番号付けし直し、コストを計算
		auto changed = _state;
		double cost = 0;
		for(size_t i=0; i < _state.size(); i++){
			for(size_t j=0; j < _state[0].size(); j++){
				changed[i][j] = targetzero[changed[i][j]];
				cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][0] - i));
				cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][1] - j));
			}
		}

		//マンハッタン距離の合計値の表示用
		//std::cout << cost << std::endl;

		_h = cost * 2.0 * exchange_costrate() + 2.5*cost* select_num() / max_select_times();
		//_h = cost;
		return _h;
	}

	//合計コストg+hを返す
	double get_totalcost(){
		//return  select_num() * select_costrate() + _h;
		//return exchange_num() + select_num() * select_costrate() + _h;
		return  select_num() * select_costrate() + exchange_num() * exchange_costrate() + _h;
	}

	//次の状態ノードリストの生成
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
				std::vector<std::vector<Index2D>> nstate = _state;	//変更後の状態
				if(i < 4){ //方向移動なら移動

					Index2D temp = nstate[_select[0]][_select[1]];
					nstate[_select[0]][_select[1]] = nstate[ny][nx];
					nstate[ny][nx] = temp;
				}else if(select_num() < max_select_times()){ //選択断片のチェンジ
					std::vector<std::vector<Index2D>> nstate = _state;	//変更後の状態

					//ゴール状態を番号付けし直す
					std::map<Index2D, Index2D> targetzero = renumbering(goal);

					//マンハッタン距離が一番大きいものを見つける
					auto changed = nstate;
					size_t max = std::numeric_limits<size_t>::min();
					for(size_t i=0; i < _state.size(); i++){
						for(size_t j=0; j < _state[0].size(); j++){
							//状態を番号付けし直し、コストを計算

							size_t cost = 0;
							changed[i][j] = targetzero[changed[i][j]];
							cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][0] - i));
							cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][1] - j));

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

	//start_ptrのみが呼び出す
	void init(std::shared_ptr<Node> goal_ptr){

		//ゴール状態を番号付けし直す
		std::map<Index2D, Index2D> targetzero = renumbering(goal_ptr);

		//マンハッタン距離が一番大きいものを見つける
		auto changed = _state;
		size_t max = std::numeric_limits<size_t>::min();
		int ny = -1;
		int nx = -1;
		for(size_t i=0; i < _state.size(); i++){
			for(size_t j=0; j < _state[0].size(); j++){
				//状態を番号付けし直し、コストを計算

				size_t cost = 0;
				changed[i][j] = targetzero[changed[i][j]];
				cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][0] - i));
				cost += std::abs(static_cast<std::ptrdiff_t>(changed[i][j][1] - j));

				if(cost > max){ 
					max = cost;
					ny = i;
					nx = j;
				}
			}
		}
		std::stringstream op;
		op << ny << nx;
		_op = op.str();
		_select = makeIndex2D(ny, nx);
		_select_num = 1;
	}


	//大小比較 g+hによる比較
	bool operator< (Node &node){
		return this->get_totalcost() < node.get_totalcost();
	}

	bool operator> (Node &node){
		return this->get_totalcost() > node.get_totalcost();
	}

	//_max_select_timesのgetter
	size_t max_select_times(){
		return _max_select_times;
	}

	//_select_costrateのgetter
	int select_costrate(){
		return _select_costrate;
	}

	//_exchange_costrateのgetter
	int exchange_costrate(){
		return _exchange_costrate;
	}

	//_stateのgetter
	std::vector<std::vector<Index2D>> state(){
		return _state;
	}

	//_opのgeter
	std::string op(){
		return _op;
	}

	//_parentのgetter
	std::shared_ptr<Node> parent(){
		return _parent;
	}

	//_selectのgetter
	Index2D select(){
		return _select;
	}

	//_select_numのgetter
	int select_num(){
		return _select_num;
	}

	//_exchange_numのgetter
	int exchange_num(){
		return _exchange_num;
	}
};

//Nodeの静的メンバ変数の実体
size_t Node::_max_select_times;
int Node::_select_costrate;
int Node::_exchange_costrate; 

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
		if(trace->op() == "R" || trace->op() == "U" || trace->op() == "L" || trace->op() == "D"){ //交換操作のとき
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

std::vector<std::string>  calc_exchange(std::vector<std::vector<Index2D>> const & target, int select_costrate, int exchange_costrate, size_t max_select_times){
	std::vector<Node*> exop;	//交換手順

	//スタートの状態の生成
	std::vector<std::vector<Index2D>> ss(target.size(), std::vector<Index2D>(target[0].size()));
	for(size_t i=0; i < target.size(); i++){
		for(size_t j=0; j < target[0].size(); j++){
			ss[i][j] = makeIndex2D(i, j);
		}
	}

	std::shared_ptr<Node> goal_ptr(new Node(target, "", makeIndex2D(0, 0), std::numeric_limits<int>::max(), 0, 0)); //ゴール状態のポインタ

	std::shared_ptr<Node> start_ptr(new Node(ss, "no", makeIndex2D(0, 0), 0, 0, 0));	//スタート状態のポインタ
	start_ptr->init(goal_ptr); //初期選択断片を決める
	//start_ptr->set_max_select_times(max_select_times); //最大選択可能数のset
	start_ptr->set_max_select_times(max_select_times); //最大選択可能数のset
	start_ptr->set_select_costrate(select_costrate); //選択コストレートのset
	start_ptr->set_exchange_costrate(exchange_costrate); //交換コストレートのset

	std::vector<std::shared_ptr<Node>> openlist;
	std::vector<std::shared_ptr<Node>> closedlist;

	//スタートノードのコスト計算と親ノードの設定
	start_ptr->calc_cost_h(goal_ptr);
	start_ptr->set_parent(nullptr);
	openlist.push_back(start_ptr);

	while(openlist.size() > 0){
		//最も評価の高いノードを選ぶ
		std::shared_ptr<Node> p = get_best_node(openlist);

		//コストと現在の選択回数の表示用
		//std::cout << p->get_totalcost() << " " << p->select_num() << std::endl;

		//openlistとclosedlistの表示用
		/*
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
		
		if(p->state() == goal_ptr->state()){
			goal_ptr = p;
			break;
		}

		for(auto st : p->get_next_state(goal_ptr)){
			st->calc_cost_h(goal_ptr);
			size_t oi = num_in_list(*st, openlist);
			size_t ci = num_in_list(*st, closedlist);

			if(oi != -1){
				//オープンリストに入っているとき
				if(openlist[oi]->get_totalcost() > st->get_totalcost()){
					//openlist[oi]->set_parent(p);
					//openlist[oi]->calc_cost_h(goal_ptr);
					openlist[oi] = st;
					openlist[oi]->set_parent(p);
				}

			}else if(ci != -1){
				if(closedlist[ci]->get_totalcost() > st->get_totalcost()){
					//closedlist[ci]->set_parent(p);
					//closedlist[ci]->calc_cost_h(goal_ptr);
					st->set_parent(p);
					closedlist.erase(closedlist.begin() + ci);
					openlist.push_back(st);
				}
			}else{
				//まだリストに入っていないのでオープンリストに追加
				st->set_parent(p);
				st->calc_cost_h(goal_ptr);
				openlist.push_back(st);
			}
		}
		openlist.erase(std::remove(openlist.begin(), openlist.end(), p), openlist.end());
		closedlist.push_back(p);
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
