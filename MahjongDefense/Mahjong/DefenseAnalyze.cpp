#include "DefenseAnalyze.h"

#include <array>

namespace {
    constexpr int kMeldModelCount = 5;
    constexpr int kFeatureCount = 17;

    // Defense starts when the linear model output is above this value.
    // Tune this threshold after checking real-game decisions.
    constexpr double kDefendThreshold = 0.55;

    // Feature order:
    // a round,
    // c suit concentration,
    // d tsumogiri ratio,
    // e consecutive tsumogiri strength,
    // f tsumogiri-to-tedashi,
    // g-j middle tile first to fourth discarded,
    // k-n honor tile first to fourth discarded,
    // o-r edge tile first to fourth discarded.
    //
    // The Python script trains one LinearRegression model for each meld_count 0..4.
    // Paste the generated WEIGHTS_BY_MELD, MEANS_BY_MELD, SCALES_BY_MELD,
    // and INTERCEPTS_BY_MELD values here after training.
    using FeatureArray = std::array<double, kFeatureCount>;
    using ModelArray = std::array<FeatureArray, kMeldModelCount>;

    constexpr ModelArray kFeatureMeansByMeld = { {
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
    } };

    constexpr ModelArray kFeatureScalesByMeld = { {
        { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
    } };

    constexpr ModelArray kFeatureWeightsByMeld = { {
        { 0.02, 0.01, 0.01, 0.01, 0.00, 0.00, 0.01, 0.00, 0.00, 0.00, 0.00, 0.00, 0.01, 0.00, 0.00, 0.00, 0.00 },
        { 0.13, 0.05, 0.00, 0.00, 0.00, 0.03, 0.04, 0.05, 0.06, 0.02, 0.03, 0.04, 0.05, 0.03, 0.04, 0.05, 0.06 },
        { 0.04, 0.06, 0.00, 0.00, 0.00, 0.04, 0.05, 0.06, 0.07, 0.03, 0.04, 0.05, 0.06, 0.04, 0.05, 0.06, 0.07 },
        { 0.05, 0.07, 0.00, 0.00, 0.00, 0.05, 0.06, 0.07, 0.08, 0.04, 0.05, 0.06, 0.07, 0.05, 0.06, 0.07, 0.08 },
        { 0.06, 0.08, 0.00, 0.00, 0.00, 0.06, 0.07, 0.08, 0.09, 0.05, 0.06, 0.07, 0.08, 0.06, 0.07, 0.08, 0.09 }
    } };

    constexpr std::array<double, kMeldModelCount> kModelInterceptsByMeld = {
        0.02, 0.08, 0.16, 0.28, 0.40
    };

    // 牌ID轉牌種索引
    int cardIdToTileIndex(int cardId) {
        return (cardId / 100 - 1) * 9 + (cardId / 10 % 10 - 1);
    }

    // 檢查牌種索引是否合法
    bool isValidTileIndex(int tileIndex) {
        return tileIndex >= 0 && tileIndex < 34;
    }

    // 判斷是否為中張
    bool isMiddleTile(int tileIndex) {
        return tileIndex < 27 && tileIndex % 9 >= 2 && tileIndex % 9 <= 6;
    }

    // 判斷是否為字牌
    bool isHonorTile(int tileIndex) {
        return tileIndex >= 27 && tileIndex < 34;
    }

    // 限制機率範圍
    double clampProbability(double value) {
        if (value < 0.0) return 0.0;
        if (value > 1.0) return 1.0;
        return value;
    }

    // 限制副露數到模型範圍
    int clampMeldCount(int meldCount) {
        if (meldCount < 0) return 0;
        if (meldCount >= kMeldModelCount) return kMeldModelCount - 1;
        return meldCount;
    }

    // 特徵標準化
    double standardizeFeature(double value, int featureIndex, int meldCount) {
        const double scale = kFeatureScalesByMeld[meldCount][featureIndex];
        if (scale == 0.0) return 0.0;
        return (value - kFeatureMeansByMeld[meldCount][featureIndex]) / scale;
    }

    // 線性模型預測聽牌機率
    double predictTenpaiProbability(const FeatureArray& features, int meldCount) {
        double prediction = kModelInterceptsByMeld[meldCount];
        for (int featureIndex = 0; featureIndex < kFeatureCount; ++featureIndex) {
            prediction += kFeatureWeightsByMeld[meldCount][featureIndex] *
                standardizeFeature(features[featureIndex], featureIndex, meldCount);
        }
        return clampProbability(prediction);
    }

    // 尋找指定玩家最後一筆棄牌紀錄
    int findLatestDiscardRecordIndex(const std::vector<DiscardRecord>& records, int playerIndex) {
        int latestRecordIndex = -1;
        for (int recordIndex = 0; recordIndex < (int)records.size(); ++recordIndex) {
            if (records[recordIndex].playerIndex == playerIndex) {
                latestRecordIndex = recordIndex;
            }
        }
        return latestRecordIndex;
    }

    // 計算最新棄牌是牌河中的第幾張同種牌
    int getLatestDiscardCopyOrder(const std::vector<DiscardRecord>& records, int latestRecordIndex) {
        if (latestRecordIndex < 0) {
            return 0;
        }

        const int latestTileIndex = cardIdToTileIndex(records[latestRecordIndex].card);
        if (!isValidTileIndex(latestTileIndex)) {
            return 0;
        }

        int copyOrder = 0;
        for (int recordIndex = 0; recordIndex <= latestRecordIndex; ++recordIndex) {
            if (cardIdToTileIndex(records[recordIndex].card) == latestTileIndex) {
                copyOrder++;
            }
        }

        if (copyOrder < 1) return 0;
        if (copyOrder > 4) return 4;
        return copyOrder;
    }

    // 加入最新棄牌的第幾張分類特徵
    void addLatestDiscardOrderFeatures(FeatureArray& features, const WallTiles& wallTiles, int playerIndex) {
        const std::vector<DiscardRecord> records = wallTiles.getAllDiscardRecords();
        const int latestRecordIndex = findLatestDiscardRecordIndex(records, playerIndex);
        if (latestRecordIndex < 0) {
            return;
        }

        const int tileIndex = cardIdToTileIndex(records[latestRecordIndex].card);
        const int copyOrder = getLatestDiscardCopyOrder(records, latestRecordIndex);
        if (!isValidTileIndex(tileIndex) || copyOrder < 1 || copyOrder > 4) {
            return;
        }

        if (isMiddleTile(tileIndex)) {
            features[4 + copyOrder] = 1.0;
        }
        else if (isHonorTile(tileIndex)) {
            features[8 + copyOrder] = 1.0;
        }
        else {
            features[12 + copyOrder] = 1.0;
        }
    }
}


// 是否進入防守模式（線性模型規則）
bool DefenseAnalyze::shouldDefend(int myPosition, const PublicHand* publicHand, const WallTiles& wallTiles) {
    return getProbilityByLinear(myPosition, wallTiles, publicHand) >= kDefendThreshold;
}

// 線性模型預測其他玩家最大聽牌機率
double DefenseAnalyze::getProbilityByLinear(int myPosition, const WallTiles& wallTiles, const PublicHand* publicHand) {
    double maxProbability = 0.0;
    for (int playerIndex = 0; playerIndex < 4; ++playerIndex) {
        if (playerIndex == myPosition) continue;

        int playerRoundCount = wallTiles.getPlayerTileSeaCount(playerIndex);
        if (playerRoundCount == 0) {
            playerRoundCount = wallTiles.getAllTileSeaCount() / 4;
        }

        const int meldCount = clampMeldCount(publicHand[playerIndex].getEatPongGroupNum());
        FeatureArray features = {};
        features[0] = (double)playerRoundCount;
        features[1] = getSuitConcentration(wallTiles, playerIndex);
        features[2] = getTsumogiriRatio(wallTiles, playerIndex);
        features[3] = getConsecutiveTsumogiriStrength(wallTiles, playerIndex);
        features[4] = getTsumogiriToTedashiRatio(wallTiles, playerIndex);
        addLatestDiscardOrderFeatures(features, wallTiles, playerIndex);

        const double probability = predictTenpaiProbability(features, meldCount);
        if (probability > maxProbability) {
            maxProbability = probability;
        }
    }
    std::cout<<"Prob:" << maxProbability;
    return maxProbability;
}

// 其他玩家吃碰副露總數
int DefenseAnalyze::getOtherPlayerEatPongCount(int myPosition, const PublicHand* publicHand) {
    int count = 0;
    for (int playerIndex = 0; playerIndex < 4; ++playerIndex) {
        if (playerIndex == myPosition) continue;
        count += publicHand[playerIndex].getEatPongGroupNum();
    }
    return count;
}

// 公開副露中的中張比例
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

// 指定玩家牌河中的中張比例
double DefenseAnalyze::getMiddleTileRatio(const WallTiles& wallTiles, int playerIndex) {
    int middleTileCount = 0;
    int totalTileCount = 0;
    const std::vector<int> discards = wallTiles.getPlayerTileSea(playerIndex);

    for (int cardId : discards) {
        const int tileIndex = cardIdToTileIndex(cardId);
        if (!isValidTileIndex(tileIndex)) continue;

        totalTileCount++;
        if (isMiddleTile(tileIndex)) {
            middleTileCount++;
        }
    }

    if (totalTileCount == 0) {
        return 0.0;
    }
    return (double)middleTileCount / totalTileCount;
}

// 公開副露中的花色分散度
double DefenseAnalyze::getSuitConcentration(const PublicHand& publicHand) {
    int suitCounts[3] = { 0 };
    int suitedTileCount = 0;
    const std::vector<std::vector<int>> publicGroups = publicHand.getTiles();

    for (const std::vector<int>& group : publicGroups) {
        for (int cardId : group) {
            const int tileIndex = cardIdToTileIndex(cardId);
            if (!isValidTileIndex(tileIndex) || tileIndex >= 27) continue;

            suitCounts[tileIndex / 9]++;
            suitedTileCount++;
        }
    }

    if (suitedTileCount == 0) {
        return 0.0;
    }

    int maxSuitCount = suitCounts[0];
    for (int suitIndex = 1; suitIndex < 3; ++suitIndex) {
        if (suitCounts[suitIndex] > maxSuitCount) {
            maxSuitCount = suitCounts[suitIndex];
        }
    }
    return 1.0 - (double)maxSuitCount / suitedTileCount;
}

// 指定玩家牌河中的花色分散度
double DefenseAnalyze::getSuitConcentration(const WallTiles& wallTiles, int playerIndex) {
    int suitCounts[3] = { 0 };
    int suitedTileCount = 0;
    const std::vector<int> discards = wallTiles.getPlayerTileSea(playerIndex);

    for (int cardId : discards) {
        const int tileIndex = cardIdToTileIndex(cardId);
        if (!isValidTileIndex(tileIndex) || tileIndex >= 27) continue;

        suitCounts[tileIndex / 9]++;
        suitedTileCount++;
    }

    if (suitedTileCount == 0) {
        return 0.0;
    }

    int maxSuitCount = suitCounts[0];
    for (int suitIndex = 1; suitIndex < 3; ++suitIndex) {
        if (suitCounts[suitIndex] > maxSuitCount) {
            maxSuitCount = suitCounts[suitIndex];
        }
    }
    return 1.0 - (double)maxSuitCount / suitedTileCount;
}

// 公開副露中的字牌比例
double DefenseAnalyze::getHonorTileRatio(const PublicHand& publicHand) {
    int honorTileCount = 0;
    int totalTileCount = 0;
    const std::vector<std::vector<int>> publicGroups = publicHand.getTiles();

    for (const std::vector<int>& group : publicGroups) {
        for (int cardId : group) {
            const int tileIndex = cardIdToTileIndex(cardId);
            if (!isValidTileIndex(tileIndex)) continue;

            totalTileCount++;
            if (isHonorTile(tileIndex)) {
                honorTileCount++;
            }
        }
    }

    if (totalTileCount == 0) {
        return 0.0;
    }
    return (double)honorTileCount / totalTileCount;
}

// 指定玩家牌河中的字牌比例
double DefenseAnalyze::getHonorTileRatio(const WallTiles& wallTiles, int playerIndex) {
    int honorTileCount = 0;
    int totalTileCount = 0;
    const std::vector<int> discards = wallTiles.getPlayerTileSea(playerIndex);

    for (int cardId : discards) {
        const int tileIndex = cardIdToTileIndex(cardId);
        if (!isValidTileIndex(tileIndex)) continue;

        totalTileCount++;
        if (isHonorTile(tileIndex)) {
            honorTileCount++;
        }
    }

    if (totalTileCount == 0) {
        return 0.0;
    }
    return (double)honorTileCount / totalTileCount;
}

// 指定玩家摸切比例
double DefenseAnalyze::getTsumogiriRatio(const WallTiles& wallTiles, int playerIndex) {
    const std::vector<DiscardRecord> records = wallTiles.getPlayerDiscardRecords(playerIndex);
    if (records.empty()) {
        return 0.0;
    }

    int tsumogiriCount = 0;
    for (const DiscardRecord& record : records) {
        if (record.isTsumogiri) {
            tsumogiriCount++;
        }
    }
    return (double)tsumogiriCount / records.size();
}

// 指定玩家最大連續摸切強度
double DefenseAnalyze::getConsecutiveTsumogiriStrength(const WallTiles& wallTiles, int playerIndex) {
    const std::vector<DiscardRecord> records = wallTiles.getPlayerDiscardRecords(playerIndex);
    if (records.empty()) {
        return 0.0;
    }

    int currentStreak = 0;
    int maxStreak = 0;
    for (const DiscardRecord& record : records) {
        if (record.isTsumogiri) {
            currentStreak++;
            if (currentStreak > maxStreak) {
                maxStreak = currentStreak;
            }
        }
        else {
            currentStreak = 0;
        }
    }
    return (double)maxStreak / records.size();
}

// 指定玩家摸切後轉手切比例
double DefenseAnalyze::getTsumogiriToTedashiRatio(const WallTiles& wallTiles, int playerIndex) {
    const std::vector<DiscardRecord> records = wallTiles.getPlayerDiscardRecords(playerIndex);
    if (records.size() < 2) {
        return 0.0;
    }

    int transitionCount = 0;
    for (int recordIndex = 1; recordIndex < (int)records.size(); ++recordIndex) {
        if (records[recordIndex - 1].isTsumogiri && !records[recordIndex].isTsumogiri) {
            transitionCount++;
        }
    }
    return (double)transitionCount / records.size();
}

// 取得候選牌中已被打過的安全牌
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
