//#pragma once
//
//#ifndef __GAME_H__
//#define __GAME_H__
//
//#include <iostream>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>
//
//#include "WallTiles.h"
//#include "Player.h"
//#include "AI.h"
//#include "Human.h"
//#include "GameStatue.h"
//
//using namespace std;
//
//#define GAME_ROUND 500
//
//class Anylsis {
//public:
//	void InitGame();
//	void Run_Analysis();
//private:
//	Table table;	
//};
//
//class Game {
//private:
//	Player* player = NULL;
//	WallTiles wallTiles;
//	PrivateHand privateHand[4];
//	PublicHand publicHand[4];
//	Table table;
//
//	int bank; // first player
//	int finalThrowPlayer; // last throw tile player
//	int status;	// current game status
//	int takeTile = 0;
//	int throwTile = 0;
//
//	//int bank; //莊家
//	int currentPlayer = 0;
//	int round = 0;
//	int flower[4] = { 0 };
//
//	std::ofstream out;
//
//	char tileType[5] = { 'W', 'S', 'B', 'Z', 'F' };
//
//	inline bool isCanEat(int* tileArray, int* tileNumArray, int length, int seaCard);
//	inline bool isCanPong(int* tileArray, int* tileNumArray, int length, int card);
//	inline bool isCanGong(int* tileArray, int* tileNumArray, int length, int card);
//
//	void ShowStage();
//public:
//	bool end = false;
//
//	int whos_turn();
//	int who_won();
//
//	void Init(bool haveFlower);
//	void FlowerDetect(int player, std::vector<int>& cards);
//	void SaveGameRecord();
//	void GameSetup();
//	void Main();
//	void MakeTake(int player, int action);
//};
//
//#endif