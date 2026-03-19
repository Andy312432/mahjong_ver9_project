//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Control player model (AI / Human / Random AI) and receive / send the data to server.
//
//=============================================================================

#include <chrono>

#include "Player.h"

#define SHOW_INFORMATION 0

const std::string Player::kCommandString[] =
{
  "Throw",
  "Eat",
  "Pong",
  "Hu",
  "Ming_Gong",
  "Dark_Gong",
  "Bu_Gong"
};

Player::Player(const int& ID, const PrivateHand& cards) : wallTiles(), publicHand() {
	this->ID = ID;
	this->lastCommand = 1;
	this->takeTile = -1;
	privateHand = cards;
}

Player::Player(const int& ID, const vector<int>& cards) : wallTiles(), publicHand(), privateHand(cards) {
	this->ID = ID;
	this->lastCommand = 1;
	this->takeTile = -1;
}

// move player command
void Player::dealCommand(const int& Command, const vector<int>& Data) {
	switch (Command) {
	case 1: { // take
		//printf("Take card: %d\n", Data[0]);
		privateHand.AddHand(Data[0], hashKey);
		DealTake(Data[0]);//將資料更新（並無決斷）
		if (mode == 0)
			DealRemainTiles(Data[0]);
#if (SHOW_INFORMATION == 1)
		ShowPrivateHand();
#endif
		break;
	}
	case 2: { // throw
		wallTiles.AddTileSea(Data[1]);
		if (Data[0] == ID) {
			vector<int> throwTile(1, Data[1]);
			privateHand.RemoveHand(Data[1], hashKey); // Modified
			MoveThrow(throwTile);
#if (SHOW_INFORMATION == 1)
			privateHand.ShowPrivateHand();
#endif
		}
		else if (mode == 0) { // AI mode & record others player
			DealRemainTiles(Data[1]); /// 把被對手捨掉的牌加入已出現的牌集合中
		}
		break;
	}
	case 3: { // eat
		vector<int> temp(Data.begin() + 1, Data.end());
		if (Data[0] == ID) { /// 我方吃牌 (不用更新以出現的牌集合，因為我方摸到牌時就已經更新過了)
			publicHand[Data[0] - 1].AddHand(temp); /// 新增該玩家的明牌資訊 // Modified
			privateHand.RemoveHand(Data[1], hashKey); // Modified
			privateHand.RemoveHand(Data[3], hashKey); // Modified
			vector<int> throwTile(1, Data[1]);
			throwTile.push_back(Data[3]);
			MoveThrow(throwTile);
		}
		else if (mode == 0) { // AI mode & record others player
			publicHand[Data[0] - 1].AddHand(temp, hashKey, oppTileNum); /// 新增該玩家的明牌資訊 // Modified
			DealRemainTiles(Data[1]);
			DealRemainTiles(Data[3]); /// 把牌加到已經出現的牌集合中

			oppGroup += 3;
		}
		break;
	}
	case 4: { // pong
		vector<int> temp(Data.begin() + 1, Data.end());
		vector<int> throwTile;
		int throwCard = wallTiles.getTileSea();
		if (Data[0] == ID) { // Modified
			publicHand[Data[0] - 1].AddHand(temp); /// 新增該玩家的明牌資訊 // Modified
			for (int i = 1; i <= 3; i++) {
				if (Data[i] != throwCard) {
					privateHand.RemoveHand(Data[i], hashKey); // Modified
					throwTile.push_back(Data[i]);
				}
			}
			MoveThrow(throwTile);
		}
		else if (mode == 0) { // AI mode & record others player // Modified
			publicHand[Data[0] - 1].AddHand(temp, hashKey, oppTileNum); /// 新增該玩家的明牌資訊 // Modified
			for (int i = 1; i <= 3; i++) {
				if (Data[i] != throwCard) {
					DealRemainTiles(Data[i]);
				}
			}
			oppGroup += 3;
		}
		break;
	}
	case 5: { // hu
		//ShowFunctionTime();
		//printf("Game Finished!\n");
		//fflush(stdout);
		break;
	}
	case 6: { // MingGong
		vector<int> throwTile;
		vector<int> temp(Data.begin() + 2, Data.end());
		int throwCard = wallTiles.getTileSea();
		if (Data[0] == ID) { // Modiifed
			publicHand[Data[0] - 1].AddHand(temp); /// 新增該玩家的明牌資訊 // Modified
			for (int i = 2; i <= 5; i++) {
				if (Data[i] != throwCard) {
					privateHand.RemoveHand(Data[i], hashKey); // Modified
					throwTile.push_back(Data[i]);
				}
			}
			MoveThrow(throwTile);
		}
		else if (mode == 0) { // Modified
			publicHand[Data[0] - 1].AddHand(temp, hashKey, oppTileNum); /// 新增該玩家的明牌資訊 // Modified
			for (int i = 2; i <= 5; i++) {
				if (Data[i] != throwCard) {
					// AI mode & record others player
					DealRemainTiles(Data[i]);
				}
			}
			oppGroup += 4;
		}
		break;
	}
	case 7: { // DarkGong
		if (Data[0] == ID) {
			vector<int> temp(Data.begin() + 2, Data.end());
			publicHand[Data[0] - 1].AddHand(temp);
			privateHand.RemoveHand(Data[2], hashKey); // Modified
			privateHand.RemoveHand(Data[3], hashKey); // Modified
			privateHand.RemoveHand(Data[4], hashKey); // Modified
			privateHand.RemoveHand(Data[5], hashKey); // Modified
			vector<int> throwTile(1, Data[2]);
			throwTile.push_back(Data[3]);
			throwTile.push_back(Data[4]);
			throwTile.push_back(Data[5]);
			MoveThrow(throwTile);
		}
		else {
			vector<int> temp(4, 0);
			publicHand[Data[0] - 1].AddHand(temp);
			oppGroup += 4;
		}
		break;
	}
	case 8: { // BuGong
		if (Data[0] == ID) {
			publicHand[Data[0] - 1].AddBuGong(Data[2]);
			privateHand.RemoveHand(Data[2], hashKey); // Modified
			vector<int> throwTile(1, Data[2]);
			MoveThrow(throwTile);
		}
		else if (mode == 0) // AI mode & record others player
		{
			publicHand[Data[0] - 1].AddBuGong(Data[2], hashKey, oppTileNum);
			DealRemainTiles(Data[2]);
		}
		break;
	}
	default: {
		printf("*** Error command! ***\n");
		fflush(stdout);
		break;
	}

	}

#if (SHOW_INFORMATION == 1)
	//show Message
	switch (Command) {
	case 1: // take
		ShowMessage(Data);
		break;
	case 6: { // Ming Gong
		vector<int> temp(Data.begin() + 2, Data.end());
		ShowMessage(Command - 2, Data[0), temp);
		break;
	}
	default: {
		vector<int> temp(Data.begin() + 1, Data.end());
		ShowMessage(Command - 2, Data[0), temp);
		break;
	}
	}
#endif
	// record last Command
	lastCommand = Command;
}

// ask player command
vector<int> Player::dealCommand(int& Command, int& gameCount) {
#if (SHOW_INFORMATION == 1)
	ShowMessage(-Command - 2);
#endif
	vector<int> data;
	switch (-Command) {
		case 2: { // throw
			std::cout << "Receive command\n"; // Debug
			std::ofstream f("TimeRecords.csv", std::ios::app);

			//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
			data = dealThrow();
			//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			//f << gameCount << "," << this->ID << "," << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;
			//f.close();
			std::cout << "Finish the porcess of command. Return "; // Debug
			for (int i = 0; i < data.size(); i++) std::cout << data[i] << " "; // Debug
			std::cout << "\n";
			break;
		}
		case 3: { // eat
			data = dealEat();
			break;
		}
		case 4: { // pong
			data = dealPong();
			break;
		}
		case 5: { // hu
			data = dealHu();
			break;
		}
		case 6: { // gong
			if (lastCommand == 2) {
				Command = -6;
				data = dealMingGong();
			}
			else if (lastCommand == 1) {
				bool enterBuGong = false;
				
				if (publicHand[ID - 1].findBuGong(takeTile)) {
					enterBuGong = true;
					Command = -8;
					data = dealBuGong(takeTile);
				}

				if (!enterBuGong || data.size() == 0) {
					if (privateHand.findDarkGong()) {
						Command = -7;
						data = dealDarkGong();
					}
					else {
						Command = -7;
						data.clear();
					}
				}
			}
			break;
		}
		default: {
			printf("Error command!\n");
			fflush(stdout);
			break;
		}
	}
	// record last Command
	lastCommand = Command;
	return data;
}

void Player::ShowMessage(const int& Command) {
	printf("\n");
	try {
		ShowPrivateHand();
		ShowPublicHand();
		ShowTileSea();
		//ShowRemainCard();
		if (Command != 6) {
			printf("%s?\n", kCommandString[Command].c_str());
		}
		else {
			printf("Gong?\n");
		}

		fflush(stdout);
	}
	catch (...) {
		printf("***** SHOW MESSAGE ERROR *****\n");
		fflush(stdout);
	}
}

void Player::ShowMessage(const int& Command, const int& Player, const vector<int>& Data) {
	printf("\n### Player %d %s ", Player, kCommandString[Command].c_str());
	for (int i = 0; i < (int)Data.size(); i++) {
		printf("%d ", Data[i]);
	}
	printf(" ###\n");

	ShowPublicHand();
	ShowTileSea();
}

void Player::ShowMessage(const vector<int>& Data) {
	printf("\n### Take %d ###\n", Data[0]);
	fflush(stdout);
}

void Player::ShowPrivateHand() {
	printf("Hand: ");
	this->privateHand.ShowPrivateHand();
}

void Player::ShowPublicHand() {
	for (int i = 0; i < 4; i++) {
		printf("Player %d: ", i + 1);
		publicHand[i].ShowGroup();
	}
}

void Player::ShowTileSea() {
	printf("Tile Sea: ");
	wallTiles.ShowTileSea();
	printf("\n");
	fflush(stdout);
}
