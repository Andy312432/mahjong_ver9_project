#pragma once

#ifndef _PUBLICHAND_H_
#define _PUBLICHAND_H_

#include <stdio.h>
#include <vector>
#include "GameStatue.h"
#include "TT.h" // Modified

using std::vector;

class PublicHand {
private:
	vector< vector<int> >Tiles;
	vector<Action> Reasons;
	const char kTileType[5] = { 'w', 't', 's', 'z', 'f' };
	int groupNum = 0;
public:
	PublicHand();

	bool findBuGong(int takeTile) const;
	bool findGroup(const vector<int>& card);

	int getGroupNum() const;
	int getGroupNumByReason(Action reason) const;
	int getEatPongGroupNum() const;
	Action getReason(int groupIndex) const;
	vector< vector<int> > getTiles() const;
	vector<Action> getReasons() const;

	void AddHand(const vector<int>& card, Action reason, boost::multiprecision::uint256_t& hashKey, short* oppTileNum); // Modified
	void AddHand(const vector<int>& card, boost::multiprecision::uint256_t& hashKey, short* oppTileNum); // Modified
	void AddHand(const vector<int>& card, Action reason);
	void AddHand(const vector<int>& card);
	void AddBuGong(const int& card, boost::multiprecision::uint256_t& hashKey, short* oppTileNum); // Modified
	void AddBuGong(const int& card);
	void ShowGroup() const;
	void ShowGroupName() const;
};
#endif
