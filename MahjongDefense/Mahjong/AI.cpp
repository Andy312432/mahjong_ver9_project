//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Create MCTS AI and random AI to simulate mahjong players.
//
//=============================================================================

#include <stdexcept>
#include <chrono>
#include <thread>
#include <random>
#include <assert.h>

#include "AI.h"
#include "TT.h" // Modified

#ifdef __GNUC__
#include "pcg_random.hpp"
#endif

#define EAT_PONG_ENABLE 1
#define SMART_AI_ENABLE 0 // Modified(original value: 0)
#define USE_THREAD_POOL 1
#define MING_GONG_LEGAL 1
#define CLOSE_MCS 0
#define DISMANTLING_MODE 0 // 0: dismantling, 1: follow listenTable to find
#define USED_MCTS 0 // 1: MCTS
#define TIME_LIMIT 0
#define LIMIT_TIME 27000 // 2850 // Modified(original: 3500)

// 0 :if condition meet min tileNum or min the holeNum put in the throwArray.
// 1 :add the tileNum & the holeNum, find the minimal.
// 2 :first check the tileNum, second check the holeNum.
#define ANALYSIS_MODE 2

#define STDIN 1
#define SHOW_TIME 0
#define SHOW_INFORMATION 1
#define SHOW_WIN_RATE 1
#define SHOW_THROW_LIST 1
#define SHOW_DISMANTLING_RESULT 1

#define LOG_TIME_AND_TREESIZE
#define LOG_COLLISION // Modified


struct NODE {
	double averageScore;
	short int throwTile;
	short int drawnTile;
	short int depth;
	int totalSimTimes;
	bool isTerminateNode;
	short int parentIndex;
	short int childrenNum;
	short int childIndex[578];
} nodes[SHRT_MAX];

AI::AI(const int& ID, const vector<int>& cards, Table* table) : Player(ID, cards) {
	this->mode = 0;
	this->table = table;
	this->board.myPosition = ID - 1;
	for (auto& cardID : cards) {
		int tile = (cardID / 100 - 1) * 9 + (cardID / 10 % 10 - 1);
		board.predictRemainTiles.AddCard(tile);
		board.privateHand.AddHand(tile, hashKey); // Modified
	}
	vector<int> throwTiles;
	MoveThrow(throwTiles);

#if !STDIN
	ShowGameInformation(cards);
#endif // !STDIN
}

AI::AI(const int& ID, const vector<int>& cards) : Player(ID, cards) {
	this->mode = 2;
	this->board.myPosition = ID - 1;

#if !STDIN
	ShowGameInformation(cards);
#endif // !STDIN
}

/// 可以吃嗎
constexpr bool AI::isCanEat(int* tileArray, int* tileNumArray, const int& length, const int& seaCard) {
	/// 別的玩打出的牌如果不合法或是是字牌就不能吃
	if (seaCard == -1 || seaCard / 9 == 3) {
		return false;
	}
	for (int i = 0; i < length; ++i) {
		/// 可以吃的狀況
		if (tileArray[i] / 9 != 3 && ((i + 1 < length && tileArray[i] == seaCard + 1 && tileArray[i + 1] == seaCard + 2 && seaCard / 9 == tileArray[i + 1] / 9) ||
			(i - 1 >= 0 && tileArray[i] == seaCard + 1 && tileArray[i - 1] == seaCard - 1 && seaCard / 9 == tileArray[i] / 9 && seaCard / 9 == tileArray[i - 2] / 9) ||
			(i - 2 >= 0 && tileArray[i] == seaCard + 1 && tileArray[i - 2] == seaCard - 1 && seaCard / 9 == tileArray[i] / 9 && seaCard / 9 == tileArray[i - 2] / 9) ||
			(i + 1 < length && tileArray[i] == seaCard - 2 && tileArray[i + 1] == seaCard - 1 && seaCard / 9 == tileArray[i] / 9))) {
			return true;
		}
	}
	/// 雖然非字牌但是不能吃
	return false;
}

/// 可以碰嗎
constexpr bool AI::isCanPong(int* tileArray, int* tileNumArray, const int& length, const int& card) {
	if (card == -1) {
		return false;
	}
	for (int i = 0; i < length; i++) {
		/// 可以碰的狀況
		if (tileArray[i] == card && tileNumArray[i] >= 2) {
			return true;
		}
	}
	/// 沒有牌可以碰seaCard
	return false;
}

/// 可以槓嗎
constexpr bool AI::isCanGong(int* tileArray, int* tileNumArray, const int& length, const int& card) {
	if (card == -1) {
		return false;
	}
	for (int i = 0; i < length; i++) {
		if (tileArray[i] == card && tileNumArray[i] == 3) {
			return true;
		}
	}
	return false;
}

/// 該盤面是否為會使遊戲結束，勝負已定return true；否則回傳false
constexpr bool AI::is_terminal(const Board& b) {
	switch (b.status) {
	case OneWin:
	case TwoWin:
	case ThreeWin:
	case FourWin:
	case Draw:
		return true;
	default:
		return false;
	}
}

/// 胡了嗎
bool AI::isHu(Board& b, int* tileArray, int* tileNumArray, const int& length, const int& group) {
	/// 查進聽數的表
	b.listenNum = table->getTilesListenNum(5 - group, tileArray, tileNumArray, length, b.is_have_eyes);
	/// 進聽數為-1表示胡牌
	return b.listenNum == -1;
}

bool AI::isHu(Board& b, const int& group) {
	b.listenNum = table->getTilesListenNum(5 - group, b.tableID, b.is_have_eyes);
	return b.listenNum == -1;
}

inline bool AI::isTimeUp() {
#if (TIME_LIMIT == 1)
	timer.stop();
	return timer.ms() >= LIMIT_TIME;
#else
	return false;
#endif
}

constexpr int AI::whos_turn(const Board& b) {
	switch (b.status) {
	case OnePlay:
		return PLAYER1;
	case TwoPlay:
		return PLAYER2;
	case ThreePlay:
		return PLAYER3;
	case FourPlay:
		return PLAYER4;
	default:
		return NONE;
	}
}

constexpr int AI::who_won(const Board& b) {
	switch (b.status) {
	case OneWin:
		return PLAYER1;
	case TwoWin:
		return PLAYER2;
	case ThreeWin:
		return PLAYER3;
	case FourWin:
		return PLAYER4;
	default:
		return NONE;
	}
}

/// 將要捨的牌種寫入throwArray，回傳捨牌集合長度
inline int AI::ThrowList(Board& b, int* throwArray) {
	// action: take-> 0、eat-> 1、pong -> 2、gong ->3、 throw -> 4
	// ===== Search Dismantling Table ==========
	/// Dismantling table是把牌拆成一部分一部分的表
#if (DISMANTLING_MODE == 0)
	if (b.listenNum == 0) { /// 聽牌階段
		int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
		/// length是暗牌種數, throwSize是捨牌集合的長度
		int length = b.privateHand.getTileNumArray(tempArray, tempNumArray), throwSize = 0;
		bool isEyes = false;
		for (int i = 0; i < length; i++) {
			table->UpdateTableID(b.tableID, tempArray[i], THROW); /// 試捨
			if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
				throwArray[throwSize++] = tempArray[i]; /// 找出捨牌後不會增加進聽數的牌
			}
			table->UpdateTableID(b.tableID, tempArray[i], TAKE); /// 回覆試捨操作
		}

		return throwSize;
	}
	ThrowSet newDismantlingResult[250];
	int startPosition[90] = { 0 }, throwContentType[90][4] = { 0 };
	int startPositionSize = table->getDismantling(b.tableID, b.is_have_eyes, newDismantlingResult, startPosition, throwContentType);
#if (SHOW_DISMANTLING_RESULT && !STDIN)
	//board.privateHand.ShowPrivateHand();
	printf("newDismantlingResult\n");
	for (int i = 0; i < startPositionSize; i++) {
		for (int j = startPosition[i], g = 0; j < startPosition[i + 1]; j++, g++) {
			for (int a = 0; a < newDismantlingResult[j].oneSize; a++) {
				printf("%d, ", newDismantlingResult[j].one[a] + throwContentType[i][g] * 9);
			}
			for (int a = 0; a < newDismantlingResult[j].pairSize; a++) {
				printf("%d %d, ", newDismantlingResult[j].pair[a] + throwContentType[i][g] * 9, newDismantlingResult[j].pair[a] + throwContentType[i][g] * 9);
			}
			for (int a = 0; a < newDismantlingResult[j].tatsuSize << 1; a += 2) {
				printf("%d %d, ", newDismantlingResult[j].tatsu[a] + throwContentType[i][g] * 9, newDismantlingResult[j].tatsu[a + 1] + throwContentType[i][g] * 9);
			}
		}
		printf("\n");
	}
#endif
#elif (DISMANTLING_MODE == 1)
	int maxValidCard = 0;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	int originListenNum = table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, b.listenNumTable, b.listenRecordGroup, b.is_have_eyes, b.takeTile, b.throwTile);
	Board a(b);
	vector<int> throwArray;
	for (int i = 0; i < length; i++) {
		table->UpdateTableID(b.tableID, tempArray[i], THROW);
		int currentListenNum = table->getTilesListenNum(5 - a.publicGroupNum, a.tableID, a.listenNumTable, a.listenRecordGroup, a.is_have_eyes, tempArray[i], tempArray[i]);
		if (currentListenNum <= originListenNum) {
			throwArray.push_back(tempArray[i]);
		}
		table->UpdateTableID(b.tableID, tempArray[i], TAKE);
	}
	//vector<int> throwArray = table->getListenDismantling(b.tableID, b.listenRecordGroup, b.is_have_eyes, -1, b.throwTile);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//printf("Spend Time: %lld\n", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()); //35500	
	/*for (unsigned i = 0; i < throwArray.size(); i++)
	{
		printf("%d ", throwArray[i]);
	}*/
#endif

	// ===== Throw Tiles Analysis for Dismantling ==========
#if(DISMANTLING_MODE == 0)
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	return ThrowTilesAnalysis(b, newDismantlingResult, startPosition, throwArray, throwContentType, startPositionSize);
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//printf("Spend Time: %lld\n", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()); //27900，26600，23100，32300->24475
#elif (DISMANTLING_MODE == 1)
	return ThrowTilesAnalysis(b, throwArray);
#endif
}

int AI::ThrowTilesAnalysis(Board& b, ThrowSet* newDismantlingResult, int* startPosition, int* throwTiles, int(*throwContentType)[4], const int& startPositionSize) {
	char pairListenTilesNum[90][8] = { 0 }, pairListenHoleNum[90][8] = { 0 }, tatsuListenTilesNum[90][16] = { 0 }, tatsuListenHoleNum[90][16] = { 0 }
	, oneListenTilesNum[90][17] = { 0 }, totalListenTilesNum[90] = { 0 }, totalListenHoleNum[90] = { 0 }, minListenSingleTileNum[90];
	char wordTiles[90][17] = { 0 }, eyeTiles[90][8] = { 0 }, tatsuTiles[90][16] = { 0 }, totalSize[90] = { 0 };
	char minListenTileNum(100), maxTotalListenTileNum(-100), maxHoleNum(-100), minListenHoleNum(100), minSize = 125;
	int  wordTileSize[90] = { 0 }, eyeTileSize[90] = { 0 }, tatsuTileSize[90] = { 0 }, pairListenTilesSize[90] = { 0 }, tatsuListenTilesSize[90] = { 0 }, oneListenTilesSize[90] = { 0 }
	, candidate[90], candidateSize = 0, recordIndex = 0, throwArraySize = 0;

	// Initialize
	for (int i = 0; i < 90; i++) {
		minListenSingleTileNum[i] = 100;
	}

	for (int index = 0; index < startPositionSize; ++index) {
		for (int i = startPosition[index], g = 0; i < startPosition[index + 1]; ++i, ++g) {
			for (int j = 0; j < newDismantlingResult[i].oneSize; ++j) {
				oneListenTilesNum[index][oneListenTilesSize[index] + j] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].one[j] + throwContentType[index][g] * 9);
				totalListenTilesNum[index] += oneListenTilesNum[index][oneListenTilesSize[index] + j];
				// find all single character tiles
				if (newDismantlingResult[i].one[j] > 26 && minListenSingleTileNum[index] > oneListenTilesNum[index][oneListenTilesSize[index] + j]) {
					minListenSingleTileNum[index] = oneListenTilesNum[index][oneListenTilesSize[index] + j];
					wordTileSize[index] = 0;
					wordTiles[index][wordTileSize[index]++] = newDismantlingResult[i].one[j];
				}
				else if (newDismantlingResult[i].one[j] > 26 && minListenSingleTileNum[index] == oneListenTilesNum[index][oneListenTilesSize[index] + j]) {
					wordTiles[index][wordTileSize[index]++] = newDismantlingResult[i].one[j];
				}
			}
			oneListenTilesSize[index] += newDismantlingResult[i].oneSize;

			for (int j = 0; j < newDismantlingResult[i].pairSize; ++j) {
				pairListenTilesNum[index][pairListenTilesSize[index] + j] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].pair[j] + throwContentType[index][g] * 9);
				totalListenTilesNum[index] += pairListenTilesNum[index][pairListenTilesSize[index] + j];
				// find all single character tiles
				if (pairListenTilesNum[index][pairListenTilesSize[index] + j] > 1) {
					++pairListenHoleNum[index][pairListenTilesSize[index] + j];
					++totalListenHoleNum[index];
				}
				else {
					eyeTiles[index][eyeTileSize[index]++] = newDismantlingResult[i].pair[j] + throwContentType[index][g] * 9;
				}
			}
			pairListenTilesSize[index] += newDismantlingResult[i].pairSize;

			for (int j = 0; j < newDismantlingResult[i].tatsuSize << 1; j += 2) {
				if (newDismantlingResult[i].tatsu[j] == 0) { // find side 1
					if (newDismantlingResult[i].tatsu[j] + 1 == newDismantlingResult[i].tatsu[j + 1]) {			// side
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 + 2);
						totalListenTilesNum[index] += tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)];
					}
					else if (newDismantlingResult[i].tatsu[j] + 2 == newDismantlingResult[i].tatsu[j + 1]) {	// middle hole
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 + 1);
						totalListenTilesNum[index] += tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)];
					}
					// check listenNum & record listenHole
					if (tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] > 1) {
						++tatsuListenHoleNum[index][tatsuListenTilesSize[index] + (j >> 1)];
						++totalListenHoleNum[index];
					}
					else {
						totalSize[index]++;
						tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9;
						tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9;
					}
				}
				else if (newDismantlingResult[i].tatsu[j + 1] == 8) {
					if (newDismantlingResult[i].tatsu[j] + 1 == newDismantlingResult[i].tatsu[j + 1])			// side
					{
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 - 1);
						totalListenTilesNum[index] += tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)];
					}
					else if (newDismantlingResult[i].tatsu[j] + 2 == newDismantlingResult[i].tatsu[j + 1])	// middle hole
					{
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 + 1);
						totalListenTilesNum[index] += tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)];
					}
					// check listenNum & record listenHole
					if (tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] > 1) {
						++tatsuListenHoleNum[index][tatsuListenTilesSize[index] + (j >> 1)];
						++totalListenHoleNum[index];
					}
					else {
						totalSize[index]++;
						tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9;
						tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9;
					}
				}
				else {
					if (newDismantlingResult[i].tatsu[j] + 1 == newDismantlingResult[i].tatsu[j + 1])			// two hole
					{
						int tempNum1 = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 - 1),
							tempNum2 = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9 + 1);
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = tempNum1 + tempNum2;
						totalListenTilesNum[index] += tempNum1 + tempNum2;
						// check listenNum & record listenHole
						if (tempNum1 > 0) {
							++tatsuListenHoleNum[index][tatsuListenTilesSize[index] + (j >> 1)];
							++totalListenHoleNum[index];
						}
						else {
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9;
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9;
						}
						// check listenNum & record listenHole
						if (tempNum2 > 0) {
							++tatsuListenHoleNum[index][tatsuListenTilesSize[index] + (j >> 1)];
							++totalListenHoleNum[index];
						}
						else if (tempNum1 > 1) {
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9;
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9;
						}
						else {
							totalSize[index]++;
						}
					}
					else if (newDismantlingResult[i].tatsu[j] + 2 == newDismantlingResult[i].tatsu[j + 1])	// middle hole
					{
						tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] = b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9 + 1);
						totalListenTilesNum[index] += tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)];
						// check listenNum & record listenHole
						if (tatsuListenTilesNum[index][tatsuListenTilesSize[index] + (j >> 1)] > 1) {
							++tatsuListenHoleNum[index][tatsuListenTilesSize[index] + (j >> 1)];
							++totalListenHoleNum[index];
						}
						else {
							totalSize[index]++;
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j] + throwContentType[index][g] * 9;
							tatsuTiles[index][tatsuTileSize[index]++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[index][g] * 9;
						}
					}
				}
			}
			tatsuListenTilesSize[index] += newDismantlingResult[i].tatsuSize;
		}
		if (totalListenTilesNum[index] > maxTotalListenTileNum) maxTotalListenTileNum = totalListenTilesNum[index];
		if (totalListenHoleNum[index] > maxHoleNum) maxHoleNum = totalListenHoleNum[index];
	}

	// find max listen hole to decision witch group is the best
	for (int i = 0; i < startPositionSize; ++i) {
		if (totalSize[i] < minSize && (totalListenTilesNum[i] == maxTotalListenTileNum || totalListenHoleNum[i] == maxHoleNum)) {
			candidateSize = 0;
			minSize = totalSize[i];
			candidate[candidateSize++] = i;
		}
		else if (totalSize[i] == minSize && (totalListenTilesNum[i] == maxTotalListenTileNum || totalListenHoleNum[i] == maxHoleNum)) {
			candidate[candidateSize++] = i;
		}
	}

	char singleArray[17] = { 0 };
	int singleArraySize = 0, pairSize = 0, tatsuSize = 0, oneSize = 0;
	if (candidateSize > 1) {
		unordered_map<int, bool> u_map;
		recordIndex = candidate[0];
		for (int i = startPosition[recordIndex], g = 0; i < startPosition[recordIndex + 1]; ++i, ++g) {
			for (int j = 0; j < newDismantlingResult[i].oneSize; ++j) {
				u_map.insert(std::pair(newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9, true));
			}
			oneSize += newDismantlingResult[i].oneSize;
			pairSize += newDismantlingResult[i].pairSize;
			tatsuSize += newDismantlingResult[i].tatsuSize;
		}

		for (int i = 1; i < candidateSize; ++i) {
			for (int j = startPosition[candidate[i]], g = 0; j < startPosition[candidate[i] + 1]; ++j, ++g) {
				for (int a = 0; a < newDismantlingResult[j].oneSize; ++a) {
					if (u_map.find(newDismantlingResult[j].one[a] + throwContentType[candidate[i]][g] * 9) == u_map.end()) {
						singleArray[singleArraySize++] = newDismantlingResult[j].one[a] + throwContentType[candidate[i]][g] * 9;
						u_map.insert(std::pair(newDismantlingResult[j].one[a] + throwContentType[candidate[i]][g] * 9, true));
					}
				}
			}
		}
	}
	else if (candidateSize == 1) {
		recordIndex = candidate[0];
		for (int i = startPosition[recordIndex], g = 0; i < startPosition[recordIndex + 1]; ++i, ++g) {
			oneSize += newDismantlingResult[i].oneSize;
			pairSize += newDismantlingResult[i].pairSize;
			tatsuSize += newDismantlingResult[i].tatsuSize;
		}
	}
	else {
		fprintf(stderr, "minSize: %d\n", minSize);
		for (int i = 0; i < startPositionSize; ++i) {
			fprintf(stderr, "%d ", totalSize[i]);
		}
		fprintf(stderr, "\n");
		fprintf(stderr, "Candidate Empty.\n");
		throw std::out_of_range("Candidate Empty.");
	}

	// Step 1 : Find the tiles that cannot be made a sequence.

		// deal with character
	if (wordTileSize[recordIndex] > 0) {
		for (int i = 0; i < wordTileSize[recordIndex]; ++i) {
			if (wordTiles[recordIndex][i] < 27 || wordTiles[recordIndex][i] > 33) throw std::out_of_range("Out of range.");
			throwTiles[throwArraySize++] = wordTiles[recordIndex][i];
		}
	}

	if (throwArraySize < 0) {
		fprintf(stderr, "throwArraySize out of range.\n");
		throw std::out_of_range("out of range.");
	}

	if (throwArraySize > 0) {
		return throwArraySize;
	}

	// can't listen
	if (eyeTileSize[recordIndex] || tatsuTileSize[recordIndex]) {
		int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
		int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);
		bool isEyes = false;
		for (int i = 0; i < length; ++i) {
			table->UpdateTableID(b.tableID, tempArray[i], THROW);
			if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
				throwTiles[throwArraySize++] = tempArray[i];
			}
			table->UpdateTableID(b.tableID, tempArray[i], TAKE);
		}

		return throwArraySize;
	}

	if (startPosition[recordIndex + 1] - startPosition[recordIndex] > 4) {
		fprintf(stderr, "startPosition : %d\n", startPosition[recordIndex + 1] - startPosition[recordIndex]);
		throw std::out_of_range("Size over 4.");
	}

	// Step 2 : Find the single and side tile. And check if the hand has excessive blocks.

	char sideTile[17] = { 0 }, remainTile[17] = { 0 }, pairGroup[6];
	char meldNum = b.publicGroupNum + pairSize + tatsuSize;
	int tilesNum = b.publicGroupNum * 3 + (pairSize << 1) + (tatsuSize << 1) + oneSize;
	int sideTileSize = 0, remainTileSize = 0, pairGroupSize = 0;

	while (tilesNum < 17) {
		tilesNum += 3;
		meldNum++;
	}

	if (meldNum >= 7) {
		for (int i = startPosition[recordIndex], g = 0; i < startPosition[recordIndex + 1]; i++, g++) {
			for (int j = 0; j < newDismantlingResult[i].oneSize; j++) {
				bool find_3 = false;
				for (int a = 0; a < newDismantlingResult[i].pairSize; a++) {
					if (newDismantlingResult[i].pair[a] < 9 && (newDismantlingResult[i].pair[a] - 1 == newDismantlingResult[i].one[j]
						|| newDismantlingResult[i].pair[a] + 1 == newDismantlingResult[i].one[j]
						|| newDismantlingResult[i].pair[a] - 2 == newDismantlingResult[i].one[j]
						|| newDismantlingResult[i].pair[a] + 2 == newDismantlingResult[i].one[j])) {
						if (b.predictRemainTiles.getRemainTilesNum(newDismantlingResult[i].pair[a] + throwContentType[recordIndex][g] * 9) >= 2) {
							pairGroup[pairGroupSize++] = newDismantlingResult[i].pair[a] + throwContentType[recordIndex][g] * 9;
						}
						else {
							throwTiles[throwArraySize++] = newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9;
						}
						find_3 = true;
						break;
					}
				}
				if (find_3) continue;
				int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
				int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);
				int index = 0;
				for (int i = 0; i < length; i++) {
					if (newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] == tempArray[i]) {
						index = i;
						break;
					}
				}
				if ((tempNumArray[index] == 1 && ((index != 0 && (tempArray[index - 1] + 1 == tempArray[index] || tempArray[index - 1] + 2 == tempArray[index]) && tempNumArray[index - 1] == 2) ||
					(index + 1 != length && (tempArray[index + 1] - 1 == tempArray[index] || tempArray[index + 1] - 2 == tempArray[index]) && tempNumArray[index + 1] == 2))) ||
					(tempNumArray[index] == 2 && ((index != 0 && (tempArray[index - 1] + 1 == tempArray[index] || tempArray[index - 1] + 2 == tempArray[index]) && tempNumArray[index - 1] == 1) ||
						(index + 1 != length && (tempArray[index + 1] - 1 == tempArray[index] || tempArray[index + 1] - 2 == tempArray[index]) && tempNumArray[index + 1] == 1))) ||
					(tempNumArray[index] == 1 && ((index - 2 >= 0 && tempArray[index - 2] + 2 == tempArray[index] && tempNumArray[index - 2] == 2) ||
						(index + 2 != length && tempArray[index + 2] - 2 == tempArray[index] && tempNumArray[index + 2] == 2))) ||
					(tempNumArray[index] == 2 && ((index - 2 >= 0 && tempArray[index - 2] + 2 == tempArray[index] && tempNumArray[index - 2] == 1) ||
						(index + 2 != length && tempArray[index + 2] - 2 == tempArray[index] && tempNumArray[index + 2] == 1)))) {
					continue;
				}
				if (newDismantlingResult[i].one[j] == 0 || newDismantlingResult[i].one[j] == 8) {
					sideTile[sideTileSize++] = newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9;
				}
				else if (newDismantlingResult[i].one[j] > 8 || newDismantlingResult[i].one[j] < 0) {
					fprintf(stderr, "One array over range : %d %d\n", newDismantlingResult[i].one[j], recordIndex);
					throw std::out_of_range("one array over range.");
				}
				else {
					remainTile[remainTileSize++] = newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9;
				}
			}
		}
	}
	else {
		for (int i = startPosition[recordIndex], g = 0; i < startPosition[recordIndex + 1]; ++i, ++g) {
			for (int j = 0; j < newDismantlingResult[i].oneSize; ++j) {
				if (newDismantlingResult[i].one[j] == 0 || newDismantlingResult[i].one[j] == 8) {
					sideTile[sideTileSize++] = newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9;
				}
				else if (newDismantlingResult[i].one[j] > 8 || newDismantlingResult[i].one[j] < 0) {
					fprintf(stderr, "One array over range : %d %d\n", newDismantlingResult[i].one[j], recordIndex);
					throw std::out_of_range("one array over range.");
				}
				else {
					remainTile[remainTileSize++] = newDismantlingResult[i].one[j] + throwContentType[recordIndex][g] * 9;
				}
			}
		}
	}

	if (pairListenTilesSize[recordIndex] > 1) {
		for (int i = 0; i < eyeTileSize[recordIndex]; i++) {
			if (eyeTiles[recordIndex][i] < 0 || eyeTiles[recordIndex][i] > 33) throw std::out_of_range("Out of range.");
			singleArray[singleArraySize++] = eyeTiles[recordIndex][i];
		}
	}

	if (tatsuTileSize[recordIndex] > 0) {
		for (int i = 0; i < tatsuTileSize[recordIndex]; i++) {
			if (tatsuTiles[recordIndex][i] < 0 || tatsuTiles[recordIndex][i] > 26) throw std::out_of_range("Out of range.");
			singleArray[singleArraySize++] = tatsuTiles[recordIndex][i];
		}
	}

	if (singleArraySize > 0) {
		for (int i = 0; i < singleArraySize; i++) {
			if (singleArray[i] % 9 == 0 || singleArray[i] % 9 == 8) {
				sideTile[sideTileSize++] = singleArray[i];
			}
			else if (singleArray[i] > 34 || singleArray[i] < 0) {
				fprintf(stderr, "One array over range : %d\n", singleArray[i]);
				throw std::out_of_range("one array over range.");
			}
			else {
				remainTile[remainTileSize++] = singleArray[i];
			}
		}
	}

	if (sideTileSize > 0) {
		for (int j = 0; j < sideTileSize; j++) {
			throwTiles[j] = sideTile[j];
		}
		bool isEyes = false;
		int checkArray[17], checkArraySize = 0;
		for (int i = 0; i < sideTileSize; i++) {
			table->UpdateTableID(b.tableID, throwTiles[i], THROW);
			if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
				checkArray[checkArraySize++] = throwTiles[i];
			}
			table->UpdateTableID(b.tableID, throwTiles[i], TAKE);
		}
		if (checkArraySize > 0) {
			sideTileSize = 0;
			for (int i = 0; i < checkArraySize; i++) {
				throwTiles[sideTileSize++] = checkArray[i];
			}
		}
		return sideTileSize;
	}
	else if (remainTileSize > 0) {
		for (int j = 0; j < remainTileSize; j++) {
			throwTiles[j] = remainTile[j];
		}
		bool isEyes = false;
		int checkArray[17], checkArraySize = 0;
		for (int i = 0; i < remainTileSize; i++) {
			table->UpdateTableID(b.tableID, throwTiles[i], THROW);
			if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
				checkArray[checkArraySize++] = throwTiles[i];
			}
			table->UpdateTableID(b.tableID, throwTiles[i], TAKE);
		}
		if (checkArraySize > 0) {
			remainTileSize = 0;
			for (int i = 0; i < checkArraySize; i++) {
				throwTiles[remainTileSize++] = checkArray[i];
			}
		}
		return remainTileSize;
	}

	if (meldNum >= 7) {
		for (int i = startPosition[recordIndex], g = 0; i < startPosition[recordIndex + 1]; i++, g++) {
			for (int j = 0; j < newDismantlingResult[i].tatsuSize << 1; j += 2) {
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j] + throwContentType[recordIndex][g] * 9;
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[recordIndex][g] * 9;
			}
			if (pairListenTilesSize[recordIndex] > 1) {
				for (int j = 0; j < newDismantlingResult[i].pairSize; j++) {
					bool find_out = false;
					for (int a = 0; a < pairGroupSize; a++) {
						if (pairGroup[a] == newDismantlingResult[i].pair[j] + throwContentType[recordIndex][g] * 9) {
							find_out = true;
							break;
						}
					}
					if (!find_out  /* && pairListenTilesSize[recordIndex] - pairGroupSize > 1*/) {
						throwTiles[throwArraySize++] = newDismantlingResult[i].pair[j] + throwContentType[recordIndex][g] * 9;
					}
				}
			}
			bool isEyes = false;
			int checkArray[17], checkArraySize = 0;
			for (int i = 0; i < throwArraySize; i++) {
				table->UpdateTableID(b.tableID, throwTiles[i], THROW);
				if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
					checkArray[checkArraySize++] = throwTiles[i];
				}
				table->UpdateTableID(b.tableID, throwTiles[i], TAKE);
			}
			if (checkArraySize > 0) {
				throwArraySize = 0;
				for (int i = 0; i < checkArraySize; i++) {
					throwTiles[throwArraySize++] = checkArray[i];
				}
			}
		}
		if (throwArraySize == 0) {
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);
			bool isEyes = false;
			for (int i = 0; i < length; i++) {
				table->UpdateTableID(b.tableID, tempArray[i], THROW);
				if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
					throwTiles[throwArraySize++] = tempArray[i];
				}
				table->UpdateTableID(b.tableID, tempArray[i], TAKE);
			}
			if (throwArraySize == 0) {
				for (int i = 0; i < length; i++) {
					throwTiles[throwArraySize++] = tempArray[i];
				}
			}
		}
		return throwArraySize;
	}

	if (pairListenTilesSize[recordIndex] == 2 && pairGroupSize < 1) {
		pairListenTilesNum[recordIndex][0] += pairListenTilesNum[recordIndex][1];
		pairListenTilesNum[recordIndex][1] = pairListenTilesNum[recordIndex][0];
		pairListenHoleNum[recordIndex][0] += pairListenHoleNum[recordIndex][1];
		pairListenHoleNum[recordIndex][1] = pairListenHoleNum[recordIndex][0];
	}

	if (pairListenTilesSize[recordIndex] > 1) {
		for (int i = 0; i < pairListenTilesSize[recordIndex]; ++i) {
			if (pairListenTilesNum[recordIndex][i] < minListenTileNum) minListenTileNum = pairListenTilesNum[recordIndex][i];
		}
	}

	for (int i = 0; i < tatsuListenTilesSize[recordIndex]; ++i) {
		if (tatsuListenTilesNum[recordIndex][i] < minListenTileNum) minListenTileNum = tatsuListenTilesNum[recordIndex][i];
	}

	for (int i = startPosition[recordIndex], a = 0, b = 0, g = 0; i < startPosition[recordIndex + 1]; ++i, ++g) {
		if (pairListenTilesSize[recordIndex] > 1) {
			for (int j = 0; j < newDismantlingResult[i].pairSize; ++j, ++a) {
				if (minListenTileNum == pairListenTilesNum[recordIndex][a] && minListenHoleNum > pairListenHoleNum[recordIndex][a]) {
					minListenHoleNum = pairListenHoleNum[recordIndex][a];
					throwArraySize = 0;
					throwTiles[throwArraySize++] = newDismantlingResult[i].pair[j] + throwContentType[recordIndex][g] * 9;
				}
				else if (minListenTileNum == pairListenTilesNum[recordIndex][a] && minListenHoleNum == pairListenHoleNum[recordIndex][a]) {
					throwTiles[throwArraySize++] = newDismantlingResult[i].pair[j] + throwContentType[recordIndex][g] * 9;
				}
			}
		}
		for (int j = 0; j < newDismantlingResult[i].tatsuSize << 1; j += 2, ++b) {
			if (minListenTileNum == tatsuListenTilesNum[recordIndex][b] && minListenHoleNum > tatsuListenHoleNum[recordIndex][b]) {
				minListenHoleNum = tatsuListenHoleNum[recordIndex][b];
				throwArraySize = 0;
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j] + throwContentType[recordIndex][g] * 9;
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[recordIndex][g] * 9;
			}
			else if (minListenTileNum == tatsuListenTilesNum[recordIndex][b] && minListenHoleNum == tatsuListenHoleNum[recordIndex][b]) {
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j] + throwContentType[recordIndex][g] * 9;
				throwTiles[throwArraySize++] = newDismantlingResult[i].tatsu[j + 1] + throwContentType[recordIndex][g] * 9;
			}
		}
	}


	bool isEyes = false;
	int checkArray[17], checkArraySize = 0;
	for (int i = 0; i < throwArraySize; ++i) {
		table->UpdateTableID(b.tableID, throwTiles[i], THROW);
		if (table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes) == b.listenNum) {
			checkArray[checkArraySize++] = throwTiles[i];
		}
		table->UpdateTableID(b.tableID, throwTiles[i], TAKE);
	}
	if (checkArraySize > 0) {
		throwArraySize = 0;
		for (int i = 0; i < checkArraySize; ++i) {
			throwTiles[throwArraySize++] = checkArray[i];
		}
	}

	if (throwArraySize == 0) {
		printf("throwTiles empty Error!\n");
		b.privateHand.ShowPrivateHand();
		printf("\n");
		printf("public Num : %d\n", b.publicGroupNum);
		int tempArray[17] = { 0 };
		int tempNumArray[17] = { 0 };
		int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);

		printf("Is Hu : %d\n", b.listenNum);
		printf("Is Hu : %d\n", isHu(b, tempArray, tempNumArray, length, b.publicGroupNum));
		throw std::out_of_range("throwTiles Empty.");
	}

	return throwArraySize;
}

vector<int> AI::dealEat() {
#if (SHOW_INFORMATION && !STDIN)
	printf("===== Eat =====\n");
#endif // DEBUG
	vector<int> result;
	if (mode == 0) {
		if (board.predictRemainTiles.getRemainTotalTilesNum() - 64 + oppGroup <= 16 && board.listenNum > 0) { /// 流局條件
			return result;
		}
		/// listenTilesNum: 吃牌前的聽牌數(可以湊成組的牌的數量)
		int moveArray[5], tempArray[17] = { 0 }, tempNumArray[17] = { 0 }, listenTilesNum[5] = { 0 }, validTileNum[5] = { 0 };
		int index = 0;
		int length = board.privateHand.getTileNumArray(tempArray, tempNumArray); /// privateHand中有多少牌
		int seaCard = (wallTiles.getTileSea() / 100 - 1) * 9 + (wallTiles.getTileSea() / 10 % 10 - 1); /// Get牌海中最新被捨棄的牌，即為當前被捨的牌

		bool ishaveEyesArray[5] = {};

		vector <vector<int>> move;
		vector<int> candidate;

		/// for每個牌種i
		for (int i = 0; i < length; ++i) {	//Eat
			if (i + 1 < length && tempArray[i] == seaCard + 1 && tempArray[i + 1] == seaCard + 2
				&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // XOO /// 34進2
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i + 1], THROW); /// 試吃 (捨掉湊成順的兩張牌)
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes)); /// 試吃後算進聽數
				if (listenNum < board.listenNum) { /// 如果吃牌後進聽數會減少
					moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
					if (tempArray[i + 1] % 9 != 8) {
						listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(tempArray[i + 1] + 1); /// 原本聽的5
						listenTilesNum[index] += board.predictRemainTiles.getRemainTilesNum(seaCard); /// 原本聽的1
					}
					else {
						listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(seaCard);
					}
					ishaveEyesArray[index] = board.is_have_eyes; /// 如果不吃牌的話是否有眼
					validTileNum[index] = table->getValidTiles(board.tableID, board.predictRemainTiles, board.is_have_eyes);
					index++;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i + 1], TAKE);
			}
			else if (i + 1 < length && tempArray[i] == seaCard - 1 && tempArray[i + 1] == seaCard + 1
				&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OXO
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i + 1], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				if (listenNum < board.listenNum) {
					moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
					listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(seaCard);
					ishaveEyesArray[index] = board.is_have_eyes;
					validTileNum[index] = table->getValidTiles(board.tableID, board.predictRemainTiles, board.is_have_eyes);
					index++;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i + 1], TAKE);
			}
			else if (i + 2 < length && tempArray[i] == seaCard - 1 && tempArray[i + 2] == seaCard + 1
				&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 2] / 9) { // OXO
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i + 2], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				if (listenNum < board.listenNum) {
					moveArray[index] = tempArray[i] << 6 | tempArray[i + 2];
					listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(seaCard);
					ishaveEyesArray[index] = board.is_have_eyes;
					validTileNum[index] = table->getValidTiles(board.tableID, board.predictRemainTiles, board.is_have_eyes);
					index++;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i + 2], TAKE);
			}
			else if (i + 1 < length && tempArray[i] == seaCard - 2 && tempArray[i + 1] == seaCard - 1
				&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OOX
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i + 1], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				if (listenNum < board.listenNum) {
					moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
					if (tempArray[i] % 9 != 0) {
						listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(tempArray[i] - 1);
						listenTilesNum[index] += board.predictRemainTiles.getRemainTilesNum(seaCard);
					}
					else {
						listenTilesNum[index] = board.predictRemainTiles.getRemainTilesNum(seaCard);
					}
					ishaveEyesArray[index] = board.is_have_eyes;
					validTileNum[index] = table->getValidTiles(board.tableID, board.predictRemainTiles, board.is_have_eyes);
					index++;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i + 1], TAKE);
			}
		}
		if (index > 0) {
			//printf("Eat:\n");
			int max = -1, pos = 0;
#if (SHOW_INFORMATION && !STDIN)
			printf("Can Eat: ");
#endif
			for (int a = 0; a < index; ++a) {
				if (!ishaveEyesArray[pos] && ishaveEyesArray[a]) {
					max = validTileNum[a];
					pos = a;
				}
				else if (ishaveEyesArray[pos] == ishaveEyesArray[a] &&
					(max < validTileNum[a] || (max == validTileNum[a] && listenTilesNum[pos] > listenTilesNum[a]))) {
					max = validTileNum[a];
					pos = a;
				}
				//printf("%d %d %d %d\n", moveArray[a], listenTilesNum[a], validTileNum[a], ishaveEyesArray[a]);
			}

			for (int a = 0; a < index; ++a) {
				if (max == validTileNum[a] && listenTilesNum[pos] == listenTilesNum[a]) {
					candidate.push_back(a);
				}
			}

			for (unsigned a = 0; a < candidate.size(); ++a) {
				vector<int> temp2;
				int temp[2];
				int t1 = moveArray[candidate[a]] >> 6;
				int t2 = moveArray[candidate[a]] & 63;
				int card1 = ((t1 / 9) + 1) * 100 + ((t1 % 9) + 1) * 10;
				int card2 = ((t2 / 9) + 1) * 100 + ((t2 % 9) + 1) * 10;
				temp2.reserve(4);
				temp[0] = card1;
				temp[1] = card2;
				for (int i = 0; i < 2; ++i) {
					for (int j = 0; j < 4; ++j) {
						int cardId = temp[i] + j;
						if (privateHand.findCard(cardId) == true) {
#if (SHOW_INFORMATION && !STDIN)
							printf("%d ", cardId);
#endif
							temp2.push_back(cardId);
							break;
						}
					}
				}
				move.push_back(temp2);
			}
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: %d %d\n", move[0][0], move[0][1]);
#endif
			return move[0];
		}
		else {
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: pass\n");
#endif
		}
	}

	return result;
}

vector<int> AI::dealPong() {
#if (SHOW_INFORMATION && !STDIN)
	printf("===== Pong =====\n");
#endif
	std::vector<int> result;
	if (mode == 0) {
		if (board.predictRemainTiles.getRemainTotalTilesNum() - 64 + oppGroup <= 16 && board.listenNum > 0) {
			return result;//剩不多排就流局、且自己離聽牌很遠：不碰了
		}
		int tempArray[17] = { 0 };//不重複的手排名單 AI定義
		int tempNumArray[17] = { 0 };//上者各自的個數
		int length = board.privateHand.getTileNumArray(tempArray, tempNumArray);//手排數量(不含重複?)
		int seaCard = (wallTiles.getTileSea() / 100 - 1) * 9 + (wallTiles.getTileSea() / 10 % 10 - 1);
		int move = -1;

		for (int i = 0; i < length; ++i) {
			if (tempArray[i] == seaCard && tempNumArray[i] >= 2) {//可以碰的條件
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				//printf("%d %d\n", BBlistenNum, currentListenNum);
				if (listenNum < board.listenNum) {//假設丟了之後可以改善上聽數
					move = tempArray[i];

					bool isNumberTile = tempArray[i] / 9 != 3;
					bool atMiddleHasNeighbors = //左右有連續排且只有一個 e.g 5667
						(i > 0) && (i + 1 < length) &&//不是邊張 e.g: 5677
						tempArray[i - 1] + 1 == tempArray[i] && tempArray[i + 1] - 1 == tempArray[i] &&
						tempNumArray[i - 1] == 1 && tempNumArray[i + 1] == 1;//左右只有一個

					bool connectedToFarLeft =
						(i > 1) && (tempArray[i - 2] + 2 == tempArray[i]);

					bool connectedToFarRight =
						(i + 2 < length) && (tempArray[i + 2] - 2 == tempArray[i]);

					if (isNumberTile && atMiddleHasNeighbors
						&& !connectedToFarLeft && !connectedToFarRight) {//滿足條件->不丟
						table->UpdateTableID(board.tableID, tempArray[i], TAKE);
						table->UpdateTableID(board.tableID, tempArray[i], TAKE);
						return result;
					}
				}
				//不滿足條件
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				break;
			}
		}
		if (move != -1) {//沒有滿足976-083各種條件的話，但可以改善上聽數->丟
			int card1 = ((move / 9) + 1) * 100 + ((move % 9) + 1) * 10;
			int count = 0;
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: ");
#endif
			for (int j = 0; j < 4; ++j) {
				int cardId = card1 + j;
				if (privateHand.findCard(cardId) == true) {
#if (SHOW_INFORMATION && !STDIN)
					printf("%d ", cardId);
#endif
					result.push_back(cardId);
					count++;
					if (count == 2)
						break;
				}
			}
#if (SHOW_INFORMATION && !STDIN)
			printf("\n");
#endif
		}
		else {//沒有改善上聽數->pass
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: pass\n");
#endif
		}
	}
	return result;
}

vector<int> AI::dealMingGong() {
#if (SHOW_INFORMATION && !STDIN)
	printf("===== Ming Gong =====\n");
#endif
	vector<int> result;
	if (mode == 0) //AI mode
	{
#if (MING_GONG_LEGAL == 1)
		if (board.listenNum == 0 || (board.predictRemainTiles.getRemainTotalTilesNum() - 64 + oppGroup <= 16 && board.listenNum > 0)) {
			return result;
		}
#endif
		int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
		int length = board.privateHand.getTileNumArray(tempArray, tempNumArray);
		int seaCard = (wallTiles.getTileSea() / 100 - 1) * 9 + (wallTiles.getTileSea() / 10 % 10 - 1);
		int move = -1;

		for (int i = 0; i < length; ++i) {	//Ming Gong
			if (tempArray[i] == seaCard && tempNumArray[i] == 3) {
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				if (listenNum == board.listenNum) {
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);

					if (table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes) < board.listenNum) {
						table->UpdateTableID(board.tableID, tempArray[i], TAKE);
						table->UpdateTableID(board.tableID, tempArray[i], TAKE);
						return result;
					}

					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					move = tempArray[i];
					break;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
			}
		}
		if (move != -1) {
			//printf("Ming Gong:\n");
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: ");
#endif
			int card1 = ((move / 9) + 1) * 100 + ((move % 9) + 1) * 10;

			for (int j = 0; j < 4; ++j) {
				int cardId = card1 + j;
				if (privateHand.findCard(cardId) == true) {
#if (SHOW_INFORMATION && !STDIN)
					printf("%d ", cardId);
#endif
					result.push_back(cardId);
				}
			}
#if (SHOW_INFORMATION && !STDIN)
			printf("\n");
#endif
		}
		else {
#if (SHOW_INFORMATION && !STDIN)
			printf("Sugesstion: pass\n");
#endif
		}

	}
	return result;
}

vector<int> AI::dealDarkGong() {
	vector<int> result;
	if (mode == 0) {
		int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
		int move = -1;
		int length = board.privateHand.getTileNumArray(tempArray, tempNumArray);

		for (int i = 0; i < length; ++i) {	//Dark Gong
			if (tempNumArray[i] == 4) {
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				table->UpdateTableID(board.tableID, tempArray[i], THROW);
				int listenNum(table->getTilesListenNum(4 - board.publicGroupNum, board.tableID, board.is_have_eyes));
				if (listenNum <= board.listenNum) {
					move = tempArray[i];
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					table->UpdateTableID(board.tableID, tempArray[i], TAKE);
					break;
				}
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
				table->UpdateTableID(board.tableID, tempArray[i], TAKE);
			}
		}
		if (move != -1) {
			int card1 = ((move / 9) + 1) * 100 + ((move % 9) + 1) * 10;

			for (int j = 0; j < 4; ++j) {
				int cardId = card1 + j;
				if (privateHand.findCard(cardId) == true) {
					result.push_back(cardId);
				}
			}
		}
	}
	return result;
}

vector<int> AI::dealBuGong(const int& takeTile) {
	vector<int> result;
	if (mode == 0) {
		int move = -1;
		int tile = (takeTile / 100 - 1) * 9 + (takeTile / 10 % 10 - 1);

		table->UpdateTableID(board.tableID, tile, THROW);
		int listenNum(table->getTilesListenNum(5 - board.publicGroupNum, board.tableID, board.is_have_eyes));
		if (listenNum <= board.listenNum) {
			move = tile;
		}
		table->UpdateTableID(board.tableID, tile, TAKE);
		if (move != -1) {
			result.push_back(takeTile);
		}
	}
	return result;
}

vector<int> AI::dealHu() {
	vector<int> temp(1, (mode == 0) ? 1 : 0);
	return temp;
}

vector<int> AI::dealThrow() {
	vector<int> result;
	if (mode == 0) {
		int record = 0;

		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		int throwList[17] = { 0 };
		int throwArraySize(ThrowList(board, throwList));
		//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//printf("Spend Time: %lld\n", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()); //35500

#if (SHOW_THROW_LIST && !STDIN)
		printf("throwList ");
		for (int i = 0; i < throwArraySize; ++i) {
			printf("%d ", throwList[i]);
		}
		printf("\n");
#endif
		int remainNumber = board.predictRemainTiles.getRemainTotalTilesNum() - 64 + oppGroup;
		/*if (remainNumber > 16 && remainNumber <= 24 && board.listenNum > 0) {
			int riskArray[17] = { 0 }, minRiskNum = 999;
			for (int i = 0; i < throwArraySize; i++) {
				if (throwList[i] / 9 != 3 && throwList[i] % 9 - 2 >= 0) {
					if (board.predictRemainTiles.getRemainTilesNum(throwList[i] - 2) > 0 && board.predictRemainTiles.getRemainTilesNum(throwList[i] - 1) > 0) {
						riskArray[i]++;
					}
				}
				if (throwList[i] / 9 != 3 && throwList[i] % 9 - 1 >= 0 && throwList[i] % 9 + 1 <= 8) {
					if (board.predictRemainTiles.getRemainTilesNum(throwList[i] - 1) > 0 && board.predictRemainTiles.getRemainTilesNum(throwList[i] + 1) > 0) {
						riskArray[i]++;
					}
				}
				if (throwList[i] / 9 != 3 && throwList[i] % 9 + 2 <= 8) {
					if (board.predictRemainTiles.getRemainTilesNum(throwList[i] + 2) > 0 && board.predictRemainTiles.getRemainTilesNum(throwList[i] + 1) > 0) {
						riskArray[i]++;
					}
				}

				int card = ((throwList[i] / 9) + 1) * 100 + ((throwList[i] % 9) + 1) * 10;
				int specificTileNum = 0;
				for (int j = 0; j < 4; j++) {
					int cardId = card + j;
					if (privateHand.findCard(cardId) == true) {
						specificTileNum++;
					}
				}
				// check pong probability
				if (board.predictRemainTiles.getRemainTilesNum(throwList[i]) >= 2) {
					riskArray[i] += 2;
				}
				if (minRiskNum > riskArray[i]) {
					minRiskNum = riskArray[i];
				}
			}
			int tempArraySize = 0;
			for (int i = 0; i < throwArraySize; i++) {
				if (minRiskNum == riskArray[i]) {
					throwList[tempArraySize++] = throwList[i];
				}
			}
			throwArraySize = tempArraySize;
			if (throwArraySize == 1) {
				int card1 = ((throwList[0] / 9) + 1) * 100 + ((throwList[0] % 9) + 1) * 10;
				for (int j = 0; j < 4; j++) {
					int cardId = card1 + j;
					if (privateHand.findCard(cardId) == true) {
						result.push_back(cardId);
						return result;
					}
				}
			}
		}
		else*/
		if (DefenseAnalyze::shouldDefend(board.myPosition, publicHand)) {
#if (SHOW_INFORMATION && !STDIN)
			printf(">>> The opponent has two groups of secondary Luda and activates the defensive mode！ <<<\n");
#endif
			// 取得安全牌清單
			std::vector<int> safeCandidates = DefenseAnalyze::getSafeTiles(throwList, throwArraySize, wallTiles);

			if (!safeCandidates.empty()) {
#if (SHOW_INFORMATION && !STDIN)
				printf(">>> Find the %zu safety card and let MCTS evaluate the best defensive solution <<<\n", safeCandidates.size());
#endif
				throwArraySize = safeCandidates.size();
				for (int i = 0; i < throwArraySize; ++i) {
					throwList[i] = safeCandidates[i];
				}
			}
		}
		// ==========================================
		if (remainNumber <= 16 && board.listenNum > 0) {
			vector<int> tileSea = wallTiles.getAllTileSea();
			for (int i = (int)tileSea.size() - 1; i >= (int)tileSea.size() - 8; --i) {
				for (int j = 0; j < 4; ++j) {
					int cardId = tileSea[i] + j;
					if (privateHand.findCard(cardId) == true) {
						result.push_back(cardId);
						return result;
					}
				}
			}
			int tempArray[17] = { 0 };
			int tempNumArray[17] = { 0 };
			//int minRemain = 9999, minTile = -1;
			int length = board.privateHand.getTileNumArray(tempArray, tempNumArray);
			int riskArray[17] = { 0 }, minRiskNum = 999;
			for (int i = 0; i < length; ++i) {
				if (tempArray[i] / 9 != 3 && tempArray[i] % 9 - 2 >= 0) {
					if (board.predictRemainTiles.getRemainTilesNum(tempArray[i] - 2) > 0 && board.predictRemainTiles.getRemainTilesNum(tempArray[i] - 1) > 0) {
						++riskArray[i];
					}
				}
				if (tempArray[i] / 9 != 3 && tempArray[i] % 9 - 1 >= 0 && tempArray[i] % 9 + 1 <= 8) {
					if (board.predictRemainTiles.getRemainTilesNum(tempArray[i] - 1) && board.predictRemainTiles.getRemainTilesNum(tempArray[i] + 1) > 0) {
						++riskArray[i];
					}
				}
				if (tempArray[i] / 9 != 3 && tempArray[i] % 9 + 2 <= 8) {
					if (board.predictRemainTiles.getRemainTilesNum(tempArray[i] + 2) && board.predictRemainTiles.getRemainTilesNum(tempArray[i] + 1)) {
						++riskArray[i];
					}
				}
				// check pong probability
				if (board.predictRemainTiles.getRemainTilesNum(tempArray[i]) >= 2) {
					riskArray[i] += 2;
				}
				if (minRiskNum > riskArray[i]) {
					minRiskNum = riskArray[i];
				}
			}
			int tempArraySize = 0;
			for (int i = 0; i < length; ++i) {
				if (minRiskNum == riskArray[i]) {
					throwList[tempArraySize++] = tempArray[i];
				}
			}
			throwArraySize = tempArraySize;
			if (throwArraySize == 1) {
				int card1 = ((throwList[0] / 9) + 1) * 100 + ((throwList[0] % 9) + 1) * 10;
				for (int j = 0; j < 4; ++j) {
					int cardId = card1 + j;
					if (privateHand.findCard(cardId) == true) {
						result.push_back(cardId);
						return result;
					}
				}
			}
			/*for (int i = 0; i < length; i++) {
				if (minRemain > board.predictRemainTiles.getRemainTilesNum(tempArray[i]) && tempNumArray[i] < 3) {
					minRemain = board.predictRemainTiles.getRemainTilesNum(tempArray[i]);
					minTile = tempArray[i];
				}
			}
			if (minTile != -1) {
				int card1 = ((minTile / 9) + 1) * 100 + ((minTile % 9) + 1) * 10;
				for (int j = 0; j < 4; j++) {
					int cardId = card1 + j;
					if (privateHand.findCard(cardId) == true) {
						result.push_back(cardId);
						return result;
					}
				}
			}*/
		}
		else if (throwArraySize == 1) {
			int card1 = ((throwList[0] / 9) + 1) * 100 + ((throwList[0] % 9) + 1) * 10;
			for (int j = 0; j < 4; ++j) {//有四張卡，一定會找到
				int cardId = card1 + j;
				if (privateHand.findCard(cardId) == true) {
					result.push_back(cardId);
					return result;
				}
			}
		}
		else if (throwArraySize > 1) { // for initGame first player
			int numbers = 0;
			for (int i = 0; i < throwArraySize; ++i) {
				if (throwList[i] / 9 == 3) {
					++numbers;
				}
			}
			if (numbers == throwArraySize) {
				int filterArray[7] = { 0 }, filterArraySize = 0;
				for (int j = 0; j < throwArraySize; ++j) {
					if (throwList[j] != 27 + board.myPosition && throwList[j] < 31) {
						filterArray[filterArraySize++] = throwList[j];
					}
				}
#ifdef __GNUC__
				pcg_extras::seed_seq_from<std::random_device> seed_source;
				pcg32 rng(seed_source);
#elif _MSC_VER
				std::random_device rd;
				std::mt19937 rng(rd());
#endif
				std::uniform_int_distribution<int> uniform_dist(0, (filterArraySize > 0) ? filterArraySize - 1 : throwArraySize - 1);
				int num = (filterArraySize > 0) ? filterArray[uniform_dist(rng)] : throwList[uniform_dist(rng)];
				int card1 = ((num / 9) + 1) * 100 + ((num % 9) + 1) * 10;
				for (int j = 0; j < 4; ++j) {
					int cardId = card1 + j;
					if (privateHand.findCard(cardId) == true) {
						result.push_back(cardId);
						return result;
					}
				}
			}
		}
#if (CLOSE_MCS == 0)
		clock_t totalStart = clock();
		MCTS(board, throwList, throwArraySize);
		totalDuration = (double)(clock() - totalStart) * 1000 / CLOCKS_PER_SEC;

#ifdef LOG_TIME_AND_TREESIZE
		{
			// Log time and treeSize, with remainTilesTotal
			std::string fileName = "timeLog" + std::to_string(getID()) + ".csv";
			std::ofstream timeLog(fileName, std::ios::app);
			timeLog << board.predictRemainTiles.getRemainTotalTilesNum() << "," << totalDuration << "\n";
			timeLog.close();

			fileName = "treeSizeLog" + std::to_string(getID()) + ".csv";
			std::ofstream treeSizeLog(fileName, std::ios::app);
			treeSizeLog << board.predictRemainTiles.getRemainTotalTilesNum() << "," << treeSize << "\n";
			treeSizeLog.close();
		}
#endif
#endif
		if (throwArraySize > 0) {
			int winRateIndexArray[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ,11, 12, 13, 14, 15, 16 };
			QuickSort(winRateIndexArray, 0, nodes[0].childrenNum - 1); /// 將children依照勝率(average)排序

			for (int i = 0; i < throwArraySize; i++) std::cout << throwList[winRateIndexArray[i]] << " "; // Debug
			std::cout << "\n"; // Debug
			for (int i = 0; i < nodes[0].childrenNum; i++) std::cout << nodes[nodes[0].childIndex[winRateIndexArray[i]]].averageScore << " "; // Debug
			std::cout << "\n"; // Debug

			int maxValid = 0; /// 有效牌種數
			int maxValidArray[17] = { winRateIndexArray[0] }, maxValidArraySize = 1;
			table->UpdateTableID(board.tableID, throwList[winRateIndexArray[0]], THROW); /// 丟棄勝率最高的牌
			for (int i = 0; i < 3; ++i) { /// 數牌
				if (board.tableID[i] != 0) { /// 試試每種牌，如果場上還有該牌且得到該牌會降低進聽數，則maxValid++
					for (int b = i * 9; b < i * 9 + 9; ++b) {
						if (board.predictRemainTiles.getRemainTilesNum(b) > 0) {
							table->UpdateTableID(board.tableID, b, TAKE);
							if (board.listenNum > table->getTilesListenNum(5 - board.publicGroupNum, board.tableID, board.is_have_eyes)) {
								++maxValid;
							}
							table->UpdateTableID(board.tableID, b, THROW);
						}
					}
				}
			}
			if (board.tableID[3] != 0) { /// 字牌
				for (int b = 27; b < 34; ++b) {
					if (board.predictRemainTiles.getRemainTilesNum(b) > 0) {
						table->UpdateTableID(board.tableID, b, TAKE);
						if (board.listenNum > table->getTilesListenNum(5 - board.publicGroupNum, board.tableID, board.is_have_eyes)) {
							++maxValid;
						}
						table->UpdateTableID(board.tableID, b, THROW);
					}
				}
			}
			//maxValid = table->getValidTiles(board.tableID, board.predictRemainTiles, board.is_have_eyes);
			table->UpdateTableID(board.tableID, throwList[winRateIndexArray[0]], TAKE);
			record = winRateIndexArray[0];

			for (int a = 1; a < nodes[0].childrenNum; ++a) {
				if (nodes[nodes[0].childIndex[winRateIndexArray[0]]].averageScore - nodes[nodes[0].childIndex[winRateIndexArray[a]]].averageScore <= 0.015) {
					table->UpdateTableID(board.tableID, throwList[winRateIndexArray[a]], THROW);
					int validNum = 0;
					for (int i = 0; i < 3; ++i) {
						if (board.tableID[i] != 0) {
							for (int b = i * 9; b < i * 9 + 9; ++b) {
								if (board.predictRemainTiles.getRemainTilesNum(b) > 0) {
									table->UpdateTableID(board.tableID, b, TAKE);
									if (board.listenNum > table->getTilesListenNum(5 - board.publicGroupNum, board.tableID, board.is_have_eyes)) {
										++validNum;
									}
									table->UpdateTableID(board.tableID, b, THROW);
								}
							}
						}
					}
					if (board.tableID[3] != 0) {
						for (int b = 27; b < 34; ++b) {
							if (board.predictRemainTiles.getRemainTilesNum(b) > 0) {
								table->UpdateTableID(board.tableID, b, TAKE);
								if (board.listenNum > table->getTilesListenNum(5 - board.publicGroupNum, board.tableID, board.is_have_eyes)) {
									++validNum;
								}
								table->UpdateTableID(board.tableID, b, THROW);
							}
						}
					}
					if (maxValid < validNum) {
						maxValid = validNum;
						record = winRateIndexArray[a];
						maxValidArray[0] = winRateIndexArray[a];
						maxValidArraySize = 1;
					}
					else if (maxValid == validNum) {
						maxValidArray[maxValidArraySize++] = winRateIndexArray[a];
					}
					table->UpdateTableID(board.tableID, throwList[winRateIndexArray[a]], TAKE);
				}
				else {
					break;
				}
			}

			if (maxValidArraySize >= 2) {
				int max_appear_num = wallTiles.getTileSeaTileNum(throwList[maxValidArray[0]]), min_second_comparison_num = 0;
				if (throwList[maxValidArray[0]] % 9 != 0) {
					min_second_comparison_num += 4 - wallTiles.getTileSeaTileNum(throwList[maxValidArray[0]] - 1);
				}
				if (throwList[winRateIndexArray[0]] % 9 != 8) {
					min_second_comparison_num += 4 - wallTiles.getTileSeaTileNum(throwList[maxValidArray[0]] + 1);
				}
				for (int i = 0; i < maxValidArraySize; ++i) {
					int appear_number = wallTiles.getTileSeaTileNum(throwList[maxValidArray[i]]), second_comparison_num = 0;
					if (throwList[maxValidArray[i]] % 9 != 0) {
						second_comparison_num += 4 - wallTiles.getTileSeaTileNum(throwList[maxValidArray[i]] - 1);
					}
					if (throwList[maxValidArray[i]] % 9 != 8) {
						second_comparison_num += 4 - wallTiles.getTileSeaTileNum(throwList[maxValidArray[i]] + 1);
					}
					if (max_appear_num < appear_number || (max_appear_num == appear_number && min_second_comparison_num > second_comparison_num)) {
						min_second_comparison_num = second_comparison_num;
						max_appear_num = appear_number;
						record = maxValidArray[i];
					}
				}
			}

#if (SHOW_WIN_RATE && !STDIN)
			double  max = -99999.9;
			for (int a = 0; a < nodes[0].Nchild; ++a) { // pick move
				//printf("%d / %d, %lf, %lf\n", nodes[nodes[0].c_id[a]].sum1, nodes[nodes[0].c_id[a]].Ntotal, nodes[nodes[0].c_id[a]].average , nodes[nodes[0].c_id[a]].variance); // Modified(no comment)
				printf("%d / %d, %lf\n", nodes[nodes[0].c_id[a]].sum1, nodes[nodes[0].c_id[a]].Ntotal, nodes[nodes[0].c_id[a]].average); // Modified
			}
#endif
			int card1 = ((throwList[record] / 9) + 1) * 100 + ((throwList[record] % 9) + 1) * 10;
			memset(&nodes, 0, sizeof(nodes));
			for (int j = 0; j < 4; ++j) {
				int cardId = card1 + j;
				if (privateHand.findCard(cardId) == true) {
					result.push_back(cardId);
					//std::cout << "Choice: " << cardId << "\n"; // Debug
					//system("pause"); // Debug
					return result;
				}
			}

			for (int i = 0; i < throwArraySize; ++i) {
				printf("%d ", throwList[i]);
			}
			printf("record : %d\n", record);
			printf("*** Can't find MCTS tiles! Tile : %d ***\n", card1);
			std::cout << "linr 1483\n"; // Debug
			return input(1, false);
		}
		else {
			throw std::invalid_argument("***** ThrowList Empty! *****\n");
		}
	}
	else { // Random AI player
		vector<int> temp = privateHand.getTiles();
		if (temp.size() <= 0) fprintf(stderr, "Random Empty.\n");
#ifdef __GNUC__
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		pcg32 rng(seed_source);
#elif _MSC_VER
		std::random_device rd;
		std::mt19937 rng(rd());
#endif
		std::uniform_int_distribution<int> uniform_dist(0, temp.size() - 1);
		int num = temp[uniform_dist(rng)];
		result.push_back(num);
	}
	return result;
}

vector<int> AI::input(const int& Input_num, const bool& Pass) {
	int input;
	vector<int> temp;
	privateHand.ShowPrivateHand();
	for (int i = 0; i < Input_num; ++i) {
		scanf("%d\n", &input);
		if (input == -1 && Pass) {
			temp.clear();
			return temp;
		}
		else if (!privateHand.findCard(input)) {
			i = -1;
			temp.clear();
			fprintf(stderr, "Wrong Card\n");
			continue;
		}
		temp.push_back(input);
	}
	return temp;
}

inline void AI::DealTake(const int& takeTile) {
	board.takeTile = (takeTile / 100 - 1) * 9 + (takeTile / 10 % 10 - 1);
	board.privateHand.AddHand(board.takeTile, hashKey); // Modified, not privatehand in Player.cpp
	table->UpdateTableID(board.tableID, board.takeTile, TAKE);
	board.listenNum = table->getTilesListenNum(5 - publicHand[board.myPosition].getGroupNum(), board.tableID, board.is_have_eyes);
}

inline void AI::DealRemainTiles(const int& card) {
	int num = (card / 100 - 1) * 9 + (card / 10 % 10 - 1);
	this->board.predictRemainTiles.AddCard(num); /// 把牌加到已經出現的牌集合中
}

inline void AI::MoveThrow(const vector<int>& throwTile) {
	if (mode == 0) { /// AI mode
		if (!throwTile.empty()) {
			for (unsigned i = 0; i < throwTile.size(); ++i) {
				int tile = (throwTile[i] / 100 - 1) * 9 + (throwTile[i] / 10 % 10 - 1);
				board.throwTile = tile;
				board.privateHand.RemoveHand(board.throwTile, hashKey); // Modified
				table->UpdateTableID(board.tableID, board.throwTile, THROW);
			}
		}
		else {
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = board.privateHand.getTileNumArray(tempArray, tempNumArray);
			table->UpdateTableID(tempArray, tempNumArray, length, board.tableID);
		}
		if (throwTile.size() > 1) {
			board.publicGroupNum++;
		}
		board.listenNum = table->getTilesListenNum(5 - publicHand[board.myPosition].getGroupNum(), board.tableID, board.is_have_eyes);
	}
}

inline void AI::MakeTake(Board& b, const int& player, const int& action) {
	int card1 = (action >> 3) & 63, code = action & 7;

	switch (code) {
	case TAKE: {
		int cards = b.predictRemainTiles.TakeTile();
		b.takeTile = cards;
		b.privateHand.AddHand(cards, hashKey); // Modified
		table->UpdateTableID(b.tableID, cards, TAKE);
		break;
	}
	case EAT: {
		int card2 = action >> 12;
		b.takeTile = card1;
		b.publicGroupNum++;
		b.privateHand.RemoveHand(card1, hashKey); // Modified
		b.privateHand.RemoveHand(card2, hashKey); // Modified
		table->UpdateTableID(b.tableID, card1, THROW);
		table->UpdateTableID(b.tableID, card2, THROW);
		break;
	}
	case PONG: {
		b.takeTile = card1;
		b.publicGroupNum++;
		b.privateHand.RemoveHand(card1, hashKey); // Modified
		b.privateHand.RemoveHand(card1, hashKey); // Modified
		table->UpdateTableID(b.tableID, card1, THROW);
		table->UpdateTableID(b.tableID, card1, THROW);
		break;
	}
	case MING_GONG: {
		b.takeTile = -1;
		b.publicGroupNum++;
		for (int i = 0; i < 3; ++i) {
			b.privateHand.RemoveHand(card1, hashKey); // Modified
			table->UpdateTableID(b.tableID, card1, THROW);
		}
		int cards = b.predictRemainTiles.TakeTile();
		b.privateHand.AddHand(cards, hashKey); // Modified
		table->UpdateTableID(b.tableID, cards, TAKE);
		break;
	}
	case DARK_GONG: {
		b.takeTile = -1;
		b.publicGroupNum++;
		for (int i = 0; i < 4; ++i) {
			b.privateHand.RemoveHand(card1, hashKey); // Modified
			table->UpdateTableID(b.tableID, card1, THROW);
		}
		int cards = b.predictRemainTiles.TakeTile();
		b.privateHand.AddHand(cards, hashKey); // Modified
		table->UpdateTableID(b.tableID, cards, TAKE);
		break;
	}
	case BU_GONG: {
		b.takeTile = -1;
		b.privateHand.RemoveHand(card1, hashKey); // Modified
		table->UpdateTableID(b.tableID, card1, THROW);
		int cards = b.predictRemainTiles.TakeTile();
		b.privateHand.AddHand(cards, hashKey); // Modified
		table->UpdateTableID(b.tableID, cards, TAKE);
		break;
	}
	default:
		break;
	}

	if (isHu(b, b.publicGroupNum)) {
		b.status = player + 4;
	}
	else if (b.predictRemainTiles.getRemainTotalTilesNum() <= 64 - oppGroup) {
		b.status = Draw;
	}
}

inline void AI::MakeThrow(Board& b, const int& card, const bool& real) {
	if (b.status == b.myPosition) {
		b.throwTile = card;
		b.privateHand.RemoveHand(card); // Modified
		table->UpdateTableID(b.tableID, card, THROW);
	}
	UpdateStatus(b);
}

inline void AI::UpdateStatus(Board& b) {
	switch (b.status) {
	case OnePlay:
		b.status = TwoPlay;
		break;
	case TwoPlay:
		b.status = ThreePlay;
		break;
	case ThreePlay:
		b.status = FourPlay;
		break;
	case FourPlay:
		b.status = OnePlay;
		break;
	default:
		b.status = NONE;
		break;
	}
}

void AI::ShowGameInformation(const vector<int>& cards) {
	printf("\n==============================\n");

	switch (this->mode) {
	case 0:
		printf("Mode : AI (MCS)\n");
		printf("Number of Simulations : %d\n", SIMULATION_NUM);
		break;
	case 2:
		printf("Mode : AI (Random)\n");
	}
	printf("Initial Hand Size : %d\n", (int)cards.size());
	printf("==============================\n");
}

void AI::ShowFunctionTime() { // Modified(no comment)
	/*printf("Averge Times : %lf\n", times);
	printf("Hu Time : %lf\n", huTime / times);
	printf("ListenTable Time : %lf\n", listenTableTime / times);
	printf("Dismantling Time : %lf\n", dismantlingTime / times);
	printf("Throw Time : %lf\n", throwTime / times);
	printf("MCTS Time : %lf\n", MCTS_Time / times);*/
}

//-----------------------------  MCTS  ------------------------------------

inline int AI::Simulate(const Board& a, const int& throwCard) {
#if (SMART_AI_ENABLE == 1 || SMART_AI_ENABLE == 2)
	Board system(a);	// for take tiles system /// 拿來發牌的還有記錄當前玩家的
	Board b_AI[4] = { a, a, a, a };
	for (int i = 0; i < 4; i++) {
		b_AI[i].publicGroupNum = publicHand[i].getGroupNum(); /// 將目前場上玩家的明牌數寫入publicGroupNum
	}
	int myPosition = a.myPosition;
	system.status = a.myPosition; /// 我方為先手
	b_AI[myPosition].privateHand.RemoveHand(throwCard); /// 把要捨的牌捨掉
	table->UpdateTableID(b_AI[myPosition].tableID, throwCard, THROW); /// 更新我方牌的資訊
	UpdateStatus(system); /// 更新status，換下家出牌

	// init hand for all other players
	for (int i = 0; i < 4; i++) {
		if (i != myPosition) { /// 其他玩家們
			vector<int> tempHand;
			tempHand.reserve(17);
			for (int j = 0; j < 16 - (b_AI[i].publicGroupNum * 3); j++) { /// 撇除掉明牌，把其他玩家的牌補到16張
				int takecard = system.predictRemainTiles.TakeTile();
				tempHand.push_back(takecard);
				b_AI[i].predictRemainTiles.AddCard(takecard);
			}
			b_AI[i].privateHand.SetHand(tempHand); /// 把產生的16張手排寫入玩家暗牌集合
			/// 因為原本是複製a，所以要把預測手牌中原本扣掉我方(a)暗牌的部分加回去。明牌部分不用加回去，因為是公開資訊
			b_AI[i].predictRemainTiles.AddBackTiles(a.privateHand.getTiles());

			/// 更新其tableID
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = b_AI[i].privateHand.getTileNumArray(tempArray, tempNumArray);
			table->UpdateTableID(tempArray, tempNumArray, length, b_AI[i].tableID);
		}
		/// 更新其進聽數
		b_AI[i].listenNum = table->getTilesListenNum(5 - b_AI[i].publicGroupNum, b_AI[i].tableID, b_AI[i].is_have_eyes); /// 建立每位玩家的上聽數資訊
	}
#else
	Board b(a);
	b.status = b.myPosition; /// 第幾個player(0 ~ 3)
	MakeThrow(b, throwCard, false); /// 把牌捨掉
#endif

#if (EAT_PONG_ENABLE == 1)
	int seaCard = throwCard; /// seaCard: 紀錄上此捨牌玩家捨的牌
	int finalThrowPlayer = a.myPosition; /// 預設上次捨牌者為我方
#endif
	while (true) {
#if (EAT_PONG_ENABLE == 1 && SMART_AI_ENABLE == 2)
		bool askPong = false, askEat = false;

		// check hu
		for (int i = 0; i < 3; i++) {
			table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, TAKE);
			int listenNum = table->getTilesListenNum(5 - b_AI[(system.status + i) & 3].publicGroupNum, b_AI[(system.status + i) & 3].tableID, b_AI[(system.status + i) & 3].is_have_eyes);
			if (listenNum == -1 && ((system.status + i) & 3) != myPosition) {
				return 0;
			}
			else if (listenNum == -1 && finalThrowPlayer == myPosition) {
				return -1;
			}
			else if (listenNum == -1) {
				/*for (int i = 0; i < 4; i++) {
					b_AI[i].privateHand.ShowPrivateHandName();
				}
				printf("\n");*/
				return 1;
			}
			table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, THROW);
		}

		// check who can pong
		for (int i = 0; i < 3; i++) {
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = b_AI[(system.status + i) & 3].privateHand.getTileNumArray(tempArray, tempNumArray);
			bool doPong = false;
			if (isCanPong(tempArray, tempNumArray, length, seaCard)) {
				b_AI[(system.status + i) & 3].listenNum = table->getTilesListenNum(5 - b_AI[(system.status + i) & 3].publicGroupNum, b_AI[(system.status + i) & 3].tableID, b_AI[(system.status + i) & 3].is_have_eyes);
				table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, THROW);
				table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, THROW);
				int listenNum = table->getTilesListenNum(4 - b_AI[(system.status + i) & 3].publicGroupNum, b_AI[(system.status + i) & 3].tableID, b_AI[(system.status + i) & 3].is_have_eyes);
				if (listenNum < b_AI[(system.status + i) & 3].listenNum) {
					doPong = true;
				}
				else {
					table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, TAKE);
					table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, TAKE);
				}

				if (doPong) {
					system.status = (system.status + i) & 3;
					b_AI[system.status].privateHand.RemoveHand(seaCard);
					b_AI[system.status].privateHand.RemoveHand(seaCard);
					b_AI[system.status].publicGroupNum++;
					askPong = true;
					for (int j = 1; j < 4; j++) {
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(seaCard);
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(seaCard);
					}
					break;
				}
			}
		}

		// check who can eat
		if (!askPong) {
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = b_AI[system.status].privateHand.getTileNumArray(tempArray, tempNumArray);
			if (isCanEat(tempArray, tempNumArray, length, seaCard)) {
				int moveArray[5], listenTilesNum[5] = { 0 }, validTileNum[5] = { 0 }, candidate[5];
				int index = 0, candidateSize = 0;
				b_AI[system.status].listenNum = table->getTilesListenNum(5 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].is_have_eyes);
				for (int i = 0; i < length; i++) {	//Eat
					if (i + 1 < length && tempArray[i] == seaCard + 1 && tempArray[i + 1] == seaCard + 2
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // XOO
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], THROW);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], THROW);
						int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup, b_AI[system.status].is_have_eyes);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], TAKE);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], TAKE);
						if (listenNum < b_AI[system.status].listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
							if (tempArray[i + 1] % 9 != 8) {
								listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(tempArray[i + 1] + 1);
								listenTilesNum[index] += b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							}
							else {
								listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							}
							validTileNum[index] = table->getValidTiles(b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup);
							index++;
						}
					}
					else if (i + 1 < length && tempArray[i] == seaCard - 1 && tempArray[i + 1] == seaCard + 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OXO
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], THROW);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], THROW);
						int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup, b_AI[system.status].is_have_eyes);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], TAKE);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], TAKE);
						if (listenNum < b_AI[system.status].listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
							listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							validTileNum[index] = table->getValidTiles(b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup);
							index++;
						}
					}
					else if (i + 2 < length && tempArray[i] == seaCard - 1 && tempArray[i + 2] == seaCard + 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 2] / 9) { // OXO
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], THROW);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 2], THROW);
						int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup, b_AI[system.status].is_have_eyes);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], TAKE);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 2], TAKE);
						if (listenNum < b_AI[system.status].listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 2];
							listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							validTileNum[index] = table->getValidTiles(b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup);
							index++;
						}
					}
					else if (i + 1 < length && tempArray[i] == seaCard - 2 && tempArray[i + 1] == seaCard - 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OOX
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], THROW);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], THROW);
						int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup, b_AI[system.status].is_have_eyes);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i], TAKE);
						table->UpdateTableID(b_AI[system.status].tableID, tempArray[i + 1], TAKE);
						if (listenNum < b_AI[system.status].listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
							if (tempArray[i] % 9 != 0) {
								listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(tempArray[i] - 1);
								listenTilesNum[index] += b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							}
							else {
								listenTilesNum[index] = b_AI[system.status].predictRemainTiles.getRemainTilesNum(seaCard);
							}
							validTileNum[index] = table->getValidTiles(b_AI[system.status].tableID, b_AI[system.status].listenRecordGroup);
							index++;
						}
					}
				}
				if (index > 0) {
					int max = -1, pos = 0;
					for (int a = 0; a < index; a++) {
						if (max < validTileNum[a]) {
							max = validTileNum[a];
							pos = a;
						}
						else if (max == validTileNum[a] && listenTilesNum[pos] > listenTilesNum[a]) {
							max = validTileNum[a];
							pos = a;
						}
					}

					for (int a = 0; a < index; a++) {
						if (max == validTileNum[a] && listenTilesNum[pos] == listenTilesNum[a]) {
							candidate[candidateSize++] = a;
							break;
						}
					}
					b_AI[system.status].privateHand.RemoveHand(moveArray[candidate[0]] & 63);
					b_AI[system.status].privateHand.RemoveHand(moveArray[candidate[0]] >> 6);
					table->UpdateTableID(b_AI[system.status].tableID, moveArray[candidate[0]] & 63, THROW);
					table->UpdateTableID(b_AI[system.status].tableID, moveArray[candidate[0]] >> 6, THROW);
					b_AI[system.status].publicGroupNum++;
					askEat = true;
					for (int j = 1; j < 4; j++) {
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(moveArray[candidate[0]] & 63);
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(moveArray[candidate[0]] >> 6);
					}
				}
			}
		}

		// take tile
		if (!askPong && !askEat) {
			int card = system.predictRemainTiles.TakeTile();
			b_AI[system.status].privateHand.AddHand(card);
			table->UpdateTableID(b_AI[system.status].tableID, card, TAKE);
			b_AI[system.status].listenNum = table->getTilesListenNum(5 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].is_have_eyes);
			if (b_AI[system.status].listenNum == -1 && system.status != myPosition) {
				return -1;
			}
			else if (b_AI[system.status].listenNum == -1) {
				return 3;
			}
			else if (system.predictRemainTiles.getRemainTotalTilesNum() <= 16) {
				return 0;
			}
		}

		// deal throw
		int throwArray[17] = { 0 };
		int throwArraySize = ThrowList(b_AI[system.status], throwArray);
		if (throwArraySize > 0) {
#ifdef __GNUC__
			pcg_extras::seed_seq_from<std::random_device> seed_source;
			pcg32 rng(seed_source);
#elif _MSC_VER
			std::random_device rd;
			std::mt19937 rng(rd());
#endif
			std::uniform_int_distribution<int> uniform_dist(0, throwArraySize - 1);
			int next = throwArray[uniform_dist(rng)];
			for (int i = 1; i < 4; i++) {
				b_AI[(system.status + i) & 3].predictRemainTiles.AddCard(next);
			}
			seaCard = next;
			finalThrowPlayer = system.status;
			b_AI[system.status].privateHand.RemoveHand(next);
			table->UpdateTableID(b_AI[system.status].tableID, next, THROW);
			UpdateStatus(system);
		}
		else {
			b_AI[system.status].privateHand.ShowPrivateHandName();
			throw std::invalid_argument("ThrowList Empty.");
		}
#elif (EAT_PONG_ENABLE == 1 && SMART_AI_ENABLE == 1)
		bool askPong = false, askEat = false;

		// check hu
		for (int i = 0; i < 3; i++) { /// 上次捨牌玩家以外的三名玩家(因為system.status已更新)
			table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, TAKE); /// 試拿並計算進聽數
			int listenNum = table->getTilesListenNum(5 - b_AI[(system.status + i) & 3].publicGroupNum, b_AI[(system.status + i) & 3].tableID, b_AI[(system.status + i) & 3].is_have_eyes); /// 試拿後的進聽數
			if (listenNum == -1 && ((system.status + i) & 3) != myPosition && finalThrowPlayer != myPosition) { /// 除了我方外的其他三位玩家是否會胡 // Modified
				return 0; /// 別人胡牌
			}
			else if (listenNum == -1 && finalThrowPlayer == myPosition) {
				return -1; /// 我方放槍
			}
			else if (listenNum == -1) {
				/*for (int i = 0; i < 4; i++) {
					b_AI[i].privateHand.ShowPrivateHandName();
				}
				printf("\n");*/
				return 1; /// 我方胡牌
			}
			table->UpdateTableID(b_AI[(system.status + i) & 3].tableID, seaCard, THROW); /// 回復試拿操作
		}

		// check who can pong
		if (finalThrowPlayer != myPosition) { /// 若丟牌者不是我方，則檢查我方是否能碰
			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			int length = b_AI[myPosition].privateHand.getTileNumArray(tempArray, tempNumArray); /// length: 手牌牌種數
			bool doPong = false;
			if (isCanPong(tempArray, tempNumArray, length, seaCard)) { /// 如果我方能碰該牌
				b_AI[myPosition].listenNum = table->getTilesListenNum(5 - b_AI[myPosition].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes); /// 先記錄碰前進聽數
				table->UpdateTableID(b_AI[myPosition].tableID, seaCard, THROW);
				table->UpdateTableID(b_AI[myPosition].tableID, seaCard, THROW); /// 試碰
				int listenNum = table->getTilesListenNum(4 - b_AI[myPosition].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes); /// 計算碰後進聽數
				if (listenNum < b_AI[myPosition].listenNum) { /// 若碰前後進聽數有減少，則碰
					doPong = true;
				}
				else { /// 若碰前後進聽數沒有減少則回復試碰操作
					table->UpdateTableID(b_AI[myPosition].tableID, seaCard, TAKE);
					table->UpdateTableID(b_AI[myPosition].tableID, seaCard, TAKE);
				}

				if (doPong) { /// 真的碰牌 // Redundant??
					system.status = myPosition;
					b_AI[system.status].privateHand.RemoveHand(seaCard);
					b_AI[system.status].privateHand.RemoveHand(seaCard);
					b_AI[system.status].publicGroupNum++;
					askPong = true;
					for (int j = 1; j < 4; j++) { /// 更新其他玩家明牌資訊
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(seaCard);
						b_AI[(system.status + j) & 3].predictRemainTiles.AddCard(seaCard);
					}
					break;
				}
			}

			// check who can eat
			if (system.status == myPosition && !askPong) { /// 若下家為我方且沒有碰該牌
				if (isCanEat(tempArray, tempNumArray, length, seaCard)) { /// 判斷是否可吃牌 // move "if" upward??
					int moveArray[5], listenTilesNum[5] = { 0 }, validTileNum[5] = { 0 }, candidate[5];
					int index = 0, candidateSize = 0;
					b_AI[myPosition].listenNum = table->getTilesListenNum(5 - b_AI[myPosition].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes); /// 先記錄吃前進聽數
					for (int i = 0; i < length; i++) {	//Eat
						if (i + 1 < length && tempArray[i] == seaCard + 1 && tempArray[i + 1] == seaCard + 2
							&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // XOO
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], THROW);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], THROW);
							int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], TAKE);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], TAKE);
							if (listenNum < b_AI[myPosition].listenNum) {
								moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
								if (tempArray[i + 1] % 9 != 8) {
									listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(tempArray[i + 1] + 1);
									listenTilesNum[index] += b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								}
								else {
									listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								}
								validTileNum[index] = table->getValidTiles(b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
								index++;
							}
						}
						else if (i + 1 < length && tempArray[i] == seaCard - 1 && tempArray[i + 1] == seaCard + 1
							&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OXO
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], THROW);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], THROW);
							int listenNum = table->getTilesListenNum(4 - b_AI[myPosition].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], TAKE);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], TAKE);
							if (listenNum < b_AI[myPosition].listenNum) {
								moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
								listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								validTileNum[index] = table->getValidTiles(b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
								index++;
							}
						}
						else if (i + 2 < length && tempArray[i] == seaCard - 1 && tempArray[i + 2] == seaCard + 1
							&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 2] / 9) { // OXO
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], THROW);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 2], THROW);
							int listenNum = table->getTilesListenNum(4 - b_AI[system.status].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], TAKE);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 2], TAKE);
							if (listenNum < b_AI[myPosition].listenNum) {
								moveArray[index] = tempArray[i] << 6 | tempArray[i + 2];
								listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								validTileNum[index] = table->getValidTiles(b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
								index++;
							}
						}
						else if (i + 1 < length && tempArray[i] == seaCard - 2 && tempArray[i + 1] == seaCard - 1
							&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OOX
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], THROW);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], THROW);
							int listenNum = table->getTilesListenNum(4 - b_AI[myPosition].publicGroupNum, b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i], TAKE);
							table->UpdateTableID(b_AI[myPosition].tableID, tempArray[i + 1], TAKE);
							if (listenNum < b_AI[myPosition].listenNum) {
								moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
								if (tempArray[i] % 9 != 0) {
									listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(tempArray[i] - 1);
									listenTilesNum[index] += b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								}
								else {
									listenTilesNum[index] = b_AI[myPosition].predictRemainTiles.getRemainTilesNum(seaCard);
								}
								validTileNum[index] = table->getValidTiles(b_AI[myPosition].tableID, b_AI[myPosition].is_have_eyes);
								index++;
							}
						}
					}
					if (index > 0) {
						/// 找最高有效牌數的組合
						int max = -1, pos = 0; /// max: 最高有牌數, 最高有效牌數於validTileNum的index
						for (int a = 0; a < index; a++) {
							if (max < validTileNum[a]) {
								max = validTileNum[a];
								pos = a;
							}
							else if (max == validTileNum[a] && listenTilesNum[pos] > listenTilesNum[a]) {
								max = validTileNum[a];
								pos = a; /// 若有效牌數一樣，則聽牌數多者優先
							}
						}

						/// 把有相同最高有效牌數的index存在candidate // Redundant??, only throw index 0 in the code below
						for (int a = 0; a < index; a++) {
							if (max == validTileNum[a] && listenTilesNum[pos] == listenTilesNum[a]) {
								candidate[candidateSize++] = a;
								break;
							}
						}
						/// 真的吃牌
						b_AI[myPosition].privateHand.RemoveHand(moveArray[candidate[0]] & 63);
						b_AI[myPosition].privateHand.RemoveHand(moveArray[candidate[0]] >> 6);
						table->UpdateTableID(b_AI[myPosition].tableID, moveArray[candidate[0]] & 63, THROW);
						table->UpdateTableID(b_AI[myPosition].tableID, moveArray[candidate[0]] >> 6, THROW);
						b_AI[myPosition].publicGroupNum++;
						askEat = true;
						for (int j = 1; j < 4; j++) { /// 更新其他玩家明牌資訊
							b_AI[(myPosition + j) & 3].predictRemainTiles.AddCard(moveArray[candidate[0]] & 63);
							b_AI[(myPosition + j) & 3].predictRemainTiles.AddCard(moveArray[candidate[0]] >> 6);
						}
					}
				}
			}
		}

		// take tile
		if (!askPong && !askEat) { /// 若沒有吃也沒有碰 (可能我方沒吃碰或輪我方出牌不能吃碰))
			if (system.status != myPosition) {
				int validArray[34] = { 0 }, array[34] = { 0 }, arraySize = 0, card;

				int validArraySize = table->getValidTiles(b_AI[system.status].tableID, validArray, b_AI[system.status].is_have_eyes);
				//printf("size: %d\n", validArraySize);
				for (int i = 0; i < validArraySize; i++) {
					if (system.predictRemainTiles.getRemainTilesNum(validArray[i]) > 0) {
						array[arraySize++] = validArray[i];
					}
				}
				if (arraySize > 0) {
#ifdef __GNUC__
					pcg_extras::seed_seq_from<std::random_device> seed_source;
					pcg32 rng(seed_source);
#elif _MSC_VER
					std::random_device rd;
					std::mt19937 rng(rd());
#endif
					std::uniform_int_distribution<int> uniform_dist(0, arraySize - 1);
					card = array[uniform_dist(rng)];
					system.predictRemainTiles.AddCard(card);
				}
				else {
					card = system.predictRemainTiles.TakeTile();
				}
				b_AI[system.status].privateHand.AddHand(card);
				table->UpdateTableID(b_AI[system.status].tableID, card, TAKE);
			}
			else {
				int card = system.predictRemainTiles.TakeTile();
				b_AI[system.status].privateHand.AddHand(card);
				table->UpdateTableID(b_AI[system.status].tableID, card, TAKE);
			}
			std::cout << "Line 2152\n"; // Debug
			b_AI[system.status].listenNum = table->getTilesListenNum(5 - b_AI[system.status].publicGroupNum, b_AI[system.status].tableID, b_AI[system.status].is_have_eyes);
			if (b_AI[system.status].listenNum == -1 && system.status != myPosition) {
				return -1;
			}
			else if (b_AI[system.status].listenNum == -1) {
				return 3;
			}
			else if (system.predictRemainTiles.getRemainTotalTilesNum() <= 16) {
				return 0;
			}
		}
		std::cout << "Line 2164\n"; // Debug

		// deal throw
		int throwArray[17] = { 0 };
		int throwArraySize = ThrowList(b_AI[system.status], throwArray);
		if (throwArraySize > 0) {
#ifdef __GNUC__
			pcg_extras::seed_seq_from<std::random_device> seed_source;
			pcg32 rng(seed_source);
#elif _MSC_VER
			std::random_device rd;
			std::mt19937 rng(rd());
#endif
			std::uniform_int_distribution<int> uniform_dist(0, throwArraySize - 1);
			int next = throwArray[uniform_dist(rng)];
			for (int i = 1; i < 4; i++) {
				b_AI[(system.status + i) & 3].predictRemainTiles.AddCard(next);
			}
			seaCard = next;
			finalThrowPlayer = system.status;
			b_AI[system.status].privateHand.RemoveHand(next);
			table->UpdateTableID(b_AI[system.status].tableID, next, THROW);
			UpdateStatus(system);
		}
		else {
			b_AI[system.status].privateHand.ShowPrivateHandName();
			throw std::invalid_argument("ThrowList Empty.");
		}
		std::cout << "Line 2193\n"; // Debug
#elif (EAT_PONG_ENABLE == 1 && SMART_AI_ENABLE == 0)		
		bool askPong = false, askEat = false;
		if (finalThrowPlayer != b.myPosition) { /// 如果最後一個出牌的人不是自己
			// check chuck
			/// 檢查拿該玩家捨去的牌後是否會胡牌
			table->UpdateTableID(b.tableID, seaCard, TAKE); /// 拿牌
			int listenNum(table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, b.is_have_eyes)); /// 計算取後上聽數
			if (listenNum == -1) { /// 拿了該牌後會胡牌
				return 1; /// 贏了，return 1
			}
			/// 如果不會胡牌，復原剛剛拿牌的操作
			table->UpdateTableID(b.tableID, seaCard, THROW);

			int tempArray[17] = { 0 }, tempNumArray[17] = { 0 };
			/// 將目前自家暗牌的資訊寫到tempArray與tempNumArray，並把length設為牌種數
			int length(b.privateHand.getTileNumArray(tempArray, tempNumArray));
			// Check Pong
			for (int j = 0; j < length; ++j) {
				/// 如果暗牌中有丟出的牌，且數量 >= 2
				if (tempArray[j] == seaCard && tempNumArray[j] >= 2) { /// Pong
					/// 把與海底牌碰的兩張牌從自家暗牌的集合裡移出
					table->UpdateTableID(b.tableID, tempArray[j], THROW);
					table->UpdateTableID(b.tableID, tempArray[j], THROW);
					/// 試算碰完的上聽數(用4扣，因為碰完會多一組)
					int listenNum(table->getTilesListenNum(4 - b.publicGroupNum, b.tableID, b.is_have_eyes));
					if (listenNum < b.listenNum) { /// 如果碰會使上聽數減少
						/// 檢查j牌附近是否有順子，如果有順子且順子的左右兩邊任一邊沒有牌(01110)，就不碰
						/// 如果是131的狀況不碰嗎
						if (tempArray[j] / 9 != 3
							&& j > 0 && (j + 1) < length && tempArray[j - 1] + 1 == tempArray[j] && tempArray[j + 1] - 1 == tempArray[j]
							&& tempNumArray[j - 1] == 1 && tempNumArray[j + 1] == 1
							&& ((j > 1 && (j + 2) < length && tempArray[j - 2] + 2 != tempArray[j] && tempArray[j + 2] - 2 != tempArray[j])
								|| ((j + 2) < length && tempArray[j + 2] - 2 != tempArray[j])
								|| (j > 1 && tempArray[j - 2] + 2 != tempArray[j]))) {
							/// 選擇不碰牌，把剛剛試算碰牌時移除的兩張自家暗牌取回
							table->UpdateTableID(b.tableID, tempArray[j], TAKE);
							table->UpdateTableID(b.tableID, tempArray[j], TAKE);
							break;
						}
						/// 其餘上聽數有因為碰而減少的情況，則執行碰牌
						b.status = b.myPosition;
						b.takeTile = tempArray[j]; /// 拿取對方打出的牌
						b.privateHand.RemoveHand(tempArray[j]); /// 把要碰的牌從自家暗牌class中移除 // Modified
						b.privateHand.RemoveHand(tempArray[j]); // Modified
						b.publicGroupNum++; ///	把門前明牌組數加一(加碰出的那組)
						askPong = true;
						b.listenNum--; /// 上聽數--
						break;
					}
					else { /// 若碰該牌不會減少上聽數，則不碰，復原試碰操作，把牌加回暗牌集合
						table->UpdateTableID(b.tableID, tempArray[j], TAKE);
						table->UpdateTableID(b.tableID, tempArray[j], TAKE);
					}
				}
			}

			// Check eat
			/// 如果還沒碰 且 最後一個出牌的玩家為上家 且 打出的牌非字牌
			if (!askPong && finalThrowPlayer == ((b.myPosition + 3) & 3) && seaCard / 9 != 3) {
				/// moveArray: 跟seacard湊成順子的兩張牌，一張牌用6bits表示(size5: 最多5順子)
				/// listenTilesNum: 如果不吃的話要聽的牌數
				int index(0), moveArray[5], listenTilesNum[5] = { 0 }, validTileNum[5] = { 0 };
				bool ishaveEyesArray[5] = {};
				for (int i = 0; i < length; ++i) {	//Eat
					if (i + 1 < length && tempArray[i] == seaCard + 1 && tempArray[i + 1] == seaCard + 2
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // XOO /// 吃牌條件(34進2)
						table->UpdateTableID(b.tableID, tempArray[i], THROW); /// 試吃
						table->UpdateTableID(b.tableID, tempArray[i + 1], THROW);
						int listenNum(table->getTilesListenNum(4 - b.publicGroupNum, b.tableID, b.is_have_eyes)); /// 試吃完算進聽數
						table->UpdateTableID(b.tableID, tempArray[i], TAKE); /// 回復因為試吃丟牌的操作
						table->UpdateTableID(b.tableID, tempArray[i + 1], TAKE);
						if (listenNum < b.listenNum) { /// 若吃牌有助於減少進聽數
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1]; /// 把跟seaCard湊成組的牌寫入moveArray
							/// 計算如果不吃牌的話要聽多少牌
							if (tempArray[i + 1] % 9 != 8) { /// 原本聽14
								listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(tempArray[i + 1] + 1);
								listenTilesNum[index] += b.predictRemainTiles.getRemainTilesNum(seaCard);
							}
							else { /// 原本聽1
								listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(seaCard);
							}
							ishaveEyesArray[index] = b.is_have_eyes; /// 吃牌後是否有眼
							validTileNum[index] = table->getValidTiles(b.tableID, b.predictRemainTiles, b.is_have_eyes); /// 計算不吃牌的話有多少有效牌
							index++;
						}
					}
					else if (i + 1 < length && tempArray[i] == seaCard - 1 && tempArray[i + 1] == seaCard + 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OXO /// 24進3
						table->UpdateTableID(b.tableID, tempArray[i], THROW);
						table->UpdateTableID(b.tableID, tempArray[i + 1], THROW);
						int listenNum(table->getTilesListenNum(4 - b.publicGroupNum, b.tableID, b.is_have_eyes));
						table->UpdateTableID(b.tableID, tempArray[i], TAKE);
						table->UpdateTableID(b.tableID, tempArray[i + 1], TAKE);
						if (listenNum < b.listenNum) { /// 原本聽3
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
							listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(seaCard);
							ishaveEyesArray[index] = b.is_have_eyes;
							validTileNum[index] = table->getValidTiles(b.tableID, b.predictRemainTiles, b.is_have_eyes);
							index++;
						}
					}
					else if (i + 2 < length && tempArray[i] == seaCard - 1 && tempArray[i + 2] == seaCard + 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 2] / 9) { // OXO /// 234進3
						table->UpdateTableID(b.tableID, tempArray[i], THROW);
						table->UpdateTableID(b.tableID, tempArray[i + 2], THROW);
						int listenNum(table->getTilesListenNum(4 - b.publicGroupNum, b.tableID, b.is_have_eyes));
						table->UpdateTableID(b.tableID, tempArray[i], TAKE);
						table->UpdateTableID(b.tableID, tempArray[i + 2], TAKE);
						if (listenNum < b.listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 2];
							listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(seaCard);
							ishaveEyesArray[index] = b.is_have_eyes;
							validTileNum[index] = table->getValidTiles(b.tableID, b.predictRemainTiles, b.is_have_eyes);
							index++;
						}
					}
					else if (i + 1 < length && tempArray[i] == seaCard - 2 && tempArray[i + 1] == seaCard - 1
						&& seaCard / 9 == tempArray[i] / 9 && seaCard / 9 == tempArray[i + 1] / 9) { // OOX /// 23進4
						table->UpdateTableID(b.tableID, tempArray[i], THROW);
						table->UpdateTableID(b.tableID, tempArray[i + 1], THROW);
						int listenNum(table->getTilesListenNum(4 - b.publicGroupNum, b.tableID, b.is_have_eyes));
						table->UpdateTableID(b.tableID, tempArray[i], TAKE);
						table->UpdateTableID(b.tableID, tempArray[i + 1], TAKE);
						if (listenNum < b.listenNum) {
							moveArray[index] = tempArray[i] << 6 | tempArray[i + 1];
							if (tempArray[i] % 9 != 0) { /// 原本聽14
								listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(tempArray[i] - 1);
								listenTilesNum[index] += b.predictRemainTiles.getRemainTilesNum(seaCard);
							}
							else {
								listenTilesNum[index] = b.predictRemainTiles.getRemainTilesNum(seaCard);
							}
							ishaveEyesArray[index] = b.is_have_eyes;
							validTileNum[index] = table->getValidTiles(b.tableID, b.predictRemainTiles, b.is_have_eyes);
							index++;
						}
					}
				}
				if (index > 0) { /// 有順子
					int max = -1, pos = 0; /// max: 最多聽多少牌, pos: 該組順的位置
					for (int a = 0; a < index; ++a) {
						if (!ishaveEyesArray[pos] && ishaveEyesArray[a]) { /// 先選吃牌後有眼牌的
							max = validTileNum[a];
							pos = a;
						}
						/// 眼牌情況一樣，但是可能湊順子之後有效牌張數變多或聽牌數變多
						else if (ishaveEyesArray[pos] == ishaveEyesArray[a] &&
							(max < validTileNum[a] || (max == validTileNum[a] && listenTilesNum[pos] > listenTilesNum[a]))) {
							max = validTileNum[a];
							pos = a;
						}
					}

					b.takeTile = moveArray[pos] >> 6;
					b.privateHand.RemoveHand(moveArray[pos] >> 6); // Modified
					b.privateHand.RemoveHand(moveArray[pos] & 63); // Modified
					table->UpdateTableID(b.tableID, moveArray[pos] >> 6, THROW);
					table->UpdateTableID(b.tableID, moveArray[pos] & 63, THROW);
					b.publicGroupNum++;
					b.listenNum--;
					askEat = true;
				}
			}
		}

		// take tile
		if (!askPong && !askEat) { /// 玩家捨的牌我方無法吃碰
			if (b.status != b.myPosition) { /// 該輪非我方，不須摸牌
				seaCard = b.predictRemainTiles.TakeTile(); /// 下輪是隨機摸打
				if (b.predictRemainTiles.getRemainTotalTilesNum() <= 64 - oppGroup) {
					return 0; /// 流局
				}
				finalThrowPlayer = b.status; /// 更新前次捨牌玩家編號
				UpdateStatus(b); /// 換下一位玩家
				continue;
			}
			else { /// 該輪為我方，要摸牌
				int card = b.predictRemainTiles.TakeTile();
				b.privateHand.AddHand(card); // Modified
				b.takeTile = card;
				table->UpdateTableID(b.tableID, card, TAKE); /// 摸牌
				b.listenNum = table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, b.is_have_eyes); /// 摸牌後更新進聽數
				if (b.listenNum == -1) {
					return 3; /// 自摸
				}
				else if (b.predictRemainTiles.getRemainTotalTilesNum() <= 64 - oppGroup) {
					return 0; /// 流局
				}
			}
		}

		if (b.status == b.myPosition) { /// 該輪為我方
			int throwList[17] = { 0 };
			int throwArraySize(ThrowList(b, throwList)); /// 計算捨牌集合

			if (throwArraySize > 0) { /// 如果手牌及何不為空，就隨機從集合挑一張出來打
#ifdef __GNUC__
				pcg_extras::seed_seq_from<std::random_device> seed_source;
				pcg32 rng(seed_source);
#elif _MSC_VER
				std::random_device rd;
				std::mt19937 rng(rd());
#endif
				std::uniform_int_distribution<int> uniform_dist(0, throwArraySize - 1);
				int next = throwList[uniform_dist(rng)];
				seaCard = next; /// 寫到捨牌資訊中，下一次while loop該牌就會被捨棄
				finalThrowPlayer = b.status; /// 更新下輪玩家資訊
				MakeThrow(b, next, false); /// 把牌丟了
			}
			else { /// 如果捨牌集合為空(不合法，最後會throw exception)
				/*printf("publicGroupNum : %d\n", b.publicGroupNum);
				int tempArray[17] = { 0 }, tempNumArray[17] = { 0 }, throwTiles[17] = {0}, throwSize = 0;
				int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);
				bool isEyes = false;
				int bbbb = table->getTilesListenNum(5 - b.publicGroupNum, tempArray, tempNumArray, length, b.is_have_eyes);
				int aaaa = table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, b.is_have_eyes);
				for (int i = 0; i < length; ++i) { /// 找看看捨哪張手牌不會改變進聽數
					table->UpdateTableID(b.tableID, tempArray[i], THROW);
					int abc = table->getTilesListenNum(5 - b.publicGroupNum, b.tableID, isEyes);
					if ( abc == b.listenNum) {
						throwTiles[throwSize++] = tempArray[i]; /// 若捨牌不會改變進聽數，則將牌寫到throwTiles中
					}
					printf("%d %d %d %d\n", b.listenNum, abc, aaaa, bbbb); /// 復原嘗試捨牌的操作
					table->UpdateTableID(b.tableID, tempArray[i], TAKE);
				}
				for (int z = 0; z < throwSize; ++z) {
					printf("%d ", throwTiles[z]);
				}
					printf("\n");*/
				b.privateHand.ShowPrivateHandName();

				std::cout << "*****Line 2441 ThroList Empty*****\n" << std::flush; // Debug
				throw std::invalid_argument("ThrowList Empty.");
			}
		}


#elif (EAT_PONG_ENABLE == 0 && SMART_AI_ENABLE == 1)
		if (whos_turn(b) == b.myPosition) {
			int cards = aiBoard.predictRemainTiles.TakeTile();
			int tempArray[17] = { 0 };
			int tempNumArray[17] = { 0 };
			b.takeTile = cards;
			b.privateHand.AddHand(cards);
			int length = b.privateHand.getTileNumArray(tempArray, tempNumArray);
			if (isHu(b, tempArray, tempNumArray, length, b.takeTile, b.throwTile, b.publicGroupNum)) {
				b.status = b.status + 4;
			}
			else if (aiBoard.predictRemainTiles.getRemainTotalTilesNum() <= 16) {
				b.status = Draw;
			}
			if (is_terminal(b)) {
				if (who_won(b) == us) {
					return 1;
				}
				return 0;
			}
			int mL_size1 = ThrowList(b, false);
			if (mL_size1 > 0) {
#ifdef __GNUC__
				pcg_extras::seed_seq_from<std::random_device> seed_source;
				pcg32 rng(seed_source);
#elif _MSC_VER
				std::random_device rd;
				std::mt19937 rng(rd());
#endif
				std::uniform_int_distribution<int> uniform_dist(0, mL_size1 - 1);
				int next = throwArray[uniform_dist(rng)];
				MakeThrow(b, next, false);
			}
			else {
				b.privateHand.ShowPrivateHandName();
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 6; j++)
						printf("%d ", b.listenNumTable[i][j]);
					printf("\n");
				}
				printf("\n");
				throw std::invalid_argument("ThrowList Empty.");
			}
		}
		else {
			int cards = aiBoard.predictRemainTiles.TakeTile();
			int tempArray[17] = { 0 };
			int tempNumArray[17] = { 0 };
			privateHand_AI[b.status].AddHand(cards);
			int length = privateHand_AI[b.status].getTileNumArray(tempArray, tempNumArray);
			int originListenNum = table->getTilesListenNum(5 - publicHand[b.status].getGroupNum(), tempArray, tempNumArray, length);
			if (originListenNum == -1) {
				b.status = b.status + 4;
			}
			else if (aiBoard.predictRemainTiles.getRemainTotalTilesNum() <= 16) {
				b.status = Draw;
			}
			if (is_terminal(b)) {
				if (who_won(b) == us) {
					return 1;
				}
				return 0;
			}
			vector<int> throwArray;
			for (int i = 0; i < length; i++) {
				tempNumArray[i]--;
				int currentListenNum = table->getTilesListenNum(5 - publicHand[b.status].getGroupNum(), tempArray, tempNumArray, length);
				if (currentListenNum <= originListenNum) {
					throwArray.push_back(tempArray[i]);
				}
				tempNumArray[i]++;
			}
			if (!throwArray.empty()) {
#ifdef __GNUC__
				pcg_extras::seed_seq_from<std::random_device> seed_source;
				pcg32 rng(seed_source);
#elif _MSC_VER
				std::random_device rd;
				std::mt19937 rng(rd());
#endif
				std::uniform_int_distribution<int> uniform_dist(0, throwArray.size() - 1);
				int next = throwArray[uniform_dist(rng)];
				b.predictRemainTiles.AddCard(next);
				privateHand_AI[b.status].RemoveHand(next);
				UpdateStatus(b);
			}
			else {
				printf("origin: %d %d %d\n", originListenNum, 5 - publicHand[b.status].getGroupNum(), b.status);
				for (int i = 0; i < 4; i++) {
					printf("%d\n", publicHand[i].getGroupNum());
				}
				privateHand_AI[b.status].ShowPrivateHandName();
				table->getTilesListenNum(5 - publicHand[b.status].getGroupNum(), tempArray, tempNumArray, length, b.listenNumTable, -1, -1);
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 6; j++)
						printf("%d ", b.listenNumTable[i][j]);
					printf("\n");
				}
				printf("\n");
				throw std::invalid_argument("AI ThrowList Empty.");
			}
		}

#elif (EAT_PONG_ENABLE == 0 && SMART_AI_ENABLE == 0)
		if (whos_turn(b) != b.myPosition) {
			int cards = b.predictRemainTiles.TakeTile();
			//b.takeTile = cards;
			if (b.predictRemainTiles.getRemainTotalTilesNum() <= 64 - oppGroup) {
				return 0;
			}
			MakeThrow(b, cards, false);
		}
		else {
			MakeTake(b, b.myPosition, TAKE);
			if (is_terminal(b)) {
				if (who_won(b) == b.myPosition) {
					return 1;
				}
				return 0;
			}
			int throwArray[17] = { 0 };
			int throwArraySize = ThrowList(b, throwArray);
			if (throwArraySize > 0) {
#ifdef __GNUC__
				pcg_extras::seed_seq_from<std::random_device> seed_source;
				pcg32 rng(seed_source);
#elif _MSC_VER
				std::random_device rd;
				std::mt19937 rng(rd());
#endif
				std::uniform_int_distribution<int> uniform_dist(0, throwArraySize - 1);
				int next = throwArray[uniform_dist(rng)];
				MakeThrow(b, next, false);
			}
			else {
				b.privateHand.ShowPrivateHandName();
				throw std::out_of_range("ThrowList Empty.");
			}
		}
#endif
	} // End while
	std::cout << "*****Line 2592 Simulation expection*****\n" << std::flush; // Debug
	throw std::invalid_argument("Simulation expection.");
}

inline int testSim() { // Debug
	return 1; // Debug
}

double AI::UCB(const int& nodeIndex, const std::atomic<int>& treeSimTimes) {
	return nodes[nodeIndex].averageScore + _C_ * sqrt(log((double)treeSimTimes) / nodes[nodeIndex].totalSimTimes);
}

void AI::backwardPropagation(const int& startIndex, const double& newAvg, const int& newSimTimes) {
	int ptr = startIndex;
	while (ptr != 0) {
		int originalSimTimes = nodes[ptr].totalSimTimes;
		nodes[ptr].totalSimTimes += newSimTimes;
		nodes[ptr].averageScore = (newSimTimes * newAvg) + (originalSimTimes * nodes[ptr].averageScore);
		nodes[ptr].averageScore /= nodes[ptr].totalSimTimes;
		ptr = nodes[ptr].parentIndex;
	}
}

inline void AI::MCTS(const Board& currentBoard, const int* throwList, const int& throwListSize) {
	//while (true) { // Debug
		//std::cout << "new round\n"; // Debug
		//for (int i = 0; i < TT_SIZE; i++) tt[i].simNum = 0; // Debug
		// Check if throwList is empty or not
	if (throwListSize == 0) throw std::invalid_argument("In MCTS: ThrowList of current board is empty,\n");

	unsigned int treeSimTimes = 0;

	// For TT
	boost::multiprecision::uint256_t queryKey; // Modified
	unsigned int hashIndex; // Modified
	short specificTileNum;
#ifdef LOG_COLLISION // Modified
	unsigned int hitNum = 0, clsNum = 0;  // Modified
#endif

	// Maintain throwIndex table: thrownTile[throwntile] = index in throwList
	int thrownTileToIndex[34] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	for (int i = 0; i < throwListSize; i++)
		thrownTileToIndex[throwList[i]] = i;

	// Initialize root: nodes[0]
	nodes[0].averageScore = 0;
	nodes[0].childrenNum = throwListSize;
	nodes[0].depth = 0;
	nodes[0].drawnTile = -1;
	nodes[0].isTerminateNode = false;
	nodes[0].parentIndex = -1;
	nodes[0].throwTile = -1;
	nodes[0].totalSimTimes = 0;

	// First-layer expansion, simulation, and backward propagation
	for (int i = 0; i < throwListSize; i++) {
		// Initialize nodes[1 + i]
		nodes[0].childIndex[i] = i + 1;
		nodes[i + 1].averageScore = 0;
		nodes[i + 1].childrenNum = 0;
		nodes[i + 1].depth = 1;
		nodes[i + 1].drawnTile = -1;
		nodes[i + 1].isTerminateNode = false;
		nodes[i + 1].parentIndex = 0;
		nodes[i + 1].throwTile = throwList[i];
		nodes[i + 1].totalSimTimes = 0;

		// Genrate the key that represent the board without throwList[i]
		specificTileNum = currentBoard.privateHand.getTileNum(throwList[i]); // Modified
		queryKey = hashKey ^ tileNumHash[(throwList[i] << 2) + specificTileNum - 1]; // Modified
		if (specificTileNum > 1) queryKey ^= tileNumHash[(throwList[i] << 2) + specificTileNum - 2]; // Modified
		hashIndex = (unsigned int)(queryKey & mask);
		if (tt[hashIndex].simNum >= SIMULATION_NUM && tt[hashIndex].key == queryKey) { // Hit
#ifdef LOG_COLLISION
			hitNum++; // Modified
#endif
			nodes[i + 1].totalSimTimes = tt[hashIndex].simNum; // Modified
			nodes[i + 1].averageScore = (double)tt[hashIndex].simScore / tt[hashIndex].simNum; // Modified

			treeSimTimes += tt[hashIndex].simNum; // Modified
		}
		else { // Modified
			if (tt[hashIndex].simNum > 0 && tt[hashIndex].key != queryKey) { // Modified
#ifdef LOG_COLLISION
				hitNum++; // Modifed 
				clsNum++; // Modified
#endif
			}

			// Simulation
			std::atomic<int> nodeScores = 0, nodeSimTimes = 0;
			pool.parallelize_loop(0, SIMULATION_NUM, [this, &currentBoard, &throwList, &i, &nodeScores, &nodeSimTimes, &queryKey](const int& q, const int& w) {
				for (int a = q; a < w; ++a) {
					nodeScores += Simulate(currentBoard, throwList[i]);
					nodeSimTimes++;
				}
				});

			// Upadate first-depth node
			nodes[i + 1].totalSimTimes += nodeSimTimes;
			nodes[i + 1].averageScore = (double)nodeScores / nodeSimTimes;

			treeSimTimes += nodeSimTimes;

			// Update tt
			unsigned long long hashIndex = (unsigned long long)(queryKey & mask); // Modified
			if (tt[hashIndex].simNum == 0) {
				tt[hashIndex].key = queryKey; // Modified
				tt[hashIndex].simNum = nodeSimTimes; // Modified
				tt[hashIndex].simScore = nodeScores; // Modified
			}
			else if (tt[hashIndex].key == queryKey) {
				tt[hashIndex].simNum += nodeSimTimes; // Modified
				tt[hashIndex].simScore += nodeScores; // Modified
			}
		}
	}
	// Generalize the other layers
	int nodesSize = throwListSize + 1; // Current number of elements of nodes[]
	short int selectionCnt = 1; // Count the number of repeated selections

	boost::multiprecision::uint256_t maxLeafKey, drawLayerKey; // Modified
	while (selectionCnt++ < SELECTION_NUM) {
		// Selection: Select the path based on UCB
		int ptr = 0; // Index of root (Selection starts from root)

		Board maxLeaf = currentBoard;
		maxLeaf.status = currentBoard.myPosition;
		maxLeafKey = hashKey; // Modified
		int maxLeafIndex = -1;
		while (nodes[ptr].childrenNum > 0) { // Trace to leaf node
			double maxUCB = -1;
			for (int i = 0; i < nodes[ptr].childrenNum; i++) {
				double nodeUCB = UCB(nodes[ptr].childIndex[i], treeSimTimes);
				if (nodeUCB > maxUCB) {
					maxUCB = nodeUCB;
					maxLeafIndex = nodes[ptr].childIndex[i];
				}
			}
			if (nodes[maxLeafIndex].drawnTile != -1) { // Depth > 1
				maxLeaf.predictRemainTiles.TakeSpecificTile(nodes[maxLeafIndex].drawnTile);
				specificTileNum = maxLeaf.privateHand.getTileNum(nodes[maxLeafIndex].drawnTile); // Modified
				if (specificTileNum > 0) maxLeafKey ^= tileNumHash[(nodes[maxLeafIndex].drawnTile << 2) + specificTileNum - 1]; // Modified
				maxLeaf.privateHand.AddHand(nodes[maxLeafIndex].drawnTile); // Modified
				maxLeafKey ^= tileNumHash[(nodes[maxLeafIndex].drawnTile << 2) + specificTileNum]; // Modified
				table->UpdateTableID(maxLeaf.tableID, nodes[maxLeafIndex].drawnTile, TAKE);
			}
			if (nodes[maxLeafIndex].throwTile != -1) { // !Leaf node
				specificTileNum = maxLeaf.privateHand.getTileNum(nodes[maxLeafIndex].throwTile); // Modified
				maxLeafKey ^= tileNumHash[(nodes[maxLeafIndex].throwTile << 2) + specificTileNum - 1]; // Modified
				MakeThrow(maxLeaf, nodes[maxLeafIndex].throwTile, false);
				if (specificTileNum > 1) maxLeafKey ^= tileNumHash[(nodes[maxLeafIndex].throwTile << 2) + specificTileNum - 2]; // Modified
			}
			ptr = maxLeafIndex;
			maxLeaf.status = currentBoard.myPosition;
		}

		// Check if maxLeaf is terminate node or not
		if (nodes[maxLeafIndex].isTerminateNode == true) {
			nodes[maxLeafIndex].totalSimTimes += SIMULATION_NUM;
			backwardPropagation(nodes[maxLeafIndex].parentIndex, 3, SIMULATION_NUM);
			continue;
		}

		// Expansion
		int drawClasses[34] = { 0 }, drawNum = maxLeaf.predictRemainTiles.getRemainTilesClasses(drawClasses);
		double expectedScore = 0;
		int maxLeafSimTimes = 0;


		for (int drawIndex = 0; drawIndex < drawNum; drawIndex++) {
			Board drawLayerChld = maxLeaf;
			drawLayerChld.status = maxLeaf.myPosition;
			drawLayerKey = maxLeafKey; // Modified

			// Calculate the probability of drawing the card
			double drawProbability = (double)drawLayerChld.predictRemainTiles.getRemainTilesNum(drawClasses[drawIndex]) / drawLayerChld.predictRemainTiles.getRemainTotalTilesNum();

			// Take the tile and update drawLayerChld info
			drawLayerChld.predictRemainTiles.TakeSpecificTile(drawClasses[drawIndex]);
			specificTileNum = drawLayerChld.privateHand.getTileNum(drawClasses[drawIndex]); // Modified
			if (specificTileNum > 0) drawLayerKey ^= tileNumHash[(drawClasses[drawIndex] << 2) + specificTileNum - 1]; // Modified
			drawLayerChld.privateHand.AddHand(drawClasses[drawIndex]); // Modified
			drawLayerKey ^= tileNumHash[(drawClasses[drawIndex] << 2) + specificTileNum]; // Modified
			table->UpdateTableID(drawLayerChld.tableID, drawClasses[drawIndex], TAKE);
			drawLayerChld.listenNum = table->getTilesListenNum(5 - drawLayerChld.publicGroupNum, drawLayerChld.tableID, drawLayerChld.is_have_eyes);

			// Check if self drawinf or not
			if (drawLayerChld.listenNum == -1) {
				// Expand a terminal node
				nodes[maxLeafIndex].childIndex[(nodes[maxLeafIndex].childrenNum++)] = nodesSize;
				if (nodes[maxLeafIndex].childrenNum >= 578) throw std::invalid_argument("In MCTS: childIndex out of range - 1\n");
				nodes[nodesSize].averageScore = 3;
				nodes[nodesSize].childrenNum = 0;
				nodes[nodesSize].depth = nodes[maxLeafIndex].depth + 1;
				nodes[nodesSize].drawnTile = drawClasses[drawIndex];
				nodes[nodesSize].isTerminateNode = true;
				nodes[nodesSize].parentIndex = maxLeafIndex;
				nodes[nodesSize].throwTile = -1;
				nodes[nodesSize].totalSimTimes = SIMULATION_NUM;

				// Backward propagation
				backwardPropagation(maxLeafIndex, 3, SIMULATION_NUM);

				nodesSize++;
				continue;
			}

			// Calculate maxLeaf's throwList
			int drawLayerThrowList[17] = { 0 }, maxLeafThrowSize = ThrowList(drawLayerChld, drawLayerThrowList);

			// Check if maxLeaf's throw is empty or not
			if (maxLeafThrowSize == 0) throw std::invalid_argument("In MCTS: ThrowList of maxLeaf is empty.\n");

			int expandedChldrenScores = 0, expandedChldrenSimtimes = 0; // Record total scores and total simTimes
			for (int i = 0; i < maxLeafThrowSize; i++) {
				if (nodes[maxLeafIndex].childrenNum >= 578) throw std::invalid_argument("In MCTS: childIndex out of range - 2\n");
				nodes[nodesSize].averageScore = 0;
				nodes[nodesSize].childrenNum = 0;
				nodes[nodesSize].depth = nodes[maxLeafIndex].depth + 1;
				nodes[nodesSize].drawnTile = drawClasses[drawIndex];
				nodes[nodesSize].isTerminateNode = false;
				nodes[nodesSize].parentIndex = maxLeafIndex;
				nodes[nodesSize].throwTile = drawLayerThrowList[i];
				nodes[nodesSize].totalSimTimes = 0;

				// Check tt
				specificTileNum = drawLayerChld.privateHand.getTileNum(drawLayerThrowList[i]); // Modified
				queryKey = drawLayerKey ^ tileNumHash[(drawLayerThrowList[i] << 2) + specificTileNum - 1]; // Modified
				if (specificTileNum > 1) queryKey ^= tileNumHash[(drawLayerThrowList[i] << 2) + specificTileNum - 2]; // Modified
				nodes[maxLeafIndex].childIndex[(nodes[maxLeafIndex].childrenNum++)] = nodesSize;
				hashIndex = (unsigned int)(queryKey & mask); // Modified
				if (tt[hashIndex].simNum >= SIMULATION_NUM && tt[hashIndex].key == queryKey) { // Hit // Modified
#ifdef LOG_COLLISION
					hitNum++; // Modified
#endif
					nodes[nodesSize].totalSimTimes += tt[hashIndex].simNum; // Modified
					nodes[nodesSize].averageScore = (double)tt[hashIndex].simScore / tt[hashIndex].simNum; // Modified

					expandedChldrenScores += tt[hashIndex].simScore; // Modified
					expandedChldrenSimtimes += tt[hashIndex].simNum; // Modified

					treeSimTimes += tt[hashIndex].simNum; // Modified
				}
				else {
					if (tt[hashIndex].simNum > 0 && tt[hashIndex].key != queryKey) { // Collision // Modified
#ifdef LOG_COLLISION
						hitNum++; // Modified
						clsNum++; //Modified
#endif
					}

					std::atomic<int> nodeScores = 9, nodeSimTimes = 0;
					pool.parallelize_loop(0, SIMULATION_NUM, [this, &drawLayerChld, &drawLayerThrowList, &i, &nodesSize, &nodeScores, &nodeSimTimes](const int& q, const int& w) {
						int oppThrowNum = 3 * (nodes[nodesSize].depth - 1);
						if (drawLayerChld.predictRemainTiles.getRemainTotalTilesNum() - oppThrowNum <= 64 - oppGroup) { // Draw
							nodeSimTimes += (w - q);
							return;
						}
						for (int a = q; a < w; ++a) {
							Board expandedChild = drawLayerChld;
							expandedChild.status = drawLayerChld.myPosition;
							// Randomly throw tiles (opponents' thrown cards)
							for (int oppThrowCnt = 0; oppThrowCnt < oppThrowNum; oppThrowCnt++)
								expandedChild.predictRemainTiles.TakeTile();

							nodeScores += Simulate(expandedChild, drawLayerThrowList[i]);
							nodeSimTimes++;
						}
						});

					treeSimTimes += nodeSimTimes; // Modified

					// Update tt
					if (tt[hashIndex].simNum == 0) {
						tt[hashIndex].key = queryKey; // Modified
						tt[hashIndex].simNum = nodeSimTimes; // Modified
						tt[hashIndex].simScore = nodeScores; // Modified
					}
					else if (tt[hashIndex].key == queryKey) {
						tt[hashIndex].simNum += nodeSimTimes; // Modified
						tt[hashIndex].simScore += nodeScores; // Modified
					}


					nodes[nodesSize].totalSimTimes += nodeSimTimes;
					nodes[nodesSize].averageScore = (double)nodeScores / nodeSimTimes;

					expandedChldrenScores += nodeScores;
					expandedChldrenSimtimes += nodeSimTimes;
				}

				nodesSize++;
				if (nodesSize >= SHRT_MAX) throw std::invalid_argument("In MCTS: Size of nodes[] is too small.\n");
			}
			maxLeafSimTimes += expandedChldrenSimtimes;
			expectedScore += drawProbability * (double)expandedChldrenScores / expandedChldrenSimtimes;
		}
		backwardPropagation(maxLeafIndex, expectedScore, maxLeafSimTimes);
	}
	for (int i = 0; i < throwListSize; i++) nodes[1 + i].throwTile = thrownTileToIndex[nodes[1 + i].throwTile];
	treeSize = nodesSize;

#ifdef LOG_COLLISION
	std::ofstream clsLog("clsLog.csv", std::ios::app); // Modified
	clsLog << clsNum << "," << hitNum << "\n"; // Modified
	clsLog.close(); // Modified
#endif
	//}
}


//-----------------------------  Tools  -----------------------------------

int AI::Partition(int* arr, int front, int end) {
	float pivot = nodes[nodes[0].childIndex[arr[end]]].averageScore;
	int i = front - 1;
	for (int j = front; j < end; ++j) {
		if (nodes[nodes[0].childIndex[arr[j]]].averageScore > pivot) {
			i++;
			std::swap(arr[i], arr[j]);
		}
	}
	i++;
	std::swap(arr[i], arr[end]);
	return i;
}

void AI::QuickSort(int* arr, int front, int end) {
	if (front < end) {
		int pivot = Partition(arr, front, end);
		QuickSort(arr, front, pivot - 1);
		QuickSort(arr, pivot + 1, end);
	}
}