#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <fstream>
#include <cctype>
#include <windows.h>

#include "ClientSocket.cpp"
//#include "ClientSocket.h"
#include "ControllerSocket.cpp"
//#include "ControllerSocket.h"
//#include "Table.cpp"
#include "Table.h"
//#include "Player.cpp"
#include "Player.h"
//#include "AI.cpp"
#include "AI.h"
//#include "Human.cpp"
#include "Human.h"
//#include "PrivateHand.cpp"
#include "PrivateHand.h"
//#include "PublicHand.cpp"
#include "PublicHand.h"
//#include "PredictRemainTiles.cpp"
//#include "PredictRemainTiles.h"
//#include "WallTiles.cpp"
#include "WallTiles.h"
#include "GameStatue.h"

//using namespace std;

// protocols enumerate
enum IO : int {
	SOCKET_INI = 0, // 0
	SOCKET_NTPU,	// 1
	STDIN_CYC,		// 2
	STDIN_NTPU		// 3
};

enum PLAYER_MODE : int {
	Ai = 0,		// 0
	HUMAN,		// 1
	RandomAI	// 2
};

#if _MSC_VER
#include <atlstr.h>
#define IO_MODE 0
#endif // _MSC_VER

bool setParameter(const char iniFile[50], int* const IOMode, string* const ip, int* const port, int* const mode, int* const contest);
void updateTitle(int id, int gameCount, int winCount);

int main(int argc, char** argv) {

	int MODE = Ai;

#if (IO_MODE == 0)
	int IOMODE, CONTEST, PORT;
	string IP, iniFileName = "ClientSetting.ini";
	bool iniFileExist = setParameter(iniFileName.c_str(), &IOMODE, &IP, &PORT, &MODE, &CONTEST);
	if (!iniFileExist) {
		cout << ">> Can Not Find ini File (" << iniFileName << ")" << " --- Use Default Setting" << endl;
	}
	cout << ">> Connect: " << IP << ":" << PORT << endl;
#elif (IO_MODE == 1)
	int CONTEST = 0, PORT = atoi(argv[argc - 1]);
	string IP = argv[argc - 2];
#endif

	Table table;
	if (MODE == Ai) {
		if (table.initListenTable() == 0) {
			cout << "***** Listen Table generate error. Please restart the program! *****" << endl << flush;
		}

		if (table.initDismantlingTable() == 0) {
			cout << "***** Dismantling Table generate error. Please restart the program! *****" << endl << flush;
		}

		if (table.initValidTileTable() == 0) {
			cout << "***** ValidTile Table generate error. Please restart the program! *****" << endl << flush;
		}
	}

	ClientSocket clientSocket;
	if (CONTEST == 0) { // Not contest mode
		while (clientSocket.initSocket(IP, PORT) == false) {
			cout << ">> Socket Connect Failed!" << endl << flush;
			Sleep(1000);
		}
#if (IO_MODE == 1)
		string ready;
		clientSocket.recvData(ready);
		clientSocket.sendData("/ready\n");
#endif
	}

	int gameCount = 1;
	int winCount = 0;

	while (true) {
		if (CONTEST == 1) {
			while (clientSocket.initSocket(IP, PORT) == false) {
				cout << ">> Socket Connect Failed" << endl << flush;
				Sleep(1000);
			}
		}
		// Init Games
		ControllerSocket controller(clientSocket);
		controller.showID();
		controller.showInitGame();

		updateTitle(controller.getID(), gameCount, winCount);

		Player* player = [&]() {
			switch (MODE) {
				case Ai:
					return new AI(controller.getID(), controller.getInitCard(), &table);
				//case HUMAN:
					//	return new Human(controller.getID(), controller.getInitCard());
				case RandomAI:
					return new AI(controller.getID(), controller.getInitCard());
				default:
					return new AI(controller.getID(), controller.getInitCard(), &table);
			}
		}();

		player->ShowPrivateHand();

		// Game start
		int command;
		vector<int> data;
		do {
			controller.getCommand(command, data);

			if (command == 0) { // exit	
				controller.showCommand();
				updateTitle(controller.getID(), gameCount++, winCount);
				ofstream f("TimeRecords.csv", std::ios::app);
				f << "Finished!" << endl;
				f.close();
				break;
			}
			else if (command == 5 && data[0] == controller.getID()) { // 5 : hu
				winCount++;
			}

			if (command > 0) { // other player do something
				player->dealCommand(command, data);
			}
			else { // Ask command
				controller.sendCard(command, player->dealCommand(command, gameCount));
			}

		} while (true);

		if (player != NULL) {
			delete player;
			player = NULL;
		}
	}

	return 0;
}


#if _MSC_VER 
void updateTitle(int id, int gameCount, int winCount) {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, CA2CT(buffer), MAX_PATH);

	string title = "ID: " + to_string(id) + " Game: " + to_string(gameCount) + " Win: " + to_string(winCount) + " Path: " + string(buffer);
	SetConsoleTitle(CA2CT(title.c_str()));
}

bool setParameter(const char iniFile[50], int* const IOMode, string* const ip, int* const port, int* const mode, int* const contest) {
	// Test if the ini file exist
	ifstream f(iniFile);
	if (!f.good()) {
		// Initfile is not exist
		// Use default setting
		*IOMode = 0;
		*ip = "127.0.0.1";
		*port = 1234;
		*mode = 0;
		*contest = 0;

		return false;
	}

	// Get absolute path
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CA2CT(path));
	strcat(path, "\\");
	strcat(path, iniFile);

	*contest = GetPrivateProfileInt(CA2CT("Contest"), CA2CT("ContestMode"), 0, CA2CT(path));
	*IOMode = GetPrivateProfileInt(CA2CT("Connect"), CA2CT("ConnectMode"), 0, CA2CT(path));
	if (*IOMode == 0) {
		// Default ip: 127.0.0.1
		char IP[20];
		GetPrivateProfileString(CA2CT("Connect"), CA2CT("IP"), CA2CT("127.0.0.1"), CA2CT(IP), 20, CA2CT(path));
		*ip = string(IP);
		// Default port: 1234
		*port = GetPrivateProfileInt(CA2CT("Connect"), CA2CT("Port"), 1234, CA2CT(path));
		// Default mode: Ai
		*mode = GetPrivateProfileInt(CA2CT("Player"), CA2CT("PlayerMode"), 0, CA2CT(path));
	}
	else {
		*mode = 0;
	}

	return true;
}
#else
void updateTitle(int id, int gameCount, int winCount) {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	string title = "ID: " + to_string(id) + " Game: " + to_string(gameCount) + " Win: " + to_string(winCount) + " Path: " + string(buffer);
	SetConsoleTitle(title.c_str());
}

bool setParameter(const char iniFile[50], int* const IOMode, string* const ip, int* const port, int* const mode, int* const contest) {
	// Test if the ini file exist
	ifstream f(iniFile);
	if (!f.good()) {
		// Initfile is not exist
		// Use default setting
		*IOMode = 0;
		*ip = "127.0.0.1";
		*port = 1234;
		*mode = 0;
		*contest = 0;

		return false;
	}
	// Get absolute path
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	strcat(path, "\\");
	strcat(path, iniFile);

	*contest = GetPrivateProfileInt("Contest", "ContestMode", 0, path);
	*IOMode = GetPrivateProfileInt("Connect", "ConnectMode", 0, path);
	if (*IOMode == 0) {
		// Default ip: 127.0.0.1
		char IP[20];
		GetPrivateProfileString("Connect", "IP", "127.0.0.1", IP, 20, path);
		*ip = string(IP);
		// Default port: 1234
		*port = GetPrivateProfileInt("Connect", "Port", 1234, path);
		// Default mode: Ai
		*mode = GetPrivateProfileInt("Player", "PlayerMode", 0, path);
	}
	else {
		*mode = 0;
	}

	return true;
}
#endif