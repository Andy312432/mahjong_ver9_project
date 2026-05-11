#pragma once
#ifndef __DEFENSE_ANALYZER_H__
#define __DEFENSE_ANALYZER_H__

#include <vector>
#include "PublicHand.h"
#include "WallTiles.h"  

class DefenseAnalyze {
public:
    // 是否進入防守模式（舊版副露數規則）
    static bool shouldDefend(int myPosition, const PublicHand* publicHand);

    // 是否進入防守模式（線性模型規則）
    static bool shouldDefend(int myPosition, const PublicHand* publicHand, const WallTiles& wallTiles);

    // 線性模型預測其他玩家最大聽牌機率
    static double getProbilityByLinear(int myPosition, const WallTiles& wallTiles, const PublicHand* publicHand);

    // 其他玩家吃碰副露總數
    static int getOtherPlayerEatPongCount(int myPosition, const PublicHand* publicHand);

    // 公開副露中的中張比例
    static double getMiddleTileRatio(const PublicHand& publicHand);

    // 指定玩家牌河中的中張比例
    static double getMiddleTileRatio(const WallTiles& wallTiles, int playerIndex);

    // 公開副露中的花色分散度
    static double getSuitConcentration(const PublicHand& publicHand);

    // 指定玩家牌河中的花色分散度
    static double getSuitConcentration(const WallTiles& wallTiles, int playerIndex);

    // 公開副露中的字牌比例
    static double getHonorTileRatio(const PublicHand& publicHand);

    // 指定玩家牌河中的字牌比例
    static double getHonorTileRatio(const WallTiles& wallTiles, int playerIndex);

    // 指定玩家摸切比例
    static double getTsumogiriRatio(const WallTiles& wallTiles, int playerIndex);

    // 指定玩家最大連續摸切強度
    static double getConsecutiveTsumogiriStrength(const WallTiles& wallTiles, int playerIndex);

    // 指定玩家摸切後轉手切比例
    static double getTsumogiriToTedashiRatio(const WallTiles& wallTiles, int playerIndex);

    // 取得候選牌中已被打過的安全牌
    static std::vector<int> getSafeTiles(const int* throwList, int throwArraySize, const WallTiles& wallTiles);
};

#endif
