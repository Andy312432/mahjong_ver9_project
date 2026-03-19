////======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
////
//// Purpose: Built-in Mahjong server.
////
////=============================================================================
//
//#include <iostream>
//#include <fstream>
//#include <cctype>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <chrono>
//
//#include "ControllerTest.cpp"
//#include "ControllerTest.h"
//#include "Game.h"
//
//#define LISTEN_DEBUG 0
//#define DISMANTLING_DEBUG 1
//#define LISTEN_THROW_DEBUG 0
//#define VALID_TILE_DEBUG 0
//
//void Game::Init(bool haveFlower) {
//	wallTiles.Init(true);
//	bank = rand() % 4;
//
//	// flower tiles initial
//	if (haveFlower)
//		memset(flower, 0, sizeof(flower));
//
//	int start = 0;
//	// hand initial
//	for (int i = 0; i < 4; i++) {
//		vector<int> cards = wallTiles.TakeInitHandArray(start, start + 15);
//		if (haveFlower) {
//			FlowerDetect(i, cards);
//		}
//		privateHand[i].SetHand(cards);
//		//privateHand[i].ShowPrivateHand();
//		start += 16;
//	}
//
//	if (bank == PLAYER1) {
//		status = OnePlay;
//	}
//	else if (bank == PLAYER2) {
//		status = TwoPlay;
//	}
//	else if (bank == PLAYER3) {
//		status = ThreePlay;
//	}
//	else if (bank == PLAYER4) {
//		status = FourPlay;
//	}
//	round = 1;
//} // Modified
//// flower tiles detect
//void Game::FlowerDetect(int player, vector<int>& cards) {
//	for (int i = 0; i < cards.size(); i++) {
//		while (cards.at(i) / 9 == 4) {
//			cards.at(i) = takeTile;//wallTiles.TakeTile(); 
// 
//			flower[player]++;
//		}
//	}
//} // Modified
//
//void GameRecordInit() {
//	FILE* _fp = fopen("Game.log", "wb");
//	time_t t = time(0);
//	char buff[21];
//	strftime(buff, 21, "%Y-%m-%d %H:%M:%S.", localtime(&t));
//
//	char bit = '#';
//	char line = '\n';
//	fwrite(&bit, 1, 1, _fp);
//	fwrite(&buff, sizeof(buff), 1, _fp);
//	fprintf(_fp, "\n\n");
//	fclose(_fp);
//}
//
//void Game::SaveGameRecord() {
//	out << "Round: " << round << std::endl;
//	out << "Turn: Player" << whos_turn() + 1 << std::endl;
//	out << "Take: " << takeTile % 9 + 1 << tileType[takeTile / 9] << std::endl;
//	out << "Throw: " << throwTile % 9 + 1 << tileType[throwTile / 9] << std::endl;
//	out << "Statue: " << getStatusName[status] << std::endl;
//	for (int i = 0; i < 4; i++) {
//		out << "Player%d: " << i + 1;
//		//privateHand[i].ShowPrivateHand();
//	}
//	out << "Tile Sea: ";
//	/*char * string = wallTiles.getAllTileSea();
//	out << string << std::endl;
//
//	delete(string);*/
//}
//
//void Game::ShowStage() {
//	out << "Round: " << round << std::endl;
//	out << "Turn: " << getPlayerName[whos_turn()] << std::endl;
//	out << "Take: " << takeTile % 9 + 1 << tileType[takeTile / 9] << std::endl;
//	out << "Throw: " << throwTile % 9 + 1 << tileType[throwTile / 9] << std::endl;
//	out << "GroupNum: " << publicHand[whos_turn()].getGroupNum() << std::endl;
//	out << "Statue: " << getStatusName[status] << std::endl;
//
//	printf("Round: %d\n", round);
//	//printf("Bank is Player%d\n", bank + 1);
//	printf("Turn: %s\n", getPlayerName[whos_turn()]);
//	printf("Take: %d%c\n", takeTile % 9 + 1, tileType[takeTile / 9]);
//	printf("Throw: %d%c\n", throwTile % 9 + 1, tileType[throwTile / 9]);
//	printf("GroupNum: %d\n", publicHand[whos_turn()].getGroupNum());
//	printf("Statue: %s\n", getStatusName[status]);
//	for (int i = 0; i < 4; i++) {
//		out << "Player" << i + 1 << ": ";
//		printf("Player%d: ", i + 1);
//		vector<int> _pri = privateHand[i].getTiles();
//		for (int i = 0; i < _pri.size(); i++) {
//			printf("%d%c ", _pri[i] % 9 + 1, tileType[_pri[i] / 9]);
//			out << _pri[i] % 9 + 1 << tileType[_pri[i] / 9] << " ";
//		}
//		out << std::endl;
//		printf("\n");
//		//privateHand[i].ShowPrivateHand();
//	}
//	/*out << "Tile Sea: ";
//	printf("Tile Sea: ");
//	vector<int> ts = wallTiles.getAllTileSea();
//	for (int i = 0; i < ts.size(); i++)
//	{
//		printf("%d%c ", ts[i] % 9 + 1, tileType[ts[i] / 9]);
//		out << ts[i] % 9 + 1 << tileType[ts[i] / 9] << " ";
//	}
//	//wallTiles.ShowTileSea();
//	out << std::endl << std::endl;*/
//	printf("\n\n");
//}
//
//void Game::GameSetup() {
//
//	int simNum = GAME_ROUND, win = 0, lose = 0, draw = 0;
//
//	// Game record
//	out.open("Game.log", std::ios::app);
//	GameRecordInit();
//	while (simNum--) {
//		out << "Game Times: " << GAME_ROUND - simNum << std::endl;
//		Init(false);
//		Main();
//
//		if (who_won() == 0) {
//			win++;
//			out << getPlayerName[who_won()] << " is Win." << std::endl << std::endl;
//		}
//		else if (who_won() == 4) {
//			draw++;
//			out << "Draw." << std::endl << std::endl;
//		}
//		else
//			lose++;
//		//memset(nodes, 0, sizeof(nodes));
//		//printf("Average Spend Time %f, Max Time: %f\n", totalSpendTime / myTurnCount, maxTime);
//		//out << "Average Spend Time " << totalSpendTime / myTurnCount << ", Max Time : " << maxTime << std::endl;
//	}
//	printf("Win: %d, Lose: %d, Draw: %d\n", win, lose, draw);
//	out << "Win: " << win << ", Lose:" << lose << ", Draw:" << draw << std::endl;
//	out.close();
//}
//
//void Game::Main() {
//	int gameNum = GAME_ROUND;
//	Player* player = NULL;
//
//	Init(false);
//}
//
//void Anylsis::InitGame() {
//	if (table.initListenTable() == 0) {
//		cout << "***** Listen Table generate error. Please restart the program! *****" << endl;
//	}
//	if (table.initDismantlingTable() == 0) {
//		cout << "***** Dismantling Table generate error. Please restart the program! *****" << endl;
//	}
//	if (table.initValidTileTable() == 0) {
//		cout << "***** ValidTile Table generate error. Please restart the program! *****" << endl;
//	}
//}
//
//void Anylsis::Run_Analysis() {
//	string filename("Game/board.txt");
//	vector<string> lines;
//	string line;
//
//	ifstream input_file(filename);
//	if (!input_file.is_open()) {
//		cerr << "Could not open the file - '" << filename << "'" << endl;
//	}
//	while (getline(input_file, line)) {
//		if (line[0] == '*') {
//			continue;
//		}
//		else if (line[0] == '\n') {
//			break;
//		}
//		else {
//			lines.push_back(line);
//		}
//	}
//
//	input_file.close();
//	ControllerTest controller(lines);
//
//	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);
//
//	int command, gameCount = 0;
//	vector<int> data;
//	for (int i = 3; i < lines.size(); i++) {
//		controller.getCommand(command, data, lines[i]);
//
//		if (command == 0) {// exit
//			// controller.showCommand();
//			break;
//		}
//		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu
//
//		}
//
//		if (command > 0) {// other player do something
//			player->dealCommand(command, data);
//		}
//		else { // Ask command
//			controller.sendCard(command, player->dealCommand(command, gameCount));
//		}
//	}
//}
//
//void main() {
//	Anylsis anylsis;
//	anylsis.InitGame();
//	anylsis.Run_Analysis();
//}