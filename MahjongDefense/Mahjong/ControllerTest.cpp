#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <ctime>

#include "ControllerTest.h"
#include "Tools.cpp"

const string ControllerTest::command[7] =
{
	"exit",
	"mo",
	"throw",
	"eat",
	"pong",
	"hu",
	"gong"
};

ControllerTest::ControllerTest(vector<string> lines) {
	execute_time = getTime();
	Start = lines[0];
	Init_Game = lines[1];
	Init_Card = lines[2];
	split_Str(Init_Card.substr(Init_Card.find(' ') + 1), Cards);

	SendCommand = "";
}

ControllerTest::~ControllerTest() {

}

int ControllerTest::getID() {
	int temp = Start[10] - '0';;
	//Str_to_Int(Start, temp);
	return temp;
}

void ControllerTest::showID() {
	cout << Start << endl;
}

string ControllerTest::getStartCommand() {
	return Start;
}

string ControllerTest::getInitGameCommand() {
	return Init_Game;
}

void ControllerTest::showInitGame() {
	cout << Init_Game << endl;
}

vector<int> ControllerTest::getInitCard() {
	return Cards;
}

string ControllerTest::getInitCardCommand() {
	return Init_Card;
}

void ControllerTest::getCommand(int& Command, vector<int>& Data, string lines) {
	CommandLine = lines;
	//recordCommand(CommandLine);
	// game finish and close server will show a lot message
	if (CommandLine.size() == 0) {
		cout << ">> Server close Socket." << endl;
		exit(1);
	}

	vector<string> temp;
	split_Str(CommandLine, temp);

	if (temp.at(0).find("ask") != string::npos) {
		for (int i = 2; i < 7; i++) {
			if (temp.at(1).find(command[i]) != string::npos) {
				Command = -i;
				Data.clear();
				return;
			}
		}
	}
	else {
		for (int i = 0; i < 7; i++) {
			if (temp.at(0).find(command[i]) != string::npos) {
				Command = i;
				if (i == 6)   // Gong
				{
					if (temp.at(2) == "0")   // DarkGong
					{
						Command += 1;
					}
					else if (temp.at(2) == "1")   // BuGong
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

string ControllerTest::getCommand() {
	return CommandLine;
}

void ControllerTest::showCommand() {
	cout << CommandLine;
}
// Ask command
void ControllerTest::sendCard(const int& Command, const vector<int>& Data) {
	if (Command < 0) {
		string temp;
		stringstream ss;

		if (Command == -5) // isHu
		{
			if (Data.at(0) == 1) {
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
					ss << " " << Data.at(i);
					throwCard = Data.at(i);
				}
			}
		}
		//cout << temp << endl << flush;
		getline(ss, temp, '\n');
		cout << temp << endl << flush;
		SendCommand = temp;
		fflush(stderr);
		//recordCommand(temp + '\n');
	}
}

void ControllerTest::sendCard(const string& Data) {
	cout << Data << flush;;
	SendCommand = Data;
}

void ControllerTest::showSendCard() {
	cout << SendCommand << endl;
}

void ControllerTest::recordCommand(const string& Command) {
	string id;
	Int_to_Str(getID(), id);
	string temp(".\\Log\\" + execute_time + "_Command_log_" + id + ".txt");

	fstream fout;
	fout.open(temp.c_str(), ios::out | ios::app);
	fout << Command;
	fout.close();
}
