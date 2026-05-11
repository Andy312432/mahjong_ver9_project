#pragma once

#ifndef _WALLTILES_H_
#define _WALLTILES_H_

#include <vector>

using std::vector;

static const char tileType[4] = { 'w', 't', 's', 'z' };

struct DiscardRecord {
	int playerIndex;
	int card;
	bool isTsumogiri;
};

class WallTiles {
private:
	vector<int> tileSea;
	vector<DiscardRecord> discardRecords;
	
	int appearTileNum[34] = { 0 }; // record appear tiles count
public:
	WallTiles();

	bool isTileSeaEmpty();

	int getTileSea() const;
	int getTileSeaTileNum(const int& card) const;

	vector<int> getAllTileSea() const;
	int getAllTileSeaCount() const;
	vector<int> getPlayerTileSea(const int& playerIndex) const;
	vector<DiscardRecord> getPlayerDiscardRecords(const int& playerIndex) const;
	vector<DiscardRecord> getAllDiscardRecords() const;
	int getPlayerTileSeaCount(const int& playerIndex) const;

	void AddTileSea(const int& card);
	void AddTileSea(const int& playerIndex, const int& card, const bool& isTsumogiri);
	void deleteTileSea();
	void ShowTileSea() const;
	void ShowTileSeaName() const;
};
#endif
