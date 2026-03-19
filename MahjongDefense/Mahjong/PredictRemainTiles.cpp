//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Record tiles that have not yet appeared (remaining tiles).
//
//=============================================================================

#include<iostream>
#include <string>
#include <stdexcept>
#include <random>

/// 模擬及判斷場上未出現的牌
#include "PredictRemainTiles.h"

#ifdef __GNUC__
#include "pcg_random.hpp"
#endif

/// 預設remainTiles = {0, 0, 0, 0, 1, 1, 1, 1, ......, 33, 33, 33, 33}
PredictRemainTiles::PredictRemainTiles() {
	remainTiles.resize(136);
	for (int i = 0; i < 34; i++) {
		for (int j = 0; j < 4; j++) {
			remainTiles[i << 2 | j] = i;
		}
	}
	appearNum = 0; /// 全部的牌都還沒出現過
}

constexpr bool PredictRemainTiles::searchRemainTiles(const int& cardID) {
	return appearTiles[cardID] >= 4 ? true : false;
}

/// 從還沒出現過的牌堆裡隨機挑一張牌
int PredictRemainTiles::TakeTile() {
	if (appearNum >= 136) { /// 全部的牌都出現過了 (136 = 4 * 9 * 3 + 4 * 7)
		fprintf(stderr, "over 136\n");
		fflush(stderr);
		throw std::invalid_argument("over 136");
	}
#ifdef __GNUC__
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
#elif _MSC_VER
	std::random_device rd;
	std::mt19937 rng(rd());
#endif
	std::uniform_int_distribution<int> uniform_dist(0, (136 - appearNum - 1));
	int randomIndex = uniform_dist(rng); /// 隨機選到的index
	int card = remainTiles[randomIndex];
	++appearNum;
	++appearTiles[card]; /// 把挑到的那張牌寫到目前已出現的牌(appearTile)集合
	//RemoveRemainTiles(card); /// 從remainTiles中移除
	remainTiles.erase(remainTiles.begin() + randomIndex);
	return card; /// 回傳挑到的那張牌
}

void PredictRemainTiles::TakeSpecificTile(int tile) {
	++appearNum;
	++appearTiles[tile];
	RemoveRemainTiles(tile); /// 從remainTiles中移除
}

/// 還沒有出現過的牌數有多少
/*constexpr*/ int PredictRemainTiles::getRemainTotalTilesNum() const {
	return 136 - appearNum;
}

/*constexpr*/ int PredictRemainTiles::getRemainTilesNum(const int& card) const {
	if (card >= 34) {
		printf("getRemainTilesNum Error: %d\n", card);
		throw std::invalid_argument("getRemainTilesNum Error : card is over 34");
	}
	return 4 - appearTiles[card];
}

// Return the number of classes of remain tiles, and write the classes into classArray
int PredictRemainTiles::getRemainTilesClasses(int* classArray) const {
	int classNum = 0;
	for (int i = 0; i < 34; i++) {
		if (appearTiles[i] < 4) {
			*(classArray + classNum++) = i;
		}
	}
	return classNum;
}

/// 把牌加到已經出現的集合裡
void PredictRemainTiles::AddCard(const int& card) {
	if (appearTiles[card] < 4) {
		appearTiles[card]++;
		appearNum++;
		RemoveRemainTiles(card);
	}
	else {
		printf("Add AppearTiles Error!, Card: %d\n", card);
		fflush(stdout);
		throw std::invalid_argument("Add AppearTiles Error : appear times is over 4");
	}
}

void PredictRemainTiles::RemoveRemainTiles(const int& card) {
	std::vector<int>::iterator it = std::find(remainTiles.begin(), remainTiles.end(), card);
	if (it != remainTiles.end()) {
		remainTiles.erase(it);
	}
	else {
		printf("Remove Remain Tiles Error!, Card: %d\n", card);
		fflush(stdout);
		throw std::invalid_argument("Remove Remain Tiles Error.");
	}
}

// for simulating opponent AI
void PredictRemainTiles::AddBackTiles(const vector<int>& hand) {
	for (unsigned i = 0; i < hand.size(); i++) {
		if (appearTiles[hand[i]] > 0) {
			appearTiles[hand[i]]--;
			appearNum--;
			remainTiles.push_back(hand[i]);
		}
		else {
			printf("Add AppearTiles Error!, Card: %d\n", hand[i]);
			fflush(stdout);
			throw std::invalid_argument("Add AppearTiles Error : appear times is over 4");
		}
	}
}