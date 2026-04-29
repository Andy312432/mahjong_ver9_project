#include "DefenseAnalyze.h"

namespace {
    constexpr double kDefendThreshold = 0.55;

    int cardIdToTileIndex(int cardId) {
        return (cardId / 100 - 1) * 9 + (cardId / 10 % 10 - 1);
    }

    bool isValidTileIndex(int tileIndex) {
        return tileIndex >= 0 && tileIndex < 34;
    }

    bool isMiddleTile(int tileIndex) {
        return tileIndex < 27 && tileIndex % 9 >= 1 && tileIndex % 9 <= 7;
    }

    double clampProbability(double value) {
        if (value < 0.0) return 0.0;
        if (value > 1.0) return 1.0;
        return value;
    }

    double minDouble(double left, double right) {
        return left < right ? left : right;
    }
}

bool DefenseAnalyze::shouldDefend(int myPosition, const PublicHand* publicHand) {
    for (int playerIndex = 0; playerIndex < 4; ++playerIndex) {
        if (playerIndex == myPosition) continue;

        if (publicHand[playerIndex].getGroupNum() >= 2) {
            return true;
        }
    }
    return false;
}

bool DefenseAnalyze::shouldDefend(int myPosition, const PublicHand* publicHand, const WallTiles& wallTiles) {
    return getProbilityByLinear(myPosition, wallTiles, publicHand) >= kDefendThreshold;
}

double DefenseAnalyze::getProbilityByLinear(int myPosition, const WallTiles& wallTiles, const PublicHand* publicHand) {
    const int roundCount = wallTiles.getAllTileSeaCount() / 4;
    const double roundScore = minDouble(roundCount / 12.0, 1.0) * 0.20;

    double maxProbability = 0.0;
    for (int playerIndex = 0; playerIndex < 4; ++playerIndex) {
        if (playerIndex == myPosition) continue;

        const int eatPongCount = publicHand[playerIndex].getEatPongGroupNum();
        const double eatPongScore = minDouble(eatPongCount / 3.0, 1.0) * 0.50;
        const double middleTileScore = getMiddleTileRatio(publicHand[playerIndex]) * 0.20;
        const double probability = clampProbability(0.15 + roundScore + eatPongScore + middleTileScore);

        if (probability > maxProbability) {
            maxProbability = probability;
        }
    }

    return maxProbability;
}

int DefenseAnalyze::getOtherPlayerEatPongCount(int myPosition, const PublicHand* publicHand) {
    int count = 0;
    for (int playerIndex = 0; playerIndex < 4; ++playerIndex) {
        if (playerIndex == myPosition) continue;
        count += publicHand[playerIndex].getEatPongGroupNum();
    }
    return count;
}

double DefenseAnalyze::getMiddleTileRatio(const PublicHand& publicHand) {
    int middleTileCount = 0;
    int totalTileCount = 0;
    const std::vector<std::vector<int>> publicGroups = publicHand.getTiles();

    for (const std::vector<int>& group : publicGroups) {
        for (int cardId : group) {
            const int tileIndex = cardIdToTileIndex(cardId);
            if (!isValidTileIndex(tileIndex)) continue;

            totalTileCount++;
            if (isMiddleTile(tileIndex)) {
                middleTileCount++;
            }
        }
    }

    if (totalTileCount == 0) {
        return 0.0;
    }
    return (double)middleTileCount / totalTileCount;
}

std::vector<int> DefenseAnalyze::getSafeTiles(const int* throwList, int throwArraySize, const WallTiles& wallTiles) {
    std::vector<int> safeTiles;
    std::vector<int> sea = wallTiles.getAllTileSea();

    for (int throwIndex = 0; throwIndex < throwArraySize; ++throwIndex) {
        int candidate = throwList[throwIndex];

        bool isSafe = false;
        for (int seaTile : sea) {
            int seaTileId = cardIdToTileIndex(seaTile);
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
