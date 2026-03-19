#pragma once
#ifndef __CONTROLLERSOCKET_H__
#define __CONTROLLERSOCKET_H__

#include <vector>
#include <string>

#include "ClientSocket.h"

class ControllerSocket {
private:
	ClientSocket clientSocket;
	string Start;
	string Init_Game;
	string Init_Card;
	string CommandLine;
	string SendCommand;
	static const string command[];
	vector<int> Cards;
	string execute_time;
public:
	ControllerSocket();
	ControllerSocket(const string& ip, const int& port);
	ControllerSocket(const ClientSocket& clientSocket);
	~ControllerSocket();

	int getID();
	
	string getCommand();
	string getStartCommand();
	string getInitGameCommand();
	string getInitCardCommand();

	vector<int> getInitCard();

	void getCommand(int& Command, vector<int>& Data);
	void sendCard(const int& Command, const vector<int>& Data);
	void sendCard(const string& Data);
	void recordCommand(const string& Command);
	void showID();
	void showInitGame();	
	void showCommand();
	void showSendCard();
};

#endif