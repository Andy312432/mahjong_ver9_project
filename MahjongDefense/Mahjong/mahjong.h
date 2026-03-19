#pragma once

#ifndef _maghjong_H_
#define _maghjong_H_
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "Table.h"
//#include "Game.h"

#define DATANUM 17 //自定義資料

class Mahjong {

private:
	char tileType[5] = { 'w', 't', 's', 'z', 'f' };
	char result[200] = { '\0' };

	//int data[17]= { 1,2,3,4,5,6,10,11,11,11,20,21,21,22,23,25,26}; //17
	//int data[DATANUM] = { 0,0,0,1,2,3,4,5,6,7,10,10,10,11,12,13,14,28 }; //自定義資料 18
	//int data[DATANUM] = { 0,0,1,2,9,10,11,12,13, 21,25,25,25,26}; //14
	int data[DATANUM] = { 2,3,4,11,12,13,14,15,16,21,22,23,27,27,27,28,28 }; //17，胡牌 /// 一個玩家所有的牌
	int _pileArray[17];	//紀錄牌型，不重複
	int _pileNumArray[17] = { 0 }; //紀錄此牌有幾張
	int* tile = new int[34]; /// 所有牌種共34種

	int randomized_search(int* array, int n, int num);
	int partition(int number[], int left, int right);

	void QuickSort(int number[], int left, int right);
	void FindSequence(int pileArray[], int pileNumArray[], int length, bool first, char string[], int start);
	void Find3(int pileArray[], int pileNumArray[], int length, bool first, char string[]);
	void FindOne(int pileArray[], int pileNumArray[], int length, char string[]);
	void FindPair(int pileArray[], int pileNumArray[], int length, char string[]);
	void FindHole(int pileArray[], int pileNumArray[], int length, char string[]);
	void FindListenHole(char string[]);
	void ReGroup(char string[]);

public:
	void Test();
	void HandTileAnalysis();
	void RandomTile();
};
#endif
