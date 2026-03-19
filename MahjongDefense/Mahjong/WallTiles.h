#pragma once

#ifndef _WALLTILES_H_
#define _WALLTILES_H_

using std::vector;

static const char tileType[4] = { 'w', 't', 's', 'z' };

class WallTiles {
private:
	vector<int> tileSea;
	
	int appearTileNum[34] = { 0 }; // record appear tiles
public:
	WallTiles();

	bool isTileSeaEmpty();

	int getTileSea() const;
	int getTileSeaTileNum(const int& card) const;

	vector<int> getAllTileSea() const;

	void AddTileSea(const int& card);
	void deleteTileSea();
	void ShowTileSea() const;
	void ShowTileSeaName() const;
};
#endif
