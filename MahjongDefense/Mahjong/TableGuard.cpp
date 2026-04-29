#include "TableGuard.h"

//被丟出的牌(要判斷的牌)在最左邊，自己手上有右邊兩張
bool IsTryEatLeftPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard) {
	return tileTypeIndex + 1 < handTileTypeCount
		&& handTileTypes[tileTypeIndex] == seaCard + 1
		&& handTileTypes[tileTypeIndex + 1] == seaCard + 2
		&& seaCard / 9 == handTileTypes[tileTypeIndex] / 9
		&& seaCard / 9 == handTileTypes[tileTypeIndex + 1] / 9;
}
//被丟出的牌在中間，自己手上有左右兩張
bool IsTryEatMiddleAdjacentPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard) {
	return tileTypeIndex + 1 < handTileTypeCount
		&& handTileTypes[tileTypeIndex] == seaCard - 1
		&& handTileTypes[tileTypeIndex + 1] == seaCard + 1
		&& seaCard / 9 == handTileTypes[tileTypeIndex] / 9
		&& seaCard / 9 == handTileTypes[tileTypeIndex + 1] / 9;
}
//同上，但有可能手排上已經有中間那張
bool IsTryEatMiddleGapPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard) {
	return tileTypeIndex + 2 < handTileTypeCount
		&& handTileTypes[tileTypeIndex] == seaCard - 1
		&& handTileTypes[tileTypeIndex + 2] == seaCard + 1
		&& seaCard / 9 == handTileTypes[tileTypeIndex] / 9
		&& seaCard / 9 == handTileTypes[tileTypeIndex + 2] / 9;
}

bool IsTryEatRightPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard) {
	return tileTypeIndex + 1 < handTileTypeCount
		&& handTileTypes[tileTypeIndex] == seaCard - 2
		&& handTileTypes[tileTypeIndex + 1] == seaCard - 1
		&& seaCard / 9 == handTileTypes[tileTypeIndex] / 9
		&& seaCard / 9 == handTileTypes[tileTypeIndex + 1] / 9;
}

TableIdPairActionGuard MakeTryPongGuard(Table* table, int* tableID, int tile) {
	return TableIdPairActionGuard(table, tableID, tile, tile, THROW, TAKE);
}

TableIdPairActionGuard MakeTryEatLeftGuard(Table* table, int* tableID, int firstTile, int secondTile) {
	return TableIdPairActionGuard(table, tableID, firstTile, secondTile, THROW, TAKE);
}

TableIdPairActionGuard MakeTryEatMiddleGuard(Table* table, int* tableID, int firstTile, int secondTile) {
	return TableIdPairActionGuard(table, tableID, firstTile, secondTile, THROW, TAKE);
}

TableIdPairActionGuard MakeTryEatRightGuard(Table* table, int* tableID, int firstTile, int secondTile) {
	return TableIdPairActionGuard(table, tableID, firstTile, secondTile, THROW, TAKE);
}

int QueryListenNumAfterTryThrow(Table* table, int* tableID, int tile, int needGroup, bool&) {
	TableIdActionGuard tempThrowGuard(table, tableID, tile, THROW, TAKE);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};
int QueryListenNumAfterTryTake(Table* table, int* tableID, int tile, int needGroup, bool&) {
	TableIdActionGuard tempTakeGuard(table, tableID, tile, TAKE, THROW);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};
int QueryListenNumAfterTryPong(Table* table, int* tableID, int tile, int needGroup, bool&) {
	TableIdPairActionGuard tempPongGuard = MakeTryPongGuard(table, tableID, tile);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};
int QueryListenNumAfterTryEatLeft(Table* table, int* tableID, int firstTile, int secondTile, int needGroup, bool&) {
	TableIdPairActionGuard tempEatLeftGuard = MakeTryEatLeftGuard(table, tableID, firstTile, secondTile);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};
int QueryListenNumAfterTryEatMiddle(Table* table, int* tableID, int firstTile, int secondTile, int needGroup, bool&) {
	TableIdPairActionGuard tempEatMiddleGuard = MakeTryEatMiddleGuard(table, tableID, firstTile, secondTile);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};
int QueryListenNumAfterTryEatRight(Table* table, int* tableID, int firstTile, int secondTile, int needGroup, bool&) {
	TableIdPairActionGuard tempEatRightGuard = MakeTryEatRightGuard(table, tableID, firstTile, secondTile);
	bool tempHasEyes = false;
	return table->getTilesListenNum(needGroup, tableID, tempHasEyes);
};