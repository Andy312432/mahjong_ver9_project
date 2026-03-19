#pragma once

#ifndef _PRIVATEHAND_H_
#define _PRIVATEHAND_H_

#include <iostream>
#include <stdio.h>
#include <vector>

#include "TT.h" // Modified

using std::vector;

class PrivateHand {
private:
	vector<int> Tiles;
	char TileType[4] = { 'w', 't', 's', 'z' };
	int tileNum[34] = { 0 }; // Record the hand tile number of each tile class // Modified
public:
	PrivateHand();
	PrivateHand(const vector<int>& cards);

	bool findCard(const int& card);
	bool findDarkGong() const;

	int getTileNumArray(int* tempArray, int* tempNumArray);

	vector<int> getTiles() const;
	short getTileNum(short tileClass) const { return tileNum[tileClass]; } // Modified

	void SetHand(const vector<int>& cards);
	void AddHand(const int& card, boost::multiprecision::uint256_t& hashKey); // Modified
	void AddHand(const int& card);
	void RemoveHand(const int& card, boost::multiprecision::uint256_t& hashKey); // Modified
	void RemoveHand(const int& card);
	void RemoveHand(const vector<int>& card);
	void ShowPrivateHand() const;
	std::string PrivateHandStr() const; // Modified
	void ShowTileNum() const; // Modified
	void ShowPrivateHandName() const;
	void quickSort(int left, int right);
};
#endif
