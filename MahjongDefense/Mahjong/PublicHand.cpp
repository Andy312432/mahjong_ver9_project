//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Record the Eat, Pong, Gong tiles.
//
//=============================================================================

#include <algorithm>

#include "PublicHand.h"

PublicHand::PublicHand() {
	Tiles.reserve(17);
}

bool PublicHand::findGroup(const vector<int>& card) {
	vector< vector<int> >::iterator it = std::find(Tiles.begin(), Tiles.end(), card);
	return it != Tiles.end();
}

bool PublicHand::findBuGong(int takeTile) const {
	for (int i = 0; i < (int)Tiles.size(); i++) {
		int count = 0;
		for (int j = 0; j < (int)Tiles[i].size(); j++) {
			if (Tiles[i][j] / 10 == takeTile / 10) {
				count++;
				if (count == 3) {
					return true;
				}
			}
			else {
				break;
			}
		}
	}
	return false;
}

/// 明牌的部分有幾張牌
int PublicHand::getGroupNum() const {
	return (int)Tiles.size();
}

vector< vector<int> > PublicHand::getTiles() const {
	return Tiles;
}

void PublicHand::AddHand(const vector<int>& card, boost::multiprecision::uint256_t& hashKey, short* oppTileNum) {
	Tiles.push_back(card);
	for (int i = 0; i < card.size(); i++) { // Modified
		short cardClass = (card[i] / 100 - 1) * 9 + (card[i] / 10 % 10 - 1); // Modified
		if (oppTileNum[cardClass] > 0) hashKey ^= oppTileNumHash[(cardClass << 2) + oppTileNum[cardClass] - 1]; // Modified
		hashKey ^= oppTileNumHash[(cardClass << 2) + (++oppTileNum[cardClass]) - 1]; // Modified
	}
}

void PublicHand::AddHand(const vector<int>& card) {
	Tiles.push_back(card);
}

void PublicHand::AddBuGong(const int& card, boost::multiprecision::uint256_t& hashKey, short* oppTileNum) {
	short cardClass = (card / 100 - 1) * 9 + (card / 10 % 10 - 1); // Modified
	hashKey ^= oppTileNumHash[(cardClass << 2) + oppTileNum[cardClass] - 1]; // Modified
	hashKey ^= oppTileNumHash[(cardClass << 2) + (++oppTileNum[cardClass]) - 1]; // Modified
	int a = card / 100;
	int b = (card / 10) % 10;
	vector< vector<int> >::iterator it;
	for (it = Tiles.begin(); it != Tiles.end(); it++) {
		int tempA = (*it)[0] / 100;
		int tempB = ((*it)[0] / 10) % 10;
		if (tempA == a && tempB == b) {
			(*it).push_back(card);
		}
	}
}

void PublicHand::AddBuGong(const int& card) {
	int a = card / 100;
	int b = (card / 10) % 10;
	vector< vector<int> >::iterator it;
	for (it = Tiles.begin(); it != Tiles.end(); it++) {
		int tempA = (*it)[0] / 100;
		int tempB = ((*it)[0] / 10) % 10;
		if (tempA == a && tempB == b) {
			(*it).push_back(card);
		}
	}
}

void PublicHand::ShowGroupName() const {
	for (int i = 0; i < (int)Tiles.size(); i++) {
		for (int j = 0; j < (int)Tiles[i].size(); j++) {
			printf("%d%c ", Tiles[i][j] % 9 + 1, kTileType[Tiles[i][j] / 9]);
		}
		printf(" / ");
	}
	printf("\n");
	fflush(stdout);
}

void PublicHand::ShowGroup() const {
	for (int i = 0; i < (int)Tiles.size(); i++) {
		for (int j = 0; j < (int)Tiles[i].size(); j++) {
			printf("%d  ", Tiles[i][j]);
		}
		printf(" / ");
	}
	printf("\n");
	fflush(stdout);
}