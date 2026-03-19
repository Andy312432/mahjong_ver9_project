#pragma once
#ifndef __DEFENSE_ANALYZER_H__
#define __DEFENSE_ANALYZER_H__

#include <vector>
#include "Player.h"     
#include "WallTiles.h"  
#include "PrivateHand.h" 

class DefenseAnalyze {
public:
    static bool shouldDefend(int myPosition, const PublicHand* publicHand);

    static std::vector<int> getSafeTiles(const int* throwList, int throwArraySize, const WallTiles& wallTiles);
};

#endif