#pragma once
#ifndef __AI_H__
#define __AI_H__


#include "Table.h"
#include "Player.h"
#include "GameStatue.h"
#include "thread_pool.hpp"
#include "TT.h" // Modified
//#include "PredictRemainTiles.h"
#include "DefenseAnalyze.h"

#define parent(ptr) (nodes[ptr].p_id) // id of your parent
#define child(ptr,i) (nodes[ptr].c_id[i]) // the ith child
#define SIMULATION_NUM 20000
#define SELECTION_NUM 4 /// MCTS的select次數，等於1的話就退化成MCS
#define _C_  1.4


typedef struct Board {
	PredictRemainTiles predictRemainTiles;
	PrivateHand privateHand;
	int tableID[4] = { 0 }; /// 萬筒條字的5進位狀況(5進位9位元)
	int takeTile = -1;
	int throwTile = -1;
	int myPosition = 0; // player1 ~ player4 /// 0 ~ 3
	char listenNum = 125; // first player
	char status = 0;	// current game status /// 現在輪誰(0 ~ 3)
	char publicGroupNum = 0;
	bool is_have_eyes = false;
} Board;

class AI : public Player {
private:
	Table* table = nullptr;
	Board board;
	thread_pool pool;
	timer timer; /// 計時模擬時間
	double MinS = 99999999.0, MaxS = -99999999.0, times = 0.0;
	double throwTime = 0, dismantlingTime = 0, huTime = 0, listenTime = 0, listenTableTime = 0, MCTS_Time = 0;
	int gameTurnTimes = 0;

	int ThrowTilesAnalysis(Board& b, ThrowSet* newDismantlingResult, int* startPosition, int* throwTiles, int(*throwContentType)[4], const int& startPositionSize);
	vector<int> input(const int& Input_num, const bool& Pass);
	int ThrowList(Board& b, int* throwArray);

	constexpr bool is_terminal(const Board& b);
	constexpr bool isCanEat(int* tileArray, int* tileNumArray, const int& length, const int& seaCard);
	constexpr bool isCanPong(int* tileArray, int* tileNumArray, const int& length, const int& card);
	constexpr bool isCanGong(int* tileArray, int* tileNumArray, const int& length, const int& card);
	bool isHu(Board& b, int* tileArray, int* tileNumArray, const int& length, const int& group);
	bool isHu(Board& b, const int& group);
	inline bool isTimeUp();

	constexpr int whos_turn(const Board& b);
	constexpr int who_won(const Board& b);
	inline int Simulate(const Board& a, const int& throwCard);

	inline double UCB(const int& nodeIndex, const std::atomic<int>& treeSimTimes);
	inline void backwardPropagation(const int& startIndex, const double& newAvg, const int& newSimTimes);
	inline void MCTS(const Board& b, const int* throwList, const int& throwArraySize);
	void MakeTake(Board& b, const int& player, const int& action);
	inline void UpdateStatus(Board& b);
	void MakeThrow(Board& b, const int& card, const bool& real);
	int Partition(int* arr, int front, int end);
	void QuickSort(int* arr, int front, int end);
	void ShowGameInformation(const vector<int>& cards);
protected:
	vector<int> dealEat();
	vector<int> dealPong();
	vector<int> dealMingGong();
	vector<int> dealDarkGong();
	vector<int> dealBuGong(const int& takeTile);
	vector<int> dealHu();
	vector<int> dealThrow();

	void DealTake(const int& takeTile);
	void DealRemainTiles(const int& card);
	void ShowFunctionTime();
	void MoveThrow(const vector<int>& throwTile);
public:
	//AI(const int& ID, const PrivateHand& cards, Table* table);
	AI(const int& ID, const vector<int>& cards, Table* table);
	AI(const int& ID, const vector<int>& cards);
};

#endif