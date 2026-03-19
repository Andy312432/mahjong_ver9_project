//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Tools for type conversion.
//
//=============================================================================

#pragma once
#ifndef __TOOLS__
#define __TOOLS__

#include <string>
#include <sstream>
#include <cctype>
#include <vector>
#include <stack>
#include <cstdarg>
#include <algorithm>
#include <Windows.h>
#include <functional>
#include <locale>
#include <ctime>

using namespace std;

void Int_to_Str(const int& data, string& str) {
	stringstream ss;
	stack<char> reverse;
	char temp;
	int num = data;
	while (num != 0) {
		temp = num % 10 + '0';
		num /= 10;
		reverse.push(temp);
	}
	while (!reverse.empty()) {
		ss << reverse.top();
		reverse.pop();
	}
	getline(ss, str);
	return;
}

void Str_to_Int(const string& str, int& data) {
	data = 0;
	for (int i = 0; i < (int)str.size(); i++) {
		char temp = str[i];
		if (isdigit(str[i])) {
			data *= 10;
			data += temp - '0';
		}
	}
	return;
}

void Str_to_Int(const vector<string>& str_vec, vector<int>& data_vec) {
	for (int k = 0; k < (int)str_vec.size(); k++) {
		string str = str_vec[k];
		int sum = 0;
		for (int i = 0; i < (int)str.size(); i++) {
			char temp = str[i];
			if (isdigit(str[i])) {
				sum *= 10;
				sum += temp - '0';
			}
		}
		data_vec.push_back(sum);
	}
	return;
}

void split_Str(const string& str, vector<string>& data) {
	stringstream ss(str);
	string temp;
	while (ss >> temp) {
		data.push_back(temp);
	}
	return;
}

void split_Str(const string& str, vector<int>& data) {
	stringstream ss(str);
	int temp;
	while (ss >> temp) {
		data.push_back(temp);
	}
	return;
}

int Int_pow(const int& num, const int& times) {
	int temp = 1;
	for (int i = 0; i < times; i++) {
		temp *= num;
	}
	return temp;
}

int Int_Combine(const vector<int>& nums) {
	int result = 0;
	for (int i = 0; i < (int)nums.size(); i++) {
		result *= 10;
		result += nums[i];
	}

	return result;
}

int Int_Combine(int nums, ...) {
	va_list args;
	va_start(args, nums);

	int sum = 0;
	for (int i = 0; i < nums; i++) {
		sum *= 10;
		sum += va_arg(args, int);
	}
	va_end(args);

	return sum;
}

int VectorToTileArray(std::vector<int> a, int* tileArray, int* tileNumArray) {
	int index = 0;
	if (a.size() > 0) {
		tileArray[index] = a[0];
		tileNumArray[index]++;
	}
	for (size_t i = 1; i < a.size(); i++) {
		if (tileArray[index] != a[i]) { // 與我不同的值
			index++; // 下一個位置
			tileArray[index] = a[i]; // 放進來
		}
		tileNumArray[index]++;
	}
	return index + 1;
}

vector<int> Int_Separate(int num) {
	vector<int> result;
	while (num != 0) {
		result.push_back(num % 10);
		num /= 10;
	}
	reverse(result.begin(), result.end());

	return result;
}

string getTime() {
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	string sTime = asctime(timeinfo);
	while (sTime.find(":") != string::npos)
		sTime.replace(sTime.find(":"), 1, "-");
	while (sTime.find("\n") != string::npos)
		sTime.replace(sTime.find("\n"), 1, "-");
	while (sTime.find("\r") != string::npos)
		sTime.replace(sTime.find("\r"), 1, "-");

	sTime += to_string((int)clock());
	return sTime;
}

#endif
