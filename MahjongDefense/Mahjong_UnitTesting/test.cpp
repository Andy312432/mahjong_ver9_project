#include "pch.h"

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <vector>
#include <string>

#include "../Mahjong/ControllerTest.cpp"
#include "../Mahjong/ControllerTest.h"
#include "../Mahjong/Table.cpp"
#include "../Mahjong/Table.h"
#include "../Mahjong/Player.cpp"
#include "../Mahjong/Player.h"
#include "../Mahjong/AI.cpp"
#include "../Mahjong/AI.h"
#include "../Mahjong/Human.cpp"
#include "../Mahjong/Human.h"
#include "../Mahjong/PrivateHand.cpp"
#include "../Mahjong/PrivateHand.h"
#include "../Mahjong/PublicHand.cpp"
#include "../Mahjong/PublicHand.h"
#include "../Mahjong/PredictRemainTiles.cpp"
#include "../Mahjong/PredictRemainTiles.h"
#include "../Mahjong/WallTiles.cpp"
#include "../Mahjong/WallTiles.h"
#include "../Mahjong/GameStatue.h"

using namespace std;

Table table;

::testing::AssertionResult IsBetweenInclusive(int val, int a, int b) {
	if ((val >= a) && (val <= b))
		return ::testing::AssertionSuccess();
	else
		return ::testing::AssertionFailure()
		<< val << " is outside the range " << a << " to " << b;
}

TEST(ThrowTesting, Test_1) {
	char num = '1';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}
	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 210, 213));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_2) {
	char num = '2';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}
	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 360, 363) || IsBetweenInclusive(controller.throwCard, 330, 333));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_3) {
	char num = '3';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 270, 273));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_4) {
	char num = '4';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 350, 353));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_5) {
	char num = '5';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 350, 353));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_6) {
	char num = '6';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 340, 343) || IsBetweenInclusive(controller.throwCard, 360, 363));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_7) {
	char num = '7';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 110, 113));

	delete player; input_file.close();
}

TEST(ThrowTesting, Test_8) {
	char num = '8';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 180, 183) || IsBetweenInclusive(controller.throwCard, 120, 123));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_9) {
	char num = '9';
	string a = ".txt";
	string filename("Data/");
	filename.push_back(num);
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 260, 263));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_10) {
	string a = "10.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 120, 123));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_11) {
	string a = "11.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 190, 193) || IsBetweenInclusive(controller.throwCard, 160, 163));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_12) {
	string a = "12.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 180, 183) || IsBetweenInclusive(controller.throwCard, 190, 193)
		|| IsBetweenInclusive(controller.throwCard, 310, 313) || IsBetweenInclusive(controller.throwCard, 330, 333));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_13) {
	string a = "13.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 180, 183) || IsBetweenInclusive(controller.throwCard, 190, 193)
		|| IsBetweenInclusive(controller.throwCard, 310, 313) || IsBetweenInclusive(controller.throwCard, 330, 333));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_14) {
	string a = "14.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 180, 183) || IsBetweenInclusive(controller.throwCard, 190, 193)
		|| IsBetweenInclusive(controller.throwCard, 310, 313) || IsBetweenInclusive(controller.throwCard, 330, 333));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_15) {
	string a = "15.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 290, 293));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_16) {
	string a = "16.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(controller.SendCommand == "/gong 0 290 291 292 293");
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_17) {
	string a = "17.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 320, 323));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_18) {
	string a = "18.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 380, 383));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_19) {
	string a = "19.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 380, 383));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_20) {
	string a = "20.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 140, 143));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_21) {
	string a = "21.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 290, 293));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_22) {
	string a = "22.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 180, 183));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_23) {
	string a = "23.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 150, 153) || IsBetweenInclusive(controller.throwCard, 240, 243));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_24) {
	string a = "24.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 360, 363));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_25) {
	string a = "25.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 130, 133));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_26) {
	string a = "26.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 270, 273));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_27) {
	string a = "27.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 120, 123));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_28) {
	string a = "28.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 430, 433));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_29) {
	string a = "29.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 110, 113));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_30) {
	string a = "30.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 320, 323));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_31) {
	string a = "31.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 380, 383));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_32) {
	string a = "32.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(controller.SendCommand == "/eat 160 170");
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_33) {
	string a = "33.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(controller.SendCommand == "/throw 451");
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_34) {
	string a = "34.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 250, 253) || IsBetweenInclusive(controller.throwCard, 420, 423));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_35) {
	string a = "35.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 160, 163) || IsBetweenInclusive(controller.throwCard, 180, 183));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_36) {
	string a = "36.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 190, 193));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_37) {
	string a = "37.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 110, 113));
	delete player; input_file.close();
}

TEST(ThrowTesting, Test_38) {
	string a = "38.txt";
	string filename("Data/");
	filename += a;
	vector<string> lines;
	string line;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
	}
	while (getline(input_file, line)) {
		lines.push_back(line);
	}

	input_file.close();
	ControllerTest controller(lines);

	Player* player = new AI(controller.getID(), controller.getInitCard(), &table);

	int command, gameCount = 0;
	vector<int> data;
	for (int i = 3; i < lines.size(); i++) {
		controller.getCommand(command, data, lines[i]);

		if (command == 0) {// exit
			// controller.showCommand();
			break;
		}
		else if (command == 5 && data.at(0) == controller.getID()) {// 5 : hu

		}

		if (command > 0) {// other player do something
			player->dealCommand(command, data);
		}
		else { // Ask command
			controller.sendCard(command, player->dealCommand(command, gameCount));
		}
	}

	EXPECT_TRUE(IsBetweenInclusive(controller.throwCard, 290, 293));
	delete player; input_file.close();
}

void main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	if (table.initListenTable() == 0) {
		cout << "***** Listen Table generate error. Please restart the program! *****" << endl;
	}

	if (table.initDismantlingTable() == 0) {
		cout << "***** Dismantling Table generate error. Please restart the program! *****" << endl;
	}

	if (table.initValidTileTable() == 0) {
		cout << "***** ValidTile Table generate error. Please restart the program! *****" << endl;
	}

	::testing::GTEST_FLAG(filter) = "ThrowTesting*Test_17*:ThrowTesting*Test_21*:ThrowTesting*Test_31*:ThrowTesting*Test_37*:ThrowTesting*Test_38*";
	//::testing::GTEST_FLAG(filter) = "ThrowTesting*Test_35*:ThrowTesting*Test_36*:ThrowTesting*Test_37*:ThrowTesting*Test_38*";
	//::testing::GTEST_FLAG(filter) = "ThrowTesting*Test_38";
	//::testing::GTEST_FLAG(filter) = "ThrowTesting*Test_24*:ThrowTesting*Test_28*";
	RUN_ALL_TESTS();
}
//233556w, 567t, 35s
//publicGroupNum : 2