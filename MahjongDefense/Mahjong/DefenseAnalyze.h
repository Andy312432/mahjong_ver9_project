#pragma once
#ifndef __DEFENSE_ANALYZER_H__
#define __DEFENSE_ANALYZER_H__

#include <vector>
#include "PublicHand.h"
#include "WallTiles.h"  

class DefenseAnalyze {
public:
    static bool shouldDefend(int myPosition, const PublicHand* publicHand);
    static bool shouldDefend(int myPosition, const PublicHand* publicHand, const WallTiles& wallTiles);

    static double getProbilityByLinear(int myPosition, const WallTiles& wallTiles, const PublicHand* publicHand);
    static int getOtherPlayerEatPongCount(int myPosition, const PublicHand* publicHand);
    static double getMiddleTileRatio(const PublicHand& publicHand);

    static std::vector<int> getSafeTiles(const int* throwList, int throwArraySize, const WallTiles& wallTiles);
};

#endif
