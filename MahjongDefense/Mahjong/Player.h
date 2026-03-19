#pragma once

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <algorithm>
#include <fstream>

#include "PrivateHand.h"
#include "PublicHand.h"
#include "Table.h"
#include "WallTiles.h"
#include "GameStatue.h"

class Player {
protected:
	Table table;
	WallTiles wallTiles;
	PublicHand publicHand[4]; /// 場上4位玩家的明牌資訊
	PrivateHand privateHand;
	boost::multiprecision::uint256_t hashKey = 0; // Modified

	static const std::string kCommandString[];

	int ID, mode = 0, lastCommand, takeTile, oppGroup = 0; /// oppGroup: 四家吃碰明槓總牌數
	short oppTileNum[34] = { 0 }; // Modified

	virtual vector<int> dealEat() = 0;
	virtual vector<int> dealPong() = 0;
	virtual vector<int> dealMingGong() = 0;
	virtual vector<int> dealDarkGong() = 0;
	virtual vector<int> dealBuGong(const int&) = 0;
	virtual vector<int> dealHu() = 0;
	virtual vector<int> dealThrow() = 0;
	virtual inline void DealTake(const int&) = 0;
	virtual void DealRemainTiles(const int&) = 0;
	virtual inline void MoveThrow(const vector<int>&) = 0;
	virtual void ShowFunctionTime() = 0;
public:
	double totalDuration = 0; // For time measurement
	int treeSize = 0; // Measure the number of nodes in the search tree

	Player(const int& ID, const PrivateHand& cards);
	Player(const int& ID, const vector<int>& cards);
	virtual ~Player() {};
	void dealCommand(const int& Command, const vector<int>& Data);
	vector<int> dealCommand(int& Command, int& gameCount);

	void ShowMessage(const int& Command);
	void ShowMessage(const int& Command, const int& Player, const vector<int>& Data);
	void ShowMessage(const vector<int>& Data);

	void ShowPrivateHand();
	void ShowPublicHand();
	void ShowTileSea();

	int getID() { return ID; }
};

#endif