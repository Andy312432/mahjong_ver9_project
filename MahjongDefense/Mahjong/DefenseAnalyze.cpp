#include "DefenseAnalyze.h"

bool DefenseAnalyze::shouldDefend(int myPosition, const PublicHand* publicHand) {
    // 遍歷包含自己在內的 4 位玩家
    for (int i = 0; i < 4; ++i) {
        if (i == myPosition) continue;

        // 如果你覺得 2 組太早防守，可以把這裡改成 >= 3
        if (publicHand[i].getGroupNum() >= 2) {
            return true;
        }
    }
    return false;
}

std::vector<int> DefenseAnalyze::getSafeTiles(const int* throwList, int throwArraySize, const WallTiles& wallTiles) {

    std::vector<int> safeTiles;
    std::vector<int> sea = wallTiles.getAllTileSea();

    for (int i = 0; i < throwArraySize; ++i) {
        int candidate = throwList[i];

        bool isSafe = false;
        for (int seaTile : sea) {
            int seaTileId = (seaTile / 100 - 1) * 9 + (seaTile / 10 % 10 - 1);
            if (candidate == seaTileId) {
                isSafe = true;
                break;
            }
        }

        if (isSafe) {
            safeTiles.push_back(candidate);
        }
    }

    return safeTiles;
}