#pragma once
#include "Table.h"
#include "GameStatue.h"

//將 暫時改table又還原這件事給包起來，比較好看程式碼
class TableIdActionGuard {
	Table* table;
	int* tableID;
	int tile;
	int rollbackAction;
public:
	inline TableIdActionGuard(Table* table, int* tableID, int tile, int applyAction, int rollbackAction) :
		table(table), tableID(tableID), tile(tile), rollbackAction(rollbackAction) {
		table->UpdateTableID(tableID, tile, applyAction);
	}
	inline ~TableIdActionGuard() { table->UpdateTableID(tableID, tile, rollbackAction); };
	TableIdActionGuard(const TableIdActionGuard&) = delete;
	TableIdActionGuard& operator=(const TableIdActionGuard&) = delete;
};

//同上，但兩個的版本
class TableIdPairActionGuard {
	Table* table;
	int* tableID;
	int firstTile;
	int secondTile;
	int rollbackAction;
public:
	inline TableIdPairActionGuard(Table* table, int* tableID, int firstTile, int secondTile, int applyAction, int rollbackAction) :
		table(table), tableID(tableID), firstTile(firstTile), secondTile(secondTile), rollbackAction(rollbackAction) {
		table->UpdateTableID(tableID, firstTile, applyAction);
		table->UpdateTableID(tableID, secondTile, applyAction);
	}
	inline ~TableIdPairActionGuard() {
		table->UpdateTableID(tableID, secondTile, rollbackAction);
		table->UpdateTableID(tableID, firstTile, rollbackAction);
	}
	TableIdPairActionGuard(const TableIdPairActionGuard&) = delete;
	TableIdPairActionGuard& operator=(const TableIdPairActionGuard&) = delete;
};

bool IsTryEatLeftPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard);
bool IsTryEatMiddleAdjacentPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard);
bool IsTryEatMiddleGapPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard);
bool IsTryEatRightPattern(const int* handTileTypes, const int handTileTypeCount, const int tileTypeIndex, const int seaCard);
int QueryListenNumAfterTryThrow(Table* table, int* tableID, int tile, int needGroup, bool&);
int QueryListenNumAfterTryTake(Table* table, int* tableID, int tile, int needGroup, bool&);
int QueryListenNumAfterTryPong(Table* table, int* tableID, int tile, int needGroup, bool&);

TableIdPairActionGuard MakeTryEatLeftGuard(Table* table, int* tableID, int firstTile, int secondTile);//To delete
TableIdPairActionGuard MakeTryEatMiddleGuard(Table* table, int* tableID, int firstTile, int secondTile);//To delete
TableIdPairActionGuard MakeTryEatRightGuard(Table* table, int* tableID, int firstTile, int secondTile);//To delete