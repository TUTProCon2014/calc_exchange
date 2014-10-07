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
#include <unordered_map>

namespace procon { namespace greedy_calc_exchange{

using namespace utils;


//解答生成クラス
class Answer{
private:
	std::vector<std::string> _ans;		//解答
	Index2D _select;					//選択中の断片座標
	int _select_num;					//選択数
	Index2D _before;					//1つ前の選択座標

public:
	//コンストラクタ
	Answer(){
		_ans.push_back("");					//選択回数用にあけておく
		_select = makeIndex2D(100, 100);	//100,100にしておけばbeforeが次のselectとかぶる心配がない
		_select_num = 0;
		_before = makeIndex2D(100, 100);	
	}

	//選択操作
	void set_select(Index2D s){
		_before = makeIndex2D(100, 100);	//選択後に交換してbeforeとselectが一致するのは問題ない
		_select = s;						//選択断片のセット
		_select_num++;						//選択数をカウント

		//選択操作を解答に追加
		std::stringstream ss;
		ss << std::hex << std::uppercase << _select[1] << _select[0]; //16進数変換
		_ans.push_back(ss.str());					//選択画像位置を解答に追加
		_ans.push_back("");							//交換回数用にあけておく
		_ans.push_back("");							//交換操作格納用
	}

	//交換操作
	void exchange(std::string dir){
		_ans[_ans.size()-1] += dir;
	}

	//選択断片位置のチェンジ(解答には追加しない)
	void move_select(Index2D s){
		_before = _select;				//以前のselectをbeforeにセット
		_select = s;
	}

	//交換操作文字列の最後の文字をクリア
	void remove(){
		//_ans[_ans.size()-1].erase(--_ans[_ans.size()-1].end());
        auto& str = _ans[_ans.size()-1];
        if (str.size())
            str.resize(str.size()-1);
	}

	//最終処理
	void finish(){

		_ans[0] += std::to_string(_select_num);	//選択回数の代入

		size_t idx = 1;				//ansの行番号
		for(size_t i=0; i < _select_num; i++){
			idx++;												//選択断片位置の行
			_ans[idx] = std::to_string(_ans[idx+1].size());	//交換回数の代入
			idx++;
			idx++;												//交換操作の行
		}

	}

	//ゲッター
	const std::vector<std::string>& ans() const {
		return _ans;
	}

	Index2D select() const {
		return _select;
	}

	Index2D before() const {
		return _before;
	}

};


//prototype宣言
void move_piece(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>> const & used, const std::vector<std::vector<ImageID>>& target, const Index2D& tgt, const Index2D& dst);
std::vector<std::vector<ImageID>> make_start_state(const size_t & height, const size_t & width);
Index2D search_piece(ImageID piece, const std::vector<std::vector<ImageID>> & from);
void target_piece_clear(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, Index2D tgt, Index2D dist, const Direction dir);
void exchange(Answer& ans, std::vector<std::vector<ImageID>>& state, const Index2D moved, Direction dir);
void parking_in_garage(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, ImageID tgt1id, ImageID tgt2id);
ImageID makeImageID(size_t i, size_t j);
void first_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width);
void second_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width);
void final_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width);
bool is_state_same(const std::vector<std::vector<ImageID>>& a, const std::vector<std::vector<ImageID>>& b);


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
void move_target(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, int disty, int distx, Index2D tgt){
				//tgtの位置を通れないようにする(そうしないとせっかくselectを移動させてもそこにtgtがいなくなってしまう)
				used[tgt[0]][tgt[1]] = true;	
				//selectをtgtの移動したい位置にもっていく
				move_piece(ans, state, used, target, tgt, makeIndex2D(disty, distx));	
				//usedをもとにもどす
				used[tgt[0]][tgt[1]] = false;	
}

//tgtの断片をdistの位置まで持っていく
//dir:今つめている方向
void target_piece_clear(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, Index2D tgt, Index2D dist, const Direction dir){

	size_t dy = std::abs((int)tgt[0] - (int)dist[0]);		//目的位置までのy距離
	size_t dx = std::abs((int)tgt[1] - (int)dist[1]);		//目的位置までのx距離
	Direction ydir = (dist[0] < tgt[0]) ? Direction::up : Direction::down;		//移動するべき方向の決定
	Direction xdir = (dist[1] < tgt[1]) ? Direction::left : Direction::right;
	int my[4] = {0, -1, 0, 1};	//移動量
	int mx[4] = {1, 0, -1, 0};
	size_t move_cnt = 0;		//xかyかどちらに移動するかを決定するためのカウント値

	//移動処理
	if(dir == Direction::up){
		//目的地のy座標+1までx移動、y移動、x移動、y移動・・・の順に動いたほうが交換回数減る
		while(dy > 1){	//目的地までのy距離=1になるまでこの動作

			if(dx > 0 && move_cnt % 2 == 0){	//x移動
				dx--;	//あとで使うので移動した分減らしておく
				
				//tgtの移動
				move_target(ans, state, used, target, tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt);	

				//selectとtgtの交換
				Index2D moved = tgt;	//selectの移動先
				tgt = ans.select();		//tgtの座標の変更

				if(xdir == Direction::left){
					exchange(ans, state, moved, Direction::right);		//xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
				}else{
					exchange(ans, state, moved, Direction::left);
				}

			}else if(dy > 0){		//y移動
				dy--;	//あとで使うので移動した分減らしておく
				
				//tgtの移動
				move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], tgt[1], tgt);	

				//selectとtgtの交換
				Index2D moved = tgt;	//selectの移動先
				tgt = ans.select();		//tgtの座標の変更
				if(ydir == Direction::up){
					exchange(ans, state, moved, Direction::down);		//ydirはtgtから見た時の方向なのでselectからみると逆
				}else{
					exchange(ans, state, moved, Direction::up);
				}
			}
			move_cnt++;		//count

		}


		//残りのx座標を合わせる(x座標からやることで一度も壁にぶつからずに持っていける)
		for(size_t i=0; i < dx; i++){

			//tgtの移動
			Index2D tmp = makeIndex2D(tgt[0], (int)tgt[1] + mx[(size_t)xdir]);
			move_target(ans, state, used, target, tmp[0], tmp[1], tgt);	

			//selectとtgtの交換
			Index2D moved = tgt;	//selectの移動先
			tgt = ans.select();		//tgtの座標の変更

			if(xdir == Direction::left){
				exchange(ans, state, moved, Direction::right);		//xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
			}else{
				exchange(ans, state, moved, Direction::left);
			}
			
		}

		//y座標を一致させる
		for(size_t i=0; i < dy; i++){

			//tgtの移動
			move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], tgt[1], tgt);	

			//selectとtgtの交換
			Index2D moved = tgt;	//selectの移動先
			tgt = ans.select();		//tgtの座標の変更
			if(ydir == Direction::up){
				exchange(ans, state, moved, Direction::down);		//ydirはtgtから見た時の方向なのでselectからみると逆
			}else{
				exchange(ans, state, moved, Direction::up);
			}
		}

	}else if(dir == Direction::left){	//左につめているとき

		while(dx > 1){	//目的地までのx距離が1になるまでななめ動作
		
			if(dx > 0 && move_cnt % 2 == 0){	//x移動

				dx--;	//あとで使うので移動した分減らしておく
				
				//tgtの移動
				move_target(ans, state, used, target, tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt);	

				//selectとtgtの交換
				Index2D moved = tgt;	//selectの移動先
				tgt = ans.select();		//tgtの座標の変更

				if(xdir == Direction::left){
					exchange(ans, state, moved, Direction::right);		//xdirはtgtから見た時の方向なのでselectからみると逆方向の移動となる
				}else{
					exchange(ans, state, moved, Direction::left);
				}

			}else if(dy > 0){		//y移動
				dy--;	//あとで使うので移動した分減らしておく
				
				//tgtの移動
				move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], tgt[1], tgt);	

				//selectとtgtの交換
				Index2D moved = tgt;	//selectの移動先
				tgt = ans.select();		//tgtの座標の変更
				if(ydir == Direction::up){
					exchange(ans, state, moved, Direction::down);		//ydirはtgtから見た時の方向なのでselectからみると逆
				}else{
					exchange(ans, state, moved, Direction::up);
				}
			}
			move_cnt++;		//count

		}

		//y座標をまず合わせる(y座標からやることで一度も壁にぶつからずに持っていける)
		for(size_t i=0; i < dy; i++){

			//tgtの移動
			move_target(ans, state, used, target, (int)tgt[0] + my[(size_t)ydir], tgt[1], tgt);	

			//selectとtgtの交換
			Index2D moved = tgt;	//selectの移動先
			tgt = ans.select();		//tgtの座標の変更
			if(ydir == Direction::up){
				exchange(ans, state, moved, Direction::down);		//ydirはtgtから見た時の方向なのでselectからみると逆
			}else{
				exchange(ans, state, moved, Direction::up);
			}

		}
		//x座標を一致させる
		for(size_t i=0; i < dx; i++){
				
			//tgtの移動
			move_target(ans, state, used, target, tgt[0], (int)tgt[1] + mx[(size_t)xdir], tgt);	

			//交換してtgtを1つ動かす
			Index2D moved = tgt;	//selectの移動先
			tgt = ans.select();		//tgtの座標の変更
			if(xdir == Direction::left){
				exchange(ans, state, moved, Direction::right);		//xdirはtgtから見た時の方向なのでselectからみると逆
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
void move_piece(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>> const & used, const std::vector<std::vector<ImageID>>& target, const Index2D& tgt, const Index2D& dst){

	using HashMap = std::unordered_map<Index2D, DijkstraData, DijkstraData::Index2DHash>;

	HashMap hashMap;
	hashMap[ans.select()] = DijkstraData(ans, state, 0);

	while(1){
		HashMap nextGen = hashMap;

		// nextGenに必要ならば追加する
		auto pushToNextGen = [&nextGen, &dst, &target, &used](DijkstraData const & e, ptrdiff_t i, ptrdiff_t j, Direction dir) -> bool
		{
			if(i < 0 || j < 0 || i >= e.state.size() || j >= e.state[i].size())
				return false;

			if(used[i][j])
				return false;

			DijkstraData next = e;

			// 入れ替え前の、選択している位置bSelと、移動先の位置bNei
			auto bSel = next.answer.select();										// 移動前
			auto bNei = Index2D({static_cast<size_t>(i), static_cast<size_t>(j)});	// 移動先
			auto bNeiDST = search_piece(next.state[bNei[0]][bNei[1]], target);			// 移動先の断片が行きたい場所
			exchange(next.answer, next.state, bNei, dir);

			// 入れ替え後の、選択している位置aSelと、入れ替えされた他の断片の位置aNei
			auto aSel = bNei,		// 結局swap
			     aNei = bSel;		// ditto

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


	//前回のselect座標を取得
	Index2D before = ans.before();

	//stateを変化
	Index2D sel = ans.select();		//一時的にselectを代入

	//状態の入れ替え
	ImageID temp = state[moved[0]][moved[1]];
	state[moved[0]][moved[1]] = state[sel[0]][sel[1]];
	state[sel[0]][sel[1]] = temp;

	//selectの座標をチェンジ
	ans.move_select(moved);

	//現在のselect座標取得
	Index2D now = ans.select();

	if(now[0] == before[0] && now[1] == before[1]){

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

//車庫入れ
//tgt1が先に入れる方(端にくる断片)
void parking_in_garage(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, Index2D tgt1, Index2D tgt2, Index2D dist1, Index2D dist2, Direction dir, ImageID tgt1id, ImageID tgt2id){

	Index2D moved;		//移動時に使う

	if(dir == Direction::up){	//上方向につめている

		//問題のある状況を除去
		for(size_t i=0; i < 2; i++){
			for(size_t j=0; j < 2; j++){
				if(dist1[0]+i == tgt2[0] && dist1[1]+j == tgt2[1]){						//この4マスに入っている場合は、そこから断片2を追い出す

					Index2D dist3 = makeIndex2D(dist1[0] + 2, dist1[1] + j);			//追い出し先の座標
					target_piece_clear(ans, state, used, target, tgt2, dist3, Direction::up);	//追い出す
						
				}
			}
		}

		//上の移動によりtgt1の座標が変わる場合があるのでもう一度tgt1の座標検索
		tgt1 = search_piece(tgt1id, state);
		//tgt1を指定した位置に入れる
		target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::up);

		//tgt1の移動によりtgt2の座標が変わる場合があるのでもう一度tgt2の座標検索
		tgt2 = search_piece(tgt2id, state);
		if(tgt2[0] == dist1[0] && tgt2[1] == dist1[1] + 1){		//絶対車庫入れできない状況になったときのスペシャルmove

			used[dist1[0]][dist1[1]] = true;
			used[dist1[0]][dist1[1]+1] = true;

			move_piece(ans, state, used, target, tgt2,  makeIndex2D(dist1[0] + 1, dist1[1] + 1));		//右上隅の１つした
			used[dist1[0]][dist1[1]] = false;
			used[dist1[0]][dist1[1]+1] = false;

			//select上移動
			moved = makeIndex2D(dist1[0], dist1[1]+1);	//一番右上隅							
			tgt2 = ans.select();								//tgt2の座標の変更
			exchange(ans, state, moved, Direction::up);			//selectの上移動
			//select左移動
			moved = makeIndex2D(dist1[0], dist1[1]);	//一番右上隅の１つ左						
			tgt1 = ans.select();								//tgt1の座標の変更
			exchange(ans, state, moved, Direction::left);		//selectの左移動

			//tgt2を離す
			target_piece_clear(ans, state, used, target, tgt2, makeIndex2D(dist1[0] + 2, dist1[1] + 1), Direction::up);
			
			//tgt1を元の位置に戻す
			used[dist1[0] + 2][dist1[1] + 1] = true;
			target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::up);
			used[dist1[0] + 2][dist1[1] + 1] = false;
		}else if(tgt2[0] == dist1[0] + 1 && tgt2[1] == dist1[1] + 1){
			//tgt1をつめたときにtgt1とtgt2で閉じ込められる場合
			
			//select左移動
			moved = makeIndex2D(dist1[0], dist1[1]);	//一番右上隅の１つ左						
			tgt1 = ans.select();								//tgt1の座標の変更
			exchange(ans, state, moved, Direction::left);		//selectの左移動

			//tgt2を離す
			target_piece_clear(ans, state, used, target, tgt2, makeIndex2D(dist1[0] + 2, dist1[1] + 1), Direction::up);
			
			//tgt1を元の位置に戻す
			used[dist1[0] + 2][dist1[1] + 1] = true;
			target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::up);
			used[dist1[0] + 2][dist1[1] + 1] = false;
		}

		//もう一度tgt2の座標検索
		tgt2 = search_piece(tgt2id, state);
		//tgt2を指定した位置に入れる(このときtgt1を移動しないようにする)
		used[dist1[0]][dist1[1]] = true;
		target_piece_clear(ans, state, used, target, tgt2, dist2, Direction::up);
		used[dist1[0]][dist1[1]] = false;

		//tgt2の現在位置を取得
		tgt2 = search_piece(tgt2id, state);
		//指定位置まで選択断片を移動する
		used[dist2[0]][dist2[1]] = true;														//tgt2の位置を通れないようにする
		move_piece(ans, state, used, target, tgt2,  makeIndex2D(dist1[0], dist1[1] + 1));		//一番右上隅
		used[dist2[0]][dist2[1]] = false;														//usedをもとにもどしておく

		//車庫入れ交換移動
		moved = dist1;									//selectはtgt1の位置に移動
		tgt1 = ans.select();							//tgt1の座標の変更
		exchange(ans, state, moved, Direction::left);	//selectの左移動

		moved = dist2;									//selectはtgt2の位置に移動
		tgt2 = ans.select();							//tgt2の座標の変更
		exchange(ans, state, moved, Direction::down);	//selectの下移動
			
	}else if(dir == Direction::left){ //左方向につめている
		
		//問題のある状況を除去
		for(size_t i=0; i < 2; i++){
			for(size_t j=0; j < 2; j++){
				if(dist1[0]-i == tgt2[0] && dist1[1]+j == tgt2[1]){									//この4マスに入っている場合は、そこから断片2を追い出す

					Index2D dist3 = makeIndex2D((int)dist1[0] - (int)i, (int)dist1[1] + (int)2);	//追い出し先の座標
					target_piece_clear(ans, state, used, target, tgt2, dist3, Direction::left);				//追い出す

				}
			}
		}

		//上の移動によりtgt1の座標が変わる場合があるのでもう一度座標検索
		tgt1 = search_piece(tgt1id, state);
		//tgt1を指定した位置に入れる
		target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::left);

		//tgt1の移動によりtgt2の座標が変わる場合があるのでもう一度座標検索
		tgt2 = search_piece(tgt2id, state);
		if(tgt2[0] == dist1[0] - 1 && tgt2[1] == dist1[1]){			//絶対車庫入れできない状況になったときのスペシャルmove

			used[dist1[0]][dist1[1]] = true;
			used[dist1[0]-1][dist1[1]] = true;

			move_piece(ans, state, used, target, tgt2,  makeIndex2D(dist1[0] - 1, dist1[1] + 1));		//左上隅の１つ右
			used[dist1[0]][dist1[1]] = false;
			used[dist1[0]-1][dist1[1]] = false;

			//select左移動
			moved = makeIndex2D(dist1[0] - 1, dist1[1]);		//一番左上隅							
			tgt2 = ans.select();								//tgt2の座標の変更
			exchange(ans, state, moved, Direction::left);		//selectの上移動
			//select下移動
			moved = makeIndex2D(dist1[0], dist1[1]);			//一番右上隅の１つ左						
			tgt1 = ans.select();								//tgt1の座標の変更
			exchange(ans, state, moved, Direction::down);		//selectの左移動

			//tgt2を離す
			target_piece_clear(ans, state, used, target, tgt2, makeIndex2D(dist1[0] - 1, dist1[1] + 2), Direction::left);
			
			//tgt1を元の位置に戻す
			used[dist1[0] - 1][dist1[1] + 2] = true;
			target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::left);
			used[dist1[0] - 1][dist1[1] + 2] = false;
		}else if(tgt2[0] == dist1[0] - 1 && tgt2[1] == dist1[1] + 1){
			//tgt1をつめたときにtgt1とtgt2で閉じ込められる場合

			//select下移動
			moved = makeIndex2D(dist1[0], dist1[1]);			//一番右上隅の１つ左						
			tgt1 = ans.select();								//tgt1の座標の変更
			exchange(ans, state, moved, Direction::down);		//selectの左移動

			//tgt2を離す
			target_piece_clear(ans, state, used, target, tgt2, makeIndex2D(dist1[0] - 1, dist1[1] + 2), Direction::left);
			
			//tgt1を元の位置に戻す
			used[dist1[0] - 1][dist1[1] + 2] = true;
			target_piece_clear(ans, state, used, target, tgt1, dist1, Direction::left);
			used[dist1[0] - 1][dist1[1] + 2] = false;
		}

		//もう一度座標検索
		tgt2 = search_piece(tgt2id, state);
		//tgt2を指定した位置に入れる(tgt1を動かさないようにする)
		used[dist1[0]][dist1[1]] = true;
		target_piece_clear(ans, state, used, target, tgt2, dist2, Direction::left);
		used[dist1[0]][dist1[1]] = false;


		//tgt2の現在位置を取得
		tgt2 = search_piece(tgt2id, state);
		//指定位置まで選択断片を移動する
		used[dist2[0]][dist2[1]] = true;													//tgt2の位置を通れないようにする
		move_piece(ans, state, used, target, tgt2,  makeIndex2D(dist1[0] - 1, dist1[1]));	//一番左下隅
		used[dist2[0]][dist2[1]] = false;													// usedをもとにもどしておく

		//車庫入れ交換移動
		moved = dist1;									//selectはtgt1の位置に移動
		tgt1 = ans.select();							//tgt1の座標の変更
		exchange(ans, state, moved, Direction::down);	//selectの下移動

		moved = dist2;									//selectはtgt2の位置に移動
		tgt2 = ans.select();							//tgt2に座標の変更
		exchange(ans, state, moved, Direction::right);	//selectの右移動

	}
}

//上からheight-2までを完成させる
void first_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width){

	for(size_t i=0; i < height-2; i++){
		for(size_t j=0; j < width-1; j++){

			Index2D dist = makeIndex2D(i, j);								//目的座標
			Index2D tgt = search_piece(target[dist[0]][dist[1]], state);	//disの位置にくるべきpieceの位置を探索

			if(j < width-2){	//愚直につめていく

				target_piece_clear(ans, state, used, target, tgt, dist, Direction::up);
				used[dist[0]][dist[1]] = true;								//正しい位置に収められた断片は移動不可にする

			}else{				//車庫入れ

				Index2D dist2 = makeIndex2D(i+1, j);						//目的座標2(distと同じx座標で1つy座標が大きい値)
				Index2D tgt2 = search_piece(target[i][j+1], state);			//断片画像は同じy座標で最も右の断片

				//車庫入れ本体
				parking_in_garage(ans, state, used, target, tgt2, tgt, dist, dist2, Direction::up, target[i][j+1], target[dist[0]][dist[1]]);

				//右端から1つ隣をusedにする
				used[dist[0]][dist[1]] = true;		
				//右端をusedにする
				used[dist[0]][dist[1]+1] = true;	
							
			}

		}
	}
}

//右下部分に4マスだけ残してそれ以外の左下部分をすべて完成させる
void second_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width){

	for(size_t j=0; j < width-2; j++){
		for(size_t i=height-1; i < height; i++){

			Index2D dist = makeIndex2D(i, j);								//目的座標
			Index2D tgt = search_piece(target[dist[0]][dist[1]], state);	//disの位置にくるべきpieceの位置を探索
			
			Index2D dist2 = makeIndex2D(i, j+1);							//目的座標2(distと同じy座標で1つx座標が大きい値)
			Index2D tgt2 = search_piece(target[i-1][j], state);				//断片画像は同じx座標で最も右の断片


			//車庫入れ本体
			parking_in_garage(ans, state, used, target, tgt2, tgt, dist, dist2, Direction::left, target[i-1][j], target[dist[0]][dist[1]]);

			//左下すみをusedにする
			used[dist[0]][dist[1]] = true;		
			//左下隅の1つ上をusedにする
			used[dist[0]-1][dist[1]] = true;	

		}
	}

}

//右下部分の4マスを完成させる
void final_solve(Answer& ans, std::vector<std::vector<ImageID>>& state, std::vector<std::vector<bool>>& used, const std::vector<std::vector<ImageID>>& target, const size_t height, const size_t width){

	//右下端の1つ上
	Index2D dist = makeIndex2D(height - 2, width - 1);					//目的座標
	Index2D tgt = search_piece(target[dist[0]][dist[1]], state);		//distの位置にくるべきpieceの位置を探索

	target_piece_clear(ans, state, used, target, tgt, dist, Direction::up);		//目的座標へ移動
	used[dist[0]][dist[1]] = true;										//正しい位置に収められた断片は移動不可にする
	
	//右下端
	move_piece(ans, state, used, target,  dist, makeIndex2D(height-1, width-1));	//いままで必死に働いたselectを正しい位置にもっていく
	used[height-1][width-1] = true;										//正しい位置に収められた断片を移動不可にする

	//まだ答えになっていなかったら、選択を変える
	//右下端の1つ左
	if(!is_state_same(state, target)){

		dist = makeIndex2D(height - 1, width - 2);						//目的座標
		tgt = search_piece(target[dist[0]][dist[1]], state);			//distの位置にくるべきpieceの位置を探索

		//選択の変更
		ans.set_select(tgt);	
		//selectを正しい位置にもっていく
		move_piece(ans, state, used, target,  makeIndex2D(height-1, width-1), makeIndex2D(height-1, width-2));	
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
			
			if(t1[0] != t2[0] || t1[1] != t2[1]) return false;		//1つでも違っていればfalse
			
		}
	}

	return true;	//1つも違っていなければtrue
}


//かなめの関数
std::vector<std::string> greedy_calc_exchange(std::vector<std::vector<ImageID>> const & target, int select_costrate, int exchange_costrate, size_t max_select_times){

	Answer ans;								//解答生成クラス
	size_t width = target[0].size();		//状態の幅
	size_t height = target.size();			//状態の高さ
	std::vector<std::vector<bool>> used(height, std::vector<bool>(width)) ; //使用不可フラグ

	//使用可能フラグの初期化
	for(size_t i=0; i < height; i++){
		for(size_t j=0; j < width; j++){
			used[i][j] = false;
		}
	}

	/*
	//最終状態の表示
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

	//targetでの右下にある断片の選択
	ImageID first = target[height - 1][width - 1];
	Index2D temp = first.get_index();
	ans.set_select(search_piece(first, state));		//最初の選択


	//=============交換操作算出========================

	//上からheight-2までにあるすべての断片をまず完成させる
	first_solve(ans, state, used, target, height, width);
	//右下部分に4マスだけ残してそれ以外の左下部分をすべて完成させる
	second_solve(ans, state, used, target, height, width);
	//最後に残りの4マスを完成させる
	final_solve(ans, state, used, target, height, width);


	//最終的な解答へ変換する
	ans.finish();	

	//解答を返す
	return ans.ans(); 

}

}}


