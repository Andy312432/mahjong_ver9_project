#pragma once

#ifndef __TABLE_H__
#define __TABLE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "PredictRemainTiles.h"

#define LISTEN_TABLE_SIZE 25390625		// MAX_ID * 13
#define LISTEN_TABLE_CHAR_SIZE 1015625	// MAX_WORD_ID * 13

#define THROW_TABLE_SIZE 2557544
#define THROW_TABLE_WORD_SIZE 64199
#define THROW_TABLE_EYE_SIZE 2080965

#define VALID_TABLE_SIZE 3656518
#define VALID_TABLE_WORD_SIZE 165998
#define VALID_TABLE_EYE_SIZE 3154112
#define VALID_TABLE_WORD_EYE_SIZE 142212

using std::vector;
using std::unordered_map;

struct ThrowSet {
	unsigned char one[7] = { 0 };
	unsigned char pair[7] = { 0 };
	unsigned char tatsu[6] = { 0 };
	unsigned char oneSize = 0;
	unsigned char pairSize = 0;
	unsigned char tatsuSize = 0;	
};


class Table {
private:
	// Length: 2557544, 64199, 2080965, 64199
	ThrowSet *dismantlingTableResult = nullptr, *dismantlingTableResultWord = nullptr, *dismantlingTableResult_Eye = nullptr, *dismantlingTableResultWord_Eye = nullptr;

	vector<vector<vector<int>>> dismantlingResult;

	int* _card = nullptr, * _cardWord = nullptr;
	int *dismantlingSize = nullptr, *dismantlingWordSize = nullptr, *dismantlingEyeSize = nullptr, *dismantlingWordEyeSize = nullptr;	
	int* validTiles_Size = nullptr, * validTilesChar_Size = nullptr, * validTiles_Eye_Size = nullptr, * validTilesChar_Eye_Size = nullptr;
	/// validTiles_Size該花色(from tableID)有幾張有效牌, 

	char* validTiles = nullptr, * validTilesChar = nullptr, * validTiles_Eye = nullptr, * validTilesChar_Eye = nullptr;
	/// validTiles: 可下表，看validTiles_Size的大小決定第幾格到第幾格為其可下牌種

	const char kListenTableName[50] = "ListenTable.bin";
	const char kDismantlingTableName[50] = "DismantlingTable.bin";
	const char kDismantlingListenTableName[50] = "DismantlingTable (Listen).bin";
	const char kValidTileTableName[50] = "ValidTileTable.bin";

	const unsigned int powFive[10] = { 1, 5, 25, 125, 625, 3125, 15625, 78125, 390625, 1953125 };
	const unsigned int MAX_ID = powFive[9];
	const unsigned int MAX_WORD_ID = powFive[7];

	int caculateNum = 9999; // for generate table

	bool haveEyes = false;	// for generate table

	unordered_map<std::string, bool> u_map;

	// ======= Listen Table ========================================
	//int caculate(int *pileArray, int *pileNumArray, int group, int length, bool word);
	int cfileexists(const char* filename);

	void FindSequence(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g);
	void Find3(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g, bool word);
	void FindOne(int* pileArray, int* pileNumArray, int length, int group, int two, int one, int g, int eyes);
	void FindPair(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g, int eyes, bool word);
	void FindHole(int* pileArray, int* pileNumArray, int length, int group, int two, int one, int g, int eyes);
	void GenerateListenTable(FILE* fp);

	// ======= Dismantling Table ========================================
	inline std::string vectorToString(vector<vector<int>> result);

	// non-recussive dfs
	inline int FindDismantlingGroup(ThrowSet* throwSet, ThrowSet* dismantlingSearchResult, int* throwSetPosition, int* typeArray, int* startPosition, int& typeArraySize, int throwContentType[][4]) const;
	inline vector<vector<vector<int>>> FindDismantlingGroup(ThrowSet* throwSet, int* throwSetPosition, int* typeArray, int& typeArraySize) const;
	inline void FindSequence(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, bool first, int start);
	inline void Find3(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, bool first, int start);
	inline void FindPair(int* pileArray, int* pileNumArray, int length, vector<vector<int>> result);
	inline void FindHole(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, int start1, int start2);
	inline void FindOne(int* pileArray, int* pileNumArray, int length, vector<vector<int>> result);

	void GenerateDismantlingTable(FILE* fp);

	// ======= Dismantling Table (Listen) ========================================
	vector<vector<vector<int>>> dismantlingListenTableResult, dismantlingListenTableResultWord;

	void GenerateDismantlingListenTable(FILE* fp);
	// ======= Valid Tiles Table ========================================
	int maxGroup = 0; /// 最多可以湊幾組

	inline void FindSequence(int* pileArray, int* tempNumArray, int length, int group, bool haveEye, int start);
	inline void Find3(int* pileArray, int* tempNumArray, int length, int group, bool haveEye, int start);
	inline void FindPair(int* pileArray, int* pileNumArray, int length, int group);

public:
	Table();
	~Table();
	// ======= Listen Table ========================================
	int initListenTable();
	int caculate(int* pileArray, int* pileNumArray, int group, int length, bool word);
	int getTilesListenNum(const int& group, int* pileArray, int* pileNumArray, const int& length, bool& is_have_eyes);
	int getTilesListenNum(const int& group, int* tableID, int* listenRecordGroup, bool& is_have_eyes);
	inline int getTilesListenNum(const int& group, int* tableID, bool& is_have_eyes);
	// ======= Dismantling Table ========================================
	int initDismantlingTable();
	inline vector<vector<vector<int>>> findThrowlist(int* tempArray, int* tempNumArray, const int& length, bool eyeTable);
	int getDismantling(int* tableID, bool& is_have_eyes, ThrowSet* dismantlingSearchResult, int* startPosition, int throwContentType[][4]) /*const*/;
	// ======= Dismantling Table (Listen) ========================================
	int initDismantlingListenTable();
	vector<int> getListenDismantling(int* pileArray, int* pileNumArray, const int& length, vector<int>& recordGroup, vector<vector<int>>& result_diff);
	vector<int> getListenDismantling(int* tableID, vector<int>& listenRecordGroup, bool& is_have_eyes, const int& takeTile, const int& throwTile);
	// ======= Valid Tiles Table ===================================
	int initValidTileTable();
	void GenerateValidTileTable(FILE* fp);
	int getValidTiles(int* tableID, bool& is_have_eyes) const;
	int getValidTiles(int* tableID, PredictRemainTiles& predict, bool& is_have_eyes) const;
	int getValidTiles(const int* tableID, int* validArray, const bool& is_have_eyes) const;
	// ======= Tools ===================================
	void UpdateTableID(int* tableID, const int& card, const int& type);
	void UpdateTableID(int* pileArray, int* pileNumArray, const int& length, int* tableID);
};

#endif