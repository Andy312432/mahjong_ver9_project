#pragma once
#define _CRT_SECURE_NO_WARNINGS_

#ifndef _PREDICTREMAINTILES_H_
#define _PREDICTREMAINTILES_H_

#include <algorithm>
#include <vector>

using std::vector;

class PredictRemainTiles {
private:
	vector<int> remainTiles;

	int appearTiles[34] = { 0 }; /// 已出現的牌種 9 * 3 + 7，每個位置有目前出現的張數(1 ~ 4)
	int appearNum = 0; /// appearTiles的大小

	void RemoveRemainTiles(const int& card);
public:
	constexpr bool searchRemainTiles(const int& cardID);

	int TakeTile();
	void TakeSpecificTile(int tile);
	/*constexpr*/ int getRemainTilesNum(const int& card) const;
	/*constexpr*/ int getRemainTotalTilesNum() const;
	int getRemainTilesClasses(int*) const;

	void AddCard(const int& card);
	void AddBackTiles(const vector<int>& hand);

	PredictRemainTiles();
};

#endif