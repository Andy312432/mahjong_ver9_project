#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <ctime>

#include "ControllerStdin.h"
#include "Tools.cpp"

const string ControllerStdin::command[7] =
{
	"exit",
	"mo",
	"throw",
	"eat",
	"pong",
	"hu",
	"gong"
};

ControllerStdin::ControllerStdin() {
	execute_time = getTime();
#ifdef testCommands
	std::ifstream commandLog("commandLog.txt", std::ios::in);
	std::getline(commandLog, Start);
	std::getline(commandLog, Init_Game);
	std::getline(commandLog, Init_Card);
	commandLog.close();
#else
	getline(cin, Start);
	//recordCommand(Start);
	getline(cin, Init_Game);
	//recordCommand(Init_Game);
	getline(cin, Init_Card);
	//recordCommand(Init_Card);
#endif
#ifdef LogCommand
	std::ofstream commandLog("commandLog.txt");
	commandLog << Start << "\n";
	commandLog << Init_Game << "\n";
	commandLog << Init_Card << "\n";
	commandLog.close();
#endif
	split_Str(Init_Card.substr(Init_Card.find(' ') + 1), Cards);

	SendCommand = "";
}

ControllerStdin::~ControllerStdin() {

}

int ControllerStdin::getID() {
	int temp = Start[10] - '0';;
	//Str_to_Int(Start, temp);
	return temp;
}

void ControllerStdin::showID() {
	cout << Start << endl;
}

string ControllerStdin::getStartCommand() {
	return Start;
}

string ControllerStdin::getInitGameCommand() {
	return Init_Game;
}

void ControllerStdin::showInitGame() {
	cout << Init_Game << endl;
}

vector<int> ControllerStdin::getInitCard() {
	return Cards;
}

string ControllerStdin::getInitCardCommand() {
	return Init_Card;
}

void ControllerStdin::getCommand(int& Command, vector<int>& Data) {
	getline(cin, CommandLine);
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

string ControllerStdin::getCommand() {
	return CommandLine;
}

void ControllerStdin::showCommand() {
	cout << CommandLine;
}
// Ask command
void ControllerStdin::sendCard(const int& Command, const vector<int>& Data) {
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
		cout << temp << endl << flush;
		SendCommand = temp;
		fflush(stderr);
		//recordCommand(temp + '\n');
	}
}

void ControllerStdin::sendCard(const string& Data) {
	cout << Data << flush;;
	SendCommand = Data;
}

void ControllerStdin::showSendCard() {
	cout << SendCommand << endl;
}

void ControllerStdin::recordCommand(const string& Command) {
	string id;
	Int_to_Str(getID(), id);
	string temp(".\\Log\\" + execute_time + "_Command_log_" + id + ".txt");

	fstream fout;
	fout.open(temp.c_str(), ios::out | ios::app);
	fout << Command;
	fout.close();
}
