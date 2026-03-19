#pragma once
#ifndef __HUMAN_H__
#define __HUMAN_H__

#include "PrivateHand.h"
#include "PublicHand.h"
#include "Table.h"
#include "Player.h"

class Human : public Player {
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
	Human(const int& ID, const PrivateHand& cards);
	Human(const int& ID, const vector<int>& cards);
};

#endif