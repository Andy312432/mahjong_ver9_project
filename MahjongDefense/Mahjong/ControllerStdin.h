#pragma once
#ifndef __CONTROLLERSTDIN_H__
#define __CONTROLLERSTDIN_H__

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <ctime>
//#define LogCommand // Write commands into file (For tracing)
//#define testCommands // Test the commands in the log file

using std::string;
using std::vector;

class ControllerStdin {
private:
	string Start;
	string Init_Game;
	string Init_Card;
	string CommandLine;
	string SendCommand;
	string execute_time;
	static const string command[];
	vector<int> Cards;
public:
	ControllerStdin();
	~ControllerStdin();

	int getID();
	string getCommand();
	string getStartCommand();
	string getInitGameCommand();
	vector<int> getInitCard();
	string getInitCardCommand();
	void showID();
	void showInitGame();
	void getCommand(int& Command, vector<int>& Data);
	void showCommand();
	void sendCard(const int& Command, const vector<int>& Data);
	void sendCard(const string& Data);
	void showSendCard();
	void recordCommand(const string& Command);
};

#endif