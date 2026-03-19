#pragma once
#ifndef __CONTROLLERSTDIN_H__
#define __CONTROLLERSTDIN_H__

#include <vector>
#include <string>

using std::string;
using std::vector;

class ControllerTest {
private:
	string Start;
	string Init_Game;
	string Init_Card;
	string CommandLine;
	static const string command[];
	vector<int> Cards;
	string execute_time;
public:
	ControllerTest(vector<string> lines);
	~ControllerTest();

	int throwCard;
	string SendCommand;

	int getID();
	void showID();
	string getStartCommand();
	string getInitGameCommand();
	vector<int> getInitCard();
	string getInitCardCommand();
	void showInitGame();
	void getCommand(int& Command, vector<int>& Data, string lines);
	string getCommand();
	void showCommand();
	void sendCard(const int& Command, const vector<int>& Data);
	void sendCard(const string& Data);
	void showSendCard();
	void recordCommand(const string& Command);
};

#endif