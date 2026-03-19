#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <ctime>

#include "ControllerSocket.h"
#include "Tools.cpp"

const string ControllerSocket::command[7] =
{
	"exit",
	"mo",
	"throw",
	"eat",
	"pong",
	"hu",
	"gong"
};

ControllerSocket::ControllerSocket() :clientSocket("127.0.0.1", 1234) {
	execute_time = getTime();

	clientSocket.recvData(Start);
	//recordCommand(Start);
	clientSocket.recvData(Init_Game);
	//recordCommand(Init_Game);
	clientSocket.recvData(Init_Card);
	//recordCommand(Init_Card);
	split_Str(Init_Card.substr(Init_Card.find(' ') + 1), Cards);

	SendCommand = "";
}

ControllerSocket::ControllerSocket(const ClientSocket& clientSocket) {
	this->clientSocket = clientSocket;
	execute_time = getTime();

	clientSocket.recvData(Start);
	//recordCommand(Start);
	clientSocket.recvData(Init_Game);
	//recordCommand(Init_Game);
	clientSocket.recvData(Init_Card);
	//recordCommand(Init_Card);
	split_Str(Init_Card.substr(Init_Card.find(' ') + 1), Cards);

	SendCommand = "";
}

ControllerSocket::~ControllerSocket() {
	//clientSocket.closeSocket();
}

int ControllerSocket::getID() {
	int temp = Start[10] - '0';;
	//Str_to_Int(Start, temp);
	return temp;
}

void ControllerSocket::showID() {
	cout << Start << endl;
}

string ControllerSocket::getStartCommand() {
	return Start;
}

string ControllerSocket::getInitGameCommand() {
	return Init_Game;
}

void ControllerSocket::showInitGame() {
	cout << Init_Game << endl;
}

vector<int> ControllerSocket::getInitCard() {
	return Cards;
}

string ControllerSocket::getInitCardCommand() {
	return Init_Card;
}

void ControllerSocket::getCommand(int& Command, vector<int>& Data) {
	clientSocket.recvData(CommandLine);
	//recordCommand(CommandLine);
	// game finish and close server will show a lot message
	if (CommandLine.size() == 0) {
		cout << ">> Server close Socket." << endl << flush;
		exit(1);
	}

	vector<string> temp;
	split_Str(CommandLine, temp);

	if (temp[0].find("ask") != string::npos) {
		for (int i = 2; i < 7; i++) {
			if (temp[1].find(command[i]) != string::npos) {
				Command = -i;
				Data.clear();
				return;
			}
		}
	}
	else {
		for (int i = 0; i < 7; i++) {
			if (temp[0].find(command[i]) != string::npos) {
				Command = i;
				if (i == 6)   // Gong
				{
					if (temp[2] == "0")   // DarkGong
					{
						Command += 1;
					}
					else if (temp[2] == "1")   // BuGong
					{
						Command += 2;
					}
				}
				break;
			}
		}
	}

	// put value into Data
	Data.clear();
	split_Str(CommandLine.substr(CommandLine.find(' ') + 1), Data);
	return;
}

string ControllerSocket::getCommand() {
	return CommandLine;
}

void ControllerSocket::showCommand() {
	cout << CommandLine;
}
// Ask command
void ControllerSocket::sendCard(const int& Command, const vector<int>& Data) {
	if (Command < 0) {
		string temp;
		stringstream ss;

		if (Command == -5) // isHu
		{
			if (Data[0] == 1) {
				ss << "/hu";
			}
			else {
				ss << "/pass";
			}
		}
		else {
			ss << ("/" + command[(Command < -6 ? 6 : -Command)]);
		}

		if (Command == -6) {
			ss << " 4";
		}
		else if (Command == -7) {
			ss << " 0";
		}
		else if (Command == -8) {
			ss << " 1";
		}

		if (Command != -5) {
			if (Data.empty()) {
				ss.str("");
				ss.clear();
				ss << "/pass";
			}
			else {
				for (int i = 0; i < (int)Data.size(); i++) {
					ss << " " << Data[i];
				}
			}
		}
		getline(ss, temp, '\n');
		clientSocket.sendData(temp + '\n');
		SendCommand = temp;
		//recordCommand(temp + '\n');
	}
}

void ControllerSocket::sendCard(const string& Data) {
	clientSocket.sendData(Data);
	SendCommand = Data;
	//recordCommand(Data);
}

void ControllerSocket::showSendCard() {
	cout << SendCommand << endl;
}

void ControllerSocket::recordCommand(const string& Command) {
	string id;
	Int_to_Str(getID(), id);
	string temp(".\\Log\\" + execute_time + "_Command_log_" + id + ".txt");

	fstream fout;
	fout.open(temp.c_str(), ios::out | ios::app);
	fout << Command;
	fout.close();
}
