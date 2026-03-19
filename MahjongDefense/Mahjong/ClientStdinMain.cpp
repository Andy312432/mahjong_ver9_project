#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <vector>
#include <string>

#include "ControllerStdin.cpp"
#include "Table.cpp"
#include "Table.h"
#include "Player.cpp"
#include "Player.h"
#include "AI.cpp"
#include "AI.h"
#include "Human.cpp"
#include "Human.h"
#include "PrivateHand.cpp"
#include "PrivateHand.h"
#include "PublicHand.cpp"
#include "PublicHand.h"
#include "PredictRemainTiles.cpp"
#include "PredictRemainTiles.h"
#include "WallTiles.cpp"
#include "WallTiles.h"
#include "GameStatue.h"
#include "DefenseAnalyze.h"
#include "DefenseAnalyze.cpp"

#include "TT.h" // Modified

using namespace std;

boost::multiprecision::uint256_t genRand256(); // Modified

#define MODE 0 // 0: ai, 1: human, 2: random, 3: run in IDE(debug)
enum : int { Ai, HUMAN, RandomAI, DEBUG };

int main() {
	Table table;
#if (MODE == 0 || MODE == 3)
	if (table.initListenTable() == 0) {
		cout << "***** Listen Table generate error. Please restart the program! *****" << endl << std::flush;
	}

	if (table.initDismantlingTable() == 0) {
		cout << "***** Dismantling Table generate error. Please restart the program! *****" << endl << std::flush;
	}

	if (table.initValidTileTable() == 0) {
		cout << "***** ValidTile Table generate error. Please restart the program! *****" << endl << std::flush;
	}
	
	std::cout << "Initialzing hash key...\n"; // Modified
	for (int i = 0; i < 136; i++) { // Modified
		tileNumHash[i] = genRand256(); // Modified
		oppTileNumHash[i] = genRand256(); // Modified
	} // Modified
	for (int i = 0; i < 33554432; i++) tt[i].simNum = 0; // Modified
	std::cout << "Hash key initialization completed.\n"; // Modified

#endif
//#if (IO_MODE == 1)
#if (MODE == 0)
	while (true) {
		string ready;
		getline(cin, ready);
		if (ready == "/ready") {
			cout << "ATTACH DEBUGGER!"<<endl;
			Sleep(5000);
			cout << "/ready" << endl << flush;
			break;
		}
	}
	
#endif
	int gameCount = 1;
	int winCount = 0;

	while (true) {
		ControllerStdin controller;
		//controller.showID();
		//controller.showInitGame();
		// Init Games
		Player* player = [&]() {
			switch (MODE) {
			case Ai:case DEBUG:
					return new AI(controller.getID(), controller.getInitCard(), &table);
				//case HUMAN:
				//	return new Human(controller.getID(), controller.getInitCard());
			case RandomAI:
					return new AI(controller.getID(), controller.getInitCard());
			default:
					return new AI(controller.getID(), controller.getInitCard(), &table);
			}
		}();

		// Game start
		int command;
		vector<int> data;
#ifdef LogCommand
		std::ofstream commandLog;
#endif
#ifdef testCommands
		std::ifstream commandLog;
		int commandCount = 3;
#endif
		do {
#ifndef testCommands
			controller.getCommand(command, data);

			std::cout << "ID: " << player->getID()<< "\n";
			std::cout << "Command: " << command << "\nData: "; // Debug
			for (int i = 0; i < data.size(); i++) std::cout << data[i] << " "; // Debug
			std::cout << "\n"; // Debug
			
#endif
#ifdef LogCommand
			if (player->getID() == 4) { // Record first player's commands
				commandLog.open("commandLog.txt", std::ios::app);
				assert(commangLog);
				commandLog << command << " ";
				for (int i = 0; i < data.size(); i++) commandLog << data[i] << " ";
				commandLog << ".\n";
				std::cout << "ID: " << player->getID();
				std::cout << "\nCommand: " << command << "\nData: "; // Debug
				for (int i = 0; i < data.size(); i++) std::cout << data[i] << " "; // Debug
				std::cout << "\n"; // Debug
				commandLog.close();
			}
#endif
#ifdef testCommands
			//std::cout << commandCount << "\n"; // Debug
			//system("pause"); // Debug
			commandLog.open("commandLog.txt", std::ios::in);
			// Skip the previous lines
			for (int i = 0; i < commandCount; i++) {
				std::string tmp;
				std::getline(commandLog, tmp);
				//std::cout << tmp << "\n"; // Debug
			}
			// Start reading
			commandLog >> command;
			std::cout << "Command" << command << "\n";
			std::string inputData;
			data.clear();
			while (commandLog >> inputData && inputData != ".") {
				//std::cout << inputData << " "; // Debug
				data.push_back(stoi(inputData));
				inputData.clear();
			}
			std::cout << "Data: "; // Debug
			for (int i = 0; i < data.size(); i++) std::cout << data[i] << " "; // Debug
			std::cout << "\n"; // Debug
			commandCount++;
			commandLog.close();
#endif
			if (command == 0) { // exit
				// controller.showCommand();
				break;
			}
			else if (command == 5 && data[0] == controller.getID()) { // 5 : hu
				winCount++;
				free(tt);
				//recordWin("GameResult.csv", gameCount, winCount);
			}

			if (command > 0) { // other player do something		
				player->dealCommand(command, data);
			}
			else {	// Ask command
				controller.sendCard(command, player->dealCommand(command, gameCount));
			}

			std::cout << "PrivateHand:\n"; // Debug
			player->ShowPrivateHand(); // Debug
			std::cout << "PublicHand:\n"; // Debug
			player->ShowPublicHand(); // Debug
			std::cout << "TileSea:\n"; // Debug
			player->ShowTileSea(); // Debug

		} while (true);

		if (player != NULL) {
			std::cout << "Out of while loop\n"; // Debug
			delete player;
		}
	}
}

boost::multiprecision::uint256_t genRand256() { // Modified
	return (((boost::multiprecision::uint256_t)rand() << 240)
		| ((boost::multiprecision::uint256_t)rand() << 224)
		| ((boost::multiprecision::uint256_t)rand() << 208)
		| ((boost::multiprecision::uint256_t)rand() << 192)
		| ((boost::multiprecision::uint256_t)rand() << 176)
		| ((boost::multiprecision::uint256_t)rand() << 160)
		| ((boost::multiprecision::uint256_t)rand() << 144)
		| ((boost::multiprecision::uint256_t)rand() << 128)
		| ((boost::multiprecision::uint256_t)rand() << 112) 
		| ((boost::multiprecision::uint256_t)rand() << 96) 
		| ((boost::multiprecision::uint256_t)rand() << 80) 
		| ((boost::multiprecision::uint256_t)rand() << 64) 
		| ((boost::multiprecision::uint256_t)rand() << 48) 
		| ((boost::multiprecision::uint256_t)rand() << 32) 
		| ((boost::multiprecision::uint256_t)rand() << 16) 
		| rand());
}