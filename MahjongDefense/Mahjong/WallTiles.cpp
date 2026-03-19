//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Record the tile sea and simulate the wall tiles.
//
//=============================================================================

#include <random>
#include <stdexcept>

/// WallTiles建立牌海資訊
#include "WallTiles.h"

#ifdef __GNUC__
#include "pcg_random.hpp"
#endif

WallTiles::WallTiles() :tileSea() {
	tileSea.reserve(136);
}

bool WallTiles::isTileSeaEmpty() {
	return tileSea.empty();
}

// get tile sea /// 牌海中最後一張牌 (最新被捨的牌)
int WallTiles::getTileSea() const {
	int card = tileSea.back();
	return card;
}

int WallTiles::getTileSeaTileNum(const int& card) const {
	return appearTileNum[card];
}

vector<int> WallTiles::getAllTileSea() const {
	vector<int> card(tileSea);
	return card;
}

// add tile sea
void WallTiles::AddTileSea(const int& card) {
	int temp = card;
	int id = (temp / 100 - 1) * 9 + (temp / 10 % 10 - 1);
	if (appearTileNum[id] < 4) {
		appearTileNum[id]++;
	}
	else {
		printf("Add TileSea Error!, Card id: %d\n", id);
		fflush(stdout);
		throw std::out_of_range("Add TileSea Error : appear times is over 4");
	}
	tileSea.push_back(temp);
}

// delete tile sea
void WallTiles::deleteTileSea() {
	int card = tileSea.back();
	int id = (card / 100 - 1) * 9 + (card / 10 % 10 - 1);
	if (appearTileNum[id] > 0) {
		appearTileNum[id]--;
	}
	else {
		printf("Delete TileSea Error!, Card id: %d\n", id);
		fflush(stdout);
		throw std::out_of_range("Delete TileSea Error : appear times is 0");
	}	
	tileSea.pop_back();
}

void WallTiles::ShowTileSea() const {
	for (int i = 0; i < int(tileSea.size()); i++) {
		printf("%d ", tileSea[i]);
	}
	printf("\n");
}

void WallTiles::ShowTileSeaName() const {
	for (int i = 0; i < int(tileSea.size()); i++) {
		printf("%d%c ", tileSea[i] % 9 + 1, tileType[tileSea[i] / 9]);
	}
	printf("\n");
}