//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Create the listen table for search listen numbers.
//
//=============================================================================
//#define _CRT_SECURE_NO_WARNINGS_

#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <stdexcept>
#include <stack>
#include <chrono>
#include <assert.h>

#include "Table.h"

Table::Table() {
	_card = new int[LISTEN_TABLE_SIZE]; /// 進聽表
	_cardWord = new int[LISTEN_TABLE_CHAR_SIZE]; /// 進聽表

	dismantlingTableResult = new ThrowSet[THROW_TABLE_SIZE];
	dismantlingTableResultWord = new ThrowSet[THROW_TABLE_WORD_SIZE];
	dismantlingTableResult_Eye = new ThrowSet[THROW_TABLE_EYE_SIZE];
	dismantlingTableResultWord_Eye = new ThrowSet[THROW_TABLE_WORD_SIZE];

	dismantlingSize = new int[MAX_ID];
	dismantlingWordSize = new int[MAX_WORD_ID];
	dismantlingEyeSize = new int[MAX_ID];
	dismantlingWordEyeSize = new int[MAX_WORD_ID];

	validTiles_Size = new int[MAX_ID];
	validTilesChar_Size = new int[MAX_WORD_ID];
	validTiles_Eye_Size = new int[MAX_ID];
	validTilesChar_Eye_Size = new int[MAX_WORD_ID];

	validTiles = new char[VALID_TABLE_SIZE];
	validTilesChar = new char[VALID_TABLE_WORD_SIZE];
	validTiles_Eye = new char[VALID_TABLE_EYE_SIZE];
	validTilesChar_Eye = new char[VALID_TABLE_WORD_EYE_SIZE];
}

Table::~Table() {
	delete[] _card;
	delete[] _cardWord;

	delete[] dismantlingTableResult;
	delete[] dismantlingTableResultWord;
	delete[] dismantlingTableResult_Eye;
	delete[] dismantlingTableResultWord_Eye;

	delete[] dismantlingSize;
	delete[] dismantlingWordSize;
	delete[] dismantlingEyeSize;
	delete[] dismantlingWordEyeSize;

	delete[] validTiles_Size;
	delete[] validTilesChar_Size;
	delete[] validTiles_Eye_Size;
	delete[] validTilesChar_Eye_Size;

	delete[] validTiles;
	delete[] validTilesChar;
	delete[] validTiles_Eye;
	delete[] validTilesChar_Eye;
}

// ======= Listen Table ========================================

int Table::caculate(int* pileArray, int* pileNumArray, int group, int length, bool word) { /// 回傳要湊成group組的缺牌數
	caculateNum = 9999; /// 9999是初始值
	haveEyes = false; /// false也是初始狀態
	Find3(pileArray, pileNumArray, 0, length, 0, 0, 0, group, word);
	/*for (int i = start; i < end; i++)
	{


		if (pileNumArray[i] > 0)
		{
			// Throw
			pileNumArray[i]--;
			caculateNum = 9999;
			Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group, false);
			num[0] = caculateNum;
			pileNumArray[i]++;

			// Single
			pileNumArray[i]--;
			caculateNum = 9999;
			Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
			num[1] = caculateNum;
			pileNumArray[i]++;


			if (i + 1 < end)
				if (pileNumArray[i + 1] > 0 && pileArray[i] + 1 == pileArray[i + 1])
				{
					// two hole
					pileNumArray[i]--;
					pileNumArray[i + 1]--;
					caculateNum = 9999;
					Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
					num[4] = caculateNum;
					pileNumArray[i]++;
					pileNumArray[i + 1]++;
				}
			if (i + 2 < end)
			{

				if (pileNumArray[i + 2] > 0 && pileArray[i] + 2 == pileArray[i + 2])
				{
					// Middle hole
					pileNumArray[i]--;
					pileNumArray[i + 2]--;
					caculateNum = 9999;
					Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
					num[5] = caculateNum;
					pileNumArray[i]++;
					pileNumArray[i + 2]++;
				}
				if (pileNumArray[i + 2] > 0 && pileArray[i] + 2 == pileArray[i + 2]
					&& pileNumArray[i + 1] > 0 && pileArray[i] + 1 == pileArray[i + 1])
				{
					// Sequence
					pileNumArray[i]--;
					pileNumArray[i + 1]--;
					pileNumArray[i + 2]--;
					caculateNum = 9999;
					Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
					num[6] = caculateNum;
					pileNumArray[i]++;
					pileNumArray[i + 1]++;
					pileNumArray[i + 2]++;
				}
			}

		}
		if (pileNumArray[i] >= 2)
		{
			// Pairs
			pileNumArray[i] -= 2;
			caculateNum = 9999;
			Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
			num[2] = caculateNum;
			pileNumArray[i] += 2;
		}
		if (pileNumArray[i] >= 3)
		{
			// Connect three
			pileNumArray[i] -= 3;
			caculateNum = 9999;
			Find3(pileArray, pileNumArray, start, end, 0, 0, 0, group - 1, false);
			num[3] = caculateNum;
			pileNumArray[i] += 3;
		}
		for (int a = 0; a < 7; a++)
		{
			//printf("%d ", num[a]);
			if (num[a] < min) min = num[a];
		}
		//printf(" g = %d, tile = %d", group, pileArray[i]);
		//printf("\n");
	}*/

	/// caculateNum最後會被更新成缺牌數
	return caculateNum;
}

// Check if the file exists
int Table::cfileexists(const char* filename) {
	struct stat buffer;
	int exist = stat(filename, &buffer);
	if (exist == 0)
		return 1;
	else // -1
		return 0;
}

int Table::initListenTable() {
	FILE* fp;
	int exist(cfileexists(kListenTableName));

	std::chrono::system_clock::time_point begin, end;
	std::chrono::duration<double> elapsed_time;
	if (!exist) {
		printf("Can't Find Table (%s)\n", kListenTableName);
		printf("Generating Table (%s)\n", kListenTableName);
		fflush(stdout); /// Immediately flush out the contents of an output stream
		fp = fopen(kListenTableName, "wb");
		assert(fp != NULL);

		begin = std::chrono::system_clock::now();
		time_t start_time_t = std::chrono::system_clock::to_time_t(begin); /// to_time_t只是用來轉換型態
		char buff[21];
		strftime(buff, 21, "%Y-%m-%d %H:%M:%S.", localtime(&start_time_t)); /// 把時間存成string格式

		char bit = '!';
		char line = '\n';
		fwrite(&bit, 1, 1, fp);
		fwrite(&buff, sizeof(buff), 1, fp);
		fwrite(&line, 1, 1, fp);
		GenerateListenTable(fp);

		bit = '#';
		fseek(fp, SEEK_SET, 0);
		fwrite(&bit, 1, 1, fp);
		fclose(fp);

		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		printf(">> Finish Generating Table (%s) --- %lf(s)\n", kListenTableName, elapsed_time.count());
		fflush(stdout);
	}
	printf(">> Start Reading Table(%s)\n", kListenTableName);
	begin = std::chrono::system_clock::now();

	char buf[1];
	fp = fopen(kListenTableName, "rb");
	fseek(fp, SEEK_SET, 0);
	fread(buf, 1, 1, fp);

	if (buf[0] == '!') {
		fclose(fp);
		int ret = remove(kListenTableName);

		if (ret == 0) {
			printf("*** Delete the Unfinish Table (%s). ***\n", kListenTableName);
			fflush(stdout);
		}
		else {
			printf("*** Error: Unable to delete the file! ***\n");
			fflush(stdout);
		}
		return 0;
	}
	else //read date
	{
		char date[50];
		fgets(date, 50, fp);
		printf("=== Table Create Time: %s ===\n", date);
		fflush(stdout);
		//fscanf(fp, "%s", date); // Read blank characters
		//while (fscanf(fp, "%d ", &_card[a++]) != EOF);
		//fread((char*)_card, 1, MAX_ID * 13, fp);
		//fread((char*)_cardWord, 1, MAX_WORD_ID * 13, fp);
		for (unsigned i = 1; i < MAX_ID * 13; i++) {
			//if (i % 13)
				fread(&_card[i], 1, 1, fp);
			//else
			//	fread(&_card[i], sizeof(unsigned*), 1, fp);
		}
		for (unsigned i = 1; i < MAX_WORD_ID * 13; i++) {
			//if (i % 13)
				fread(&_cardWord[i], 1, 1, fp);
			//else
			//	fread(&_cardWord[i], sizeof(unsigned*), 1, fp);
		}	
		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;

		fseek(fp, 0L, SEEK_END);
		double file_size(((double)ftell(fp)) / 1024 / 1024);
		//printf("=== File Size : %lfMB ===\n", file_size);
		printf(">> Finish Reading Table(%s)\n\n", kListenTableName);
		fclose(fp);
		printf("+-----------------+-------------+--------------+\n|   Table Name    | Spend Time  |  Table Size  |\n+-----------------+-------------+--------------+\n| %s | %lf(s) | %lf MB |\n+-----------------+-------------+--------------+\n\n"
			, kListenTableName, elapsed_time.count(), file_size);
		fflush(stdout);
		bool eyeE = false;
		int tableID[4] = { 15, 484375, 31250 }; /// 15 = 030000000, 484375 = 000000111, 31250 = 000000200 
		for (int i = 0; i < 3; i++) {
			for (int j = tableID[i] * 13; j < (tableID[i] + 1) * 13; ++j) {
				printf("%d ", _card[j]);
			}
			printf("\n");
		}
		printf("listen : %d\n", getTilesListenNum(5 - 3, tableID, eyeE));
		/*for (int i = abc * 13; i < (abc + 1) * 13; i++) {
			printf("%d ", _cardWord[i]);
		}
		printf("\n");*/
	}
	return 1;
}

int Table::getTilesListenNum(const int& group, int* tableID, int* listenRecordGroup, bool& is_have_eyes) {
	int temp[4] = { 0 };
	int min = 0, minNum = 999, num = 0, record = 0;	
	/*if (takeTile == -1 || throwTile == -1) {
		for (int i = 0; i < 48; i++) {
			result_diff[i] = 255;
		}
		for (int i = 0; i < 4; i++) {
			if (i != 3 && tableID[i] != 0) {
				for (int j = 0; j < 12; j++) {
					result_diff[i*12 + j] = _card[tableID[i] * 13 + 1 + j];
				}
				//printf("ID:%d\n%d %d %d %d %d %d\n\n", _card[ID * 7 + 0], _card[ID * 7 + 1], _card[ID * 7 + 2], _card[ID * 7 + 3], _card[ID * 7 + 4], _card[ID * 7 + 5], _card[ID * 7 + 6]);
			}
			else if (tableID[i] != 0) {
				for (int j = 0; j < 12; j++) {
					result_diff[36 + j] = _cardWord[tableID[i] * 13 + 1 + j];
				}
				//printf("ID:%d\n%d %d %d %d %d\n\n", _cardWord[ID * 6 + 0], _cardWord[ID * 6 + 1], _cardWord[ID * 6 + 2], _cardWord[ID * 6 + 3], _cardWord[ID * 6 + 4], _cardWord[ID * 6 + 5]);
			}
		}
	}
	else {
		if (takePos != 3 && tableID[takePos] != 0) {
			for (int c = 0; c < 12; c++) {
				result_diff[takePos*12 + c] = _card[tableID[takePos] * 13 + 1 + c];
			}
			//printf("ID:%d\n%d %d %d %d %d %d\n\n", _card[ID * 7 + 0], _card[ID * 7 + 1], _card[ID * 7 + 2], _card[ID * 7 + 3], _card[ID * 7 + 4], _card[ID * 7 + 5], _card[ID * 7 + 6]);
		}
		else if (tableID[takePos] != 0) {
			for (int c = 0; c < 12; c++) {
				result_diff[36 + c] = _cardWord[tableID[takePos] * 13 + 1 + c];
			}
			//printf("ID:%d\n%d %d %d %d %d\n\n", _cardWord[ID * 6 + 0], _cardWord[ID * 6 + 1], _cardWord[ID * 6 + 2], _cardWord[ID * 6 + 3], _cardWord[ID * 6 + 4], _cardWord[ID * 6 + 5]);
		}
		else {
			for (int a = 0; a < 12; a++) {
				result_diff[takePos * 12 + a] = 255;
			}
		}

		if (takePos != throwPos) {
			if (throwPos != 3 && tableID[throwPos] != 0) {
				for (int c = 0; c < 12; c++) {
					result_diff[throwPos*12 + c] = _card[tableID[throwPos] * 13 + 1 + c];
				}
				//printf("ID:%d\n%d %d %d %d %d %d\n\n", _card[ID * 7 + 0], _card[ID * 7 + 1], _card[ID * 7 + 2], _card[ID * 7 + 3], _card[ID * 7 + 4], _card[ID * 7 + 5], _card[ID * 7 + 6]);
			}
			else if (tableID[throwPos] != 0) {
				for (int c = 0; c < 12; c++) {
					result_diff[36 + c] = _cardWord[tableID[throwPos] * 13 + 1 + c];
				}
				//printf("ID:%d\n%d %d %d %d %d\n\n", _cardWord[ID * 6 + 0], _cardWord[ID * 6 + 1], _cardWord[ID * 6 + 2], _cardWord[ID * 6 + 3], _cardWord[ID * 6 + 4], _cardWord[ID * 6 + 5]);
			}
			else {
				for (int a = 0; a < 12; a++) {
					result_diff[throwPos * 12 + a] = 255;
				}
			}
		}
	}*/

	is_have_eyes = false;

	while (num != group) {
		minNum = 999;
		for (int i = 0; i < 4; i++) {
			if (tableID[i] == 0) continue;
			if (i != 3 && minNum > _card[tableID[i] * 13 + temp[i] + 1]) {
				minNum = _card[tableID[i] * 13 + temp[i] + 1];
				record = i;
			}
			else if (i == 3 && minNum > _cardWord[tableID[i] * 13 + temp[i] + 1]) {
				minNum = _cardWord[tableID[i] * 13 + temp[i] + 1];
				record = i;
			}
		}
		min += minNum;
		temp[record]++;
		num++;
	}
	bool notFound(false);
	//int change_temp = -1;
	for (int i = 0; i < 4; i++) {
		if (tableID[i] == 0) continue;
		if (i != 3) {
			if (_card[tableID[i] * 13 + temp[i] + 1] == 1 && _card[tableID[i] * 13 + 6 + temp[i] + 1] == 89) {
				min--;
				//temp[i]++;
				is_have_eyes = true;
				break;
			}
			else if (_card[tableID[i] * 13 + temp[i] + 1] == 1) {
				notFound = true;
				//change_temp = i;
			}
		}
		else {
			if (_cardWord[tableID[i] * 13 + temp[i] + 1] == 1 && _cardWord[tableID[i] * 13 + 6 + temp[i] + 1] == 89) {
				min--;
				//temp[i]++;
				is_have_eyes = true;
				break;
			}
			else if (_cardWord[i * 13 + temp[i] + 1] == 1) {
				notFound = true;
				//change_temp = i;
			}
		}
	}
	if (!is_have_eyes && notFound) {
		for (int i = 0; i < 4; i++) {
			if (tableID[i] == 0) continue;
			if (i != 3 && temp[i] > 0 && _card[tableID[i] * 13 + temp[i]] == 1 && _card[tableID[i] * 13 + 5 + temp[i] + 1] == 89) {
				min--;
				//temp[i]++;
				//temp[change_temp]++;
				is_have_eyes = true;
				break;
			}
			else if (i == 3 && temp[i] > 0 && _cardWord[tableID[i] * 13 + temp[i]] == 1 && _cardWord[tableID[i] * 13 + 5 + temp[i] + 1] == 89) {
				min--;
				is_have_eyes = true;
				break;
			}
		}
	}
	for (int i = 0; i < 4; i++) { // for valid tile
		listenRecordGroup[i] = temp[i];
	}
	return min;
}

//上廳數?
inline int Table::getTilesListenNum(const int& group, int* tableID, bool& is_have_eyes) {
	int temp[4] = { 0 };
	int min = 0, minNum = 999, num = 0, record = 0;

	is_have_eyes = false;

	while (num != group) {
		minNum = 999; /// 最小上聽數
		for (int i = 0; i < 4; ++i) {
			if (tableID[i] == 0) continue; /// 若該花色沒有牌就換檢查下一個花色
			if (i != 3 && minNum > _card[(tableID[i] * 13) + temp[i] + 1]) { /// 數牌
				minNum = _card[tableID[i] * 13 + temp[i] + 1];
				record = i;
			}
			else if (i == 3 && minNum > _cardWord[(tableID[i] * 13) + temp[i] + 1]) {
				minNum = _cardWord[tableID[i] * 13 + temp[i] + 1];
				record = i;
			}
		}
		min += minNum;
		temp[record]++;
		num++;
	}
	//有沒有找到什麼????
	bool notFound(false);
	//int change_temp = -1;
	for (int i = 0; i < 4; ++i) {
		if (tableID[i] == 0) continue;
		if (i != 3) {
			if (_card[tableID[i] * 13 + temp[i] + 1] == 1 && _card[tableID[i] * 13 + 6 + temp[i] + 1] == 89) {
				min--;
				//temp[i]++;
				is_have_eyes = true;
				break;
			}
			else if (_card[tableID[i] * 13 + temp[i] + 1] == 1) {
				notFound = true;
				//change_temp = i;
			}
		}
		else {
			if (_cardWord[tableID[i] * 13 + temp[i] + 1] == 1 && _cardWord[tableID[i] * 13 + 6 + temp[i] + 1] == 89) {
				min--;
				//temp[i]++;
				is_have_eyes = true;
				break;
			}
			else if (_cardWord[i * 13 + temp[i] + 1] == 1) {
				notFound = true;
				//change_temp = i;
			}
		}
	}
	if (!is_have_eyes && notFound) {
		for (int i = 0; i < 4; ++i) {
			if (tableID[i] == 0) continue;
			if (i != 3 && temp[i] > 0/* && _card[tableID[i] * 13 + temp[i]] == 1 && _card[tableID[i] * 13 + 5 + temp[i] + 1] == 89*/) {
				for (int j = temp[i]; j > 0; j--) {
					if (_card[tableID[i] * 13 + j] == 1 && _card[tableID[i] * 13 + 5 + j + 1] == 89) {
						min--;
						is_have_eyes = true;
						break;
					}
					else if (_card[tableID[i] * 13 + temp[i]] != 1) break;
				}
				//min--;
				//is_have_eyes = true;
				if(is_have_eyes) break;
			}
			else if (i == 3 && temp[i] > 0 /* && _cardWord[tableID[i] * 13 + temp[i]] == 1 && _cardWord[tableID[i] * 13 + 5 + temp[i] + 1] == 89*/) {
				for (int j = temp[i]; j > 0; j--) {
					if (_cardWord[tableID[i] * 13 + j] == 1 && _cardWord[tableID[i] * 13 + 5 + j + 1] == 89) {
						min--;
						is_have_eyes = true;
						break;
					}
					else if (_card[tableID[i] * 13 + temp[i]] != 1) break;
				}
				//min--;
				//is_have_eyes = true;
				if (is_have_eyes) break;
			}
		}
	}

	return min;
}

int Table::getTilesListenNum(const int& group, int* pileArray, int* pileNumArray, const int& length, bool& is_have_eyes) {
	unsigned int ID = 0;
	int result_diff[48];
	for (int i = 0; i < 48; i++) {
		result_diff[i] = 255;
	}

	for (int i = 0; i < length; i++) {
		ID += pileNumArray[i] * powFive[pileArray[i] % 9];

		if ((i + 1 < length && pileArray[i] / 9 != pileArray[i + 1] / 9) || i + 1 == length) {
			int pos = pileArray[i] / 9;
			if (ID == 0) {
				for (int i = 0; i < 12; i++) {
					result_diff[pos*12 + i] = 255;
				}
				continue;
			}
			//printf("Decimal ID:%d\n", ID);
			if (pos != 3) {
				for (int c = 0; c < 12; c++) {
					result_diff[pos*12+ c] = _card[ID * 13 + 1 + c];
				}
				//printf("ID:%d\n%d %d %d %d %d %d\n\n", _card[ID * 7 + 0], _card[ID * 7 + 1], _card[ID * 7 + 2], _card[ID * 7 + 3], _card[ID * 7 + 4], _card[ID * 7 + 5], _card[ID * 7 + 6]);
			}
			else {
				for (int c = 0; c < 12; c++) {
					result_diff[36 + c] = _cardWord[ID * 13 + 1 + c];
				}
				//printf("ID:%d\n%d %d %d %d %d %d\n\n", _cardWord[ID * 7 + 0], _cardWord[ID * 7 + 1], _cardWord[ID * 7 + 2], _cardWord[ID * 7 + 3], _cardWord[ID * 7 + 4], _cardWord[ID * 7 + 5], _cardWord[ID * 7 + 6]);
			}
			ID = 0;
		}
	}
	is_have_eyes = false;
	int min = 0, minNum = 999, temp[4] = { 0 }, num = 0, record = 0;
	while (num != group) {
		minNum = 999;
		for (int i = 0; i < 4; i++) {
			if (minNum > result_diff[i * 12 + temp[i]]) {
				minNum = result_diff[i * 12 + temp[i]];
				record = i;
			}
		}
		min += minNum;
		temp[record]++;
		num++;
	}
	bool notFound = false;
	for (int i = 0; i < 4; i++) {
		if (result_diff[i * 12 + temp[i]] == 1 && result_diff[i * 12 + 6 + temp[i]] == 89) {
			min--;
			//temp[i]++;
			is_have_eyes = true;
			break;
		}
		else if (result_diff[i * 12 + temp[i]] == 1) {
			notFound = true;
			//change_temp = i;
		}
	}
	if (!is_have_eyes && notFound) {
		for (int i = 0; i < 4; i++) {
			if (temp[i] > 0 && result_diff[i * 12 + temp[i] - 1] == 1 && result_diff[i * 12 + 5 + temp[i]] == 89) {
				min--;
				//temp[i]++;
				//temp[change_temp]++;
				is_have_eyes = true;
				break;
			}
		}
	}
	return min;
}

void Table::FindSequence(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g) {
	/// 找順子
	int tempNumArray[17] = { 0 };
	for (int i = 0; i < end; i++) /// copy一份
		tempNumArray[i] = pileNumArray[i];
	for (int i = start; i < end - 2; i++) {
		/// 順子條件：連續三個牌種都有牌，且這三個牌種的數字是連續的 (因為餵進來的tiles本身就同花色，所以不用檢查花色)
		if (i >= 0 && i < 17 && tempNumArray[i] > 0 && tempNumArray[i + 1] > 0 && tempNumArray[i + 2] > 0 &&
			pileArray[i] + 1 == pileArray[i + 1] && pileArray[i] + 2 == pileArray[i + 2]) {
			tempNumArray[i]--;
			tempNumArray[i + 1]--;
			tempNumArray[i + 2]--;
			group++; // 組數++
			FindSequence(pileArray, tempNumArray, i, end, group, two, one, g); /// 繼續找
			tempNumArray[i]++; /// 復原
			tempNumArray[i + 1]++; /// 復原
			tempNumArray[i + 2]++; /// 復原
			group--; /// 復原
		}
	}
	/// 執行到這裡就是都沒有順子了，接著找對子
	FindPair(pileArray, tempNumArray, 0, end, group, two, one, g, 0, false);
}

void Table::Find3(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g, bool word) {
	/// pileArray是牌種集合
	/// pileNumArray是該牌種的張數
	/// group 目前組數
	/// two是目前搭數(包含眼牌)
	/// one是目前孤張數
	/// start到end是湊組範圍
	/// g是目標組數
	/// word代表目前為進來的tiles是否為字牌
	
	int tempNumArray[17] = { 0 };
	if (g == 0) caculateNum = 0;
	for (int i = 0; i < end; i++)
		tempNumArray[i] = pileNumArray[i]; /// copy一份
	for (int i = start; i < end; i++) {
		if (i >= 0 && i < 17 && tempNumArray[i] >= 3) { /// 刻子
			tempNumArray[i] -= 3;
			group++; // 組數+=1
			Find3(pileArray, tempNumArray, i + 1, end, group, two, one, g, word); /// 繼續往下找
			tempNumArray[i] += 3; /// 復原
			group--; /// 復原
		}
	}
	/// 執行到這裡就是沒有刻子了，因為如果還有的話會在for裡繼續呼叫Find3
	/// 如果目前這些牌是字牌，則直接檢查對子，一開始還沒檢查過，所以eyes = 0
	if (word) FindPair(pileArray, tempNumArray, 0, end, group, two, one, g, 0, true);
	/// 如果這些牌是數字牌，接著檢查順子
	else FindSequence(pileArray, tempNumArray, 0, end, group, two, one, g);
}

void Table::FindOne(int* pileArray, int* pileNumArray, int length, int group, int two, int one, int g, int eyes) {
	/// 找孤張
	int count = 0;

	if (group >= g)
		caculateNum = 0;
	else {
		for (int i = 0; i < length; i++) {
			if (pileNumArray[i] > 0) { /// 剩下的牌都是孤張
				one++;
			}
		}
		//printf("\ngroup = %d, %d, %d g = %d\n", group, two, one, g);
		/// 如果目前組數比目標組數少時，會把一些不成組的搭子納入group中
		/// 再用count計算目前group中如果要湊成目標組數的缺牌數
		while (group < g) {
			/// 先把搭子或對子列入group
			if (two > 0) {
				group++;
				two--;
				count++; /// 每搭要變成組都差一張
			}
			/// 沒有搭子或對子就加入孤張
			else if (one > 0) {
				group++;
				one--;
				count += 2; /// 每個孤張要變成組都差兩張
			}
			/// 沒有搭子或對子或孤張就只能直接算g組與group組相差的牌數
			else {
				count += (g - group) * 3; /// 從沒有組到有組差3張牌，缺(g - group)組
				break;
			}
		}
		if (caculateNum >= count) {
			/// caculateNum在這之前要不是0要不是9999
			/// 如果calculateNum是0，代表找刻子時目標組數為0或是找孤張時目前組數已經>=目標組數
			if (caculateNum > count) haveEyes = false; /// 如果是以上兩種情況，不可能有眼牌
			caculateNum = count;
			if (eyes > 0) haveEyes = true;
		}
	}
}

void Table::FindPair(int* pileArray, int* pileNumArray, int start, int end, int group, int two, int one, int g, int eyes, bool word) {
	/// 找對子
	int tempNumArray[17] = { 0 };
	for (int i = 0; i < end; i++)
		tempNumArray[i] = pileNumArray[i]; // copy一份
	for (int i = start; i < end; i++) {
		if (i >= 0 && i < 17 && tempNumArray[i] >= 2) { /// 對子條件
			tempNumArray[i] -= 2;
			two++;
			eyes++;
			FindPair(pileArray, tempNumArray, i + 1, end, group, two, one, g, eyes, word);
			tempNumArray[i] += 2; /// 復原
			two--; /// 復原
			eyes--; /// 復原
		}
	}
	/// 沒有對子了
	/// 如果餵進來的tiles為字牌，則直接找孤張
	if (word) FindOne(pileArray, tempNumArray, end, group, two, one, g, eyes);
	/// 如果餵進來的牌是數字牌，則找需要聽洞的搭子
	else FindHole(pileArray, tempNumArray, end, group, two, one, g, eyes);
}

void Table::FindHole(int* pileArray, int* pileNumArray, int length, int group, int two, int one, int g, int eyes) {
	for (int i = 0; i < length - 1; i++) {
		if (pileArray[i] / 9 == 3) break; /// 字牌不用檢查洞
		// Find the starting point and ending point, a is the starting point, b is the ending point, count to see if both have values
		if (pileNumArray[i] <= 0) continue; /// 找到有牌處開始處理
		for (int j = i + 1; j < length; j++) {
			/// 如果該牌種沒有牌或兩牌種花色不同就跳過不處理
			/// 因為如果有湊成洞會再把j--，檢查湊成洞的兩張牌牌數都不為0的狀況，所以每次還是要檢查i牌牌數是否>0
			if (pileNumArray[i] <= 0 || pileNumArray[j] <= 0 || pileArray[i] / 9 != pileArray[j] / 9) continue;
			/// 如果i牌的數字還可以更大一個數(i要在8以下)且i牌與j牌是兩張數字相鄰的牌(like 23 or 78)
			if (pileArray[i] % 9 + 1 < 9 && pileArray[i] + 1 == pileArray[j]) {
				pileNumArray[i]--;
				pileNumArray[j]--;
				two++; /// 搭數+=1
				j--; /// 說不定23都有不只一張牌，所以再檢查一次，如果都只有一張牌會在上上個if往下執行for loop
			}
			/// 如果i牌的數字還可以更大兩個數且是兩張數(i要在7以下)且i牌與i牌是兩張數字差2的牌(like 79)
			else if (pileArray[i] % 9 + 2 < 9 && pileArray[i] + 2 == pileArray[j]) {
				pileNumArray[i]--;
				pileNumArray[j]--;
				two++; /// 搭數+=1
				j--; /// 再檢查一次
			}
		}
	}
	/// 需要聽洞的搭子都找完之後就剩孤張了，找孤張
	FindOne(pileArray, pileNumArray, length, group, two, one, g, eyes);
}

void Table::GenerateListenTable(FILE* fp) {
	int tempArray[9] = { 0,1,2,3,4,5,6,7,8 }; /// 9個手牌數字
	int tempNumArray[9] = { 0 }; /// 存每一種可能性手牌每個數字的數量
	unsigned powFiveID = 0;

	/// 13是每個手牌各有一個的進聽數差值表
	int* card = new int[MAX_ID * 13]; /// MAX_ID = 5^9 /// 進聽差值表
	int* cardTemp = new int[MAX_ID * 13]; /// 湊成固定組數的缺牌數(前後相減作成進聽差值表)
	int* cardWord = new int[MAX_WORD_ID * 13]; /// MAX_WORD_ID = 5^7
	int* cardWordTemp = new int[MAX_WORD_ID * 13];
	for (unsigned i = 1; i < MAX_ID; i++) { // Generate all possible condition
		unsigned a = i, j = 0;
		unsigned b = 1;
		powFiveID = 0;
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		while (a) { /// 用10進位的bit表示5進位(10進位每個位數都只有0~5)
			tempNumArray[j++] = a % 5; /// 把牌種的數量存到陣列
			powFiveID += ((a % 5) * b);
			b *= 10;
			a /= 5;
		}
		card[i * 13] = 255;//powFiveID;
		for (int c = 0; c < 6; c++) {
			/// 先算出單純的缺牌數，寫到cardTemp
			cardTemp[i * 13 + c + 1] = caculate(tempArray, tempNumArray, c + 1, 9, false); //because i*11 is the ID location, so +1 
			/// 有沒有眼牌的資訊可以直接更新到進聽數差值表裡
			if (haveEyes) {
				card[i * 13 + 7 + c] = 89; /// 'Y'的ASCII
			}
			else {
				card[i * 13 + 7 + c] = 78; /// 'N'的ASCII
			}
		}
		/// 因為目標組數為0時缺牌數必為0，所以差值表的第一個位置不用跟前一項相減(-0無意義)
		card[i * 13 + 1] = cardTemp[i * 13 + 1];
		for (int c = 1; c < 6; c++) {
			/// 進聽數差值表除了第一項的其他項為缺牌數表(cardTemp)裡前後兩項相減
			card[i * 13 + c + 1] = cardTemp[i * 13 + c + 1] - cardTemp[i * 13 + c];
		}
	}

	for (unsigned i = 1; i < MAX_ID * 13; i++) {
		//if (i % 13)
			fwrite((char*)&card[i], 1, 1, fp);
		//else
		//	fwrite(&card[i], sizeof(unsigned*), 1, fp);
	}
	/// 字牌的部分
	for (unsigned i = 1; i < MAX_WORD_ID; i++) { // Generate all possible condition
		unsigned a = i, j = 0;
		unsigned b = 1;
		powFiveID = 0;
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		while (a) {
			tempNumArray[j++] = a % 5;
			powFiveID += ((a % 5) * b);
			b *= 10;
			a /= 5;
		}
		cardWord[i * 13] = 255;// powFiveID;
		for (int c = 0; c < 6; c++) {
			cardWordTemp[i * 13 + c + 1] = caculate(tempArray, tempNumArray, c + 1, 7, true); //because i*11 is the ID location, so +1 
			if (haveEyes) {
				cardWord[i * 13 + 7 + c] = 89;
			}
			else {
				cardWord[i * 13 + 7 + c] = 78;
			}
		}
		cardWord[i * 13 + 1] = cardWordTemp[i * 13 + 1];
		for (int c = 1; c < 6; c++) {
			cardWord[i * 13 + c + 1] = cardWordTemp[i * 13 + c + 1] - cardWordTemp[i * 13 + c];
		}
	}

	for (unsigned i = 1; i < MAX_WORD_ID * 13; i++) {
		//if (i % 13)
			fwrite((char*)&cardWord[i], 1, 1, fp);
		//else
		//	fwrite(&cardWord[i], sizeof(unsigned*), 1, fp);
	}
	delete[] card;
	delete[] cardWord;
	delete[] cardTemp;
	delete[] cardWordTemp;
	//fclose(fp);
}

// ======= Dismantling Table ===================================

int Table::initDismantlingTable() {
	FILE* fp;
	int exist(cfileexists(kDismantlingTableName));

	std::chrono::system_clock::time_point begin, end;
	std::chrono::duration<double> elapsed_time;
	if (!exist) {
		printf("Can't Find Table (%s)\n", kDismantlingTableName);
		printf("Generating Table (%s)\n", kDismantlingTableName);
		fflush(stdout);
		fp = fopen(kDismantlingTableName, "wb");

		begin = std::chrono::system_clock::now();
		time_t start_time_t(std::chrono::system_clock::to_time_t(begin));
		char buff[21];
		strftime(buff, 21, "%Y-%m-%d %H:%M:%S.", localtime(&start_time_t));

		char bit = '!';
		char line = '\n';
		fwrite(&bit, 1, 1, fp);
		fwrite(&buff, sizeof(buff), 1, fp);
		fwrite(&line, 1, 1, fp);

		GenerateDismantlingTable(fp);

		// reset the check bit to 1
		bit = '#';
		fseek(fp, SEEK_SET, 0);
		fwrite(&bit, 1, 1, fp);

		fclose(fp);
		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		printf(">> Finish Generating Table (%s) --- %lf(s)\n", kDismantlingTableName, elapsed_time.count());
		fflush(stdout);
	}
	printf(">> Start Reading Table(%s)\n", kDismantlingTableName);
	begin = std::chrono::system_clock::now();

	char bit[1];
	fp = fopen(kDismantlingTableName, "rb");
	fseek(fp, SEEK_SET, 0);
	fread(bit, 1, 1, fp);

	// !: unfinish #: finish
	if (bit[0] != '#') {
		// calculate file size
		fclose(fp);
		int ret = remove(kDismantlingTableName);

		if (ret == 0) {
			printf("*** Delete the Unfinish Table (%s). ***\n", kDismantlingTableName);
			fflush(stdout);
		}
		else {
			printf("*** Error: Unable to delete the file! ***\n");
			fflush(stdout);
		}
		return 0;
	}
	else {
		// read created date
		char date[50];
		fgets(date, 50, fp);
		printf("=== Table Create Time: %s ===\n", date);
		fflush(stdout);

		for (int i = 0; i < THROW_TABLE_SIZE; i++) {
			fread((char*)&dismantlingTableResult[i], sizeof(ThrowSet), 1, fp);
		}
		fread(dismantlingSize, sizeof(int), MAX_ID, fp);
		
		for (int i = 0; i < THROW_TABLE_WORD_SIZE; i++) {
			fread((char*)&dismantlingTableResultWord[i], sizeof(ThrowSet), 1, fp);
		}
		fread((char*)dismantlingWordSize, sizeof(int), MAX_WORD_ID, fp);

		for (int i = 0; i < THROW_TABLE_EYE_SIZE; i++) {
			fread(&dismantlingTableResult_Eye[i], sizeof(ThrowSet), 1, fp);
		}
		fread(dismantlingEyeSize, sizeof(int), MAX_ID, fp);

		for (int i = 0; i < THROW_TABLE_WORD_SIZE; i++) {
			fread((char*)&dismantlingTableResultWord_Eye[i], sizeof(ThrowSet), 1, fp);
		}
		fread(dismantlingWordEyeSize, sizeof(int), MAX_WORD_ID, fp);

		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		// calculate file size
		double file_size = ((double)ftell(fp)) / 1024 / 1024;
		fclose(fp);
		//printf("=== File Size : %lfMB ===\n", file_size);
		printf(">> Finish Reading Table(%s)\n\n", kDismantlingTableName);
		printf("+----------------------+-------------+---------------+\n|      Table Name      | Spend Time  |  Table Size   |\n+----------------------+-------------+---------------+\n| %s | %lf(s) | %lf MB |\n+----------------------+-------------+---------------+\n\n"
			, kDismantlingTableName, elapsed_time.count(), file_size);
		//printf("%d\n", (int)sizeof(ThrowSet));
		//printf("%d\n", dismantlingSize[78275]);
		//for(int i = 0; i < 10; i++) printf("%d ", dismantlingSize[i]);
		/*for (int i = dismantlingSize[417315]; i < dismantlingSize[417316]; i++) {
			for (unsigned j = 0; j < dismantlingTableResult[i].pairSize; j++) {
				printf("%d %d, ", dismantlingTableResult[i].pair[j], dismantlingTableResult[i].pair[j]);
			}
			for (unsigned j = 0; j < dismantlingTableResult[i].tatsuSize * 2; j += 2) {
				printf("%d %d, ", dismantlingTableResult[i].tatsu[j], dismantlingTableResult[i].tatsu[j + 1]);
			}
			for (unsigned j = 0; j < dismantlingTableResult[i].oneSize; j++) {
				printf("%d, ", dismantlingTableResult[i].one[j]);
			}
			printf("\n");
		}
		printf("\n"); //78276 82151 82030

		int maxSize = 0, ids = 0;
		for (int i = 1; i < MAX_ID; i++) {
			if (maxSize < dismantlingSize[i] - dismantlingSize[i - 1]) {
				maxSize = dismantlingSize[i] - dismantlingSize[i - 1];
				ids = i - 1;
			}
		}
		printf("%d %d\n", maxSize, ids);*/
		
		//printf("Max: %d %d %d\n", max1, max2, max3); 
		// dismantlingTableResult: 2 7 40, dismantlingTableResultWord: 2 7 1, dismantlingTableResult_Eye: 2 6 31, dismantlingTableResult_Eye: 2 7 1
		fflush(stdout);
	}

	return 1;
}

inline std::string Table::vectorToString(vector<vector<int>> result) {
	std::string temp;
	for (unsigned i = 0; i < result.size(); i++) {
		for (unsigned j = 0; j < result[i].size(); j++)
			temp += result[i][j];
		temp += ",";
	}
	return temp;
}

vector<vector<vector<int>>> Table::findThrowlist(int* tempArray, int* tempNumArray, const int& length, bool eyeTable) {
	vector<int> temp;
	vector<vector<int>> result;
	vector<vector<vector<int>>> newDismantlingResult;
	dismantlingResult.clear();
	u_map.clear();
	// Dismantling hand
	FindSequence(tempArray, tempNumArray, length, result, true, 0);
	int minNotGroup = 999;
	for (unsigned a = 0; a < dismantlingResult.size(); a++) {
		int notGroup = dismantlingResult[a].size();
		if (eyeTable) {
			notGroup--;
			temp.push_back(notGroup);
		}
		else {
			bool findEyes = false;
			for (unsigned i = 0; i < dismantlingResult[a].size(); i++) {
				if (dismantlingResult[a][i].size() == 2 && dismantlingResult[a][i][0] == dismantlingResult[a][i][1]) {
					findEyes = true;
					notGroup--;
					temp.push_back(notGroup);
					break;
				}
			}
			if (!findEyes) {
				//notGroup++;
				temp.push_back(notGroup);
			}
		}
		if (notGroup < minNotGroup) minNotGroup = notGroup;
	}
	for (unsigned a = 0; a < dismantlingResult.size(); a++) {
		if (temp[a] == minNotGroup) newDismantlingResult.push_back(dismantlingResult[a]);
	}

	return newDismantlingResult;
}

int Table::getDismantling(int* tableID, bool& is_have_eyes, ThrowSet* dismantlingSearchResult, int *startPosition, int throwContentType[][4]) /*const*/ {
	ThrowSet throwSet[50];
	int throwSetPosition[5] = { 0 }, typeArray[4] = { 0 };
	int typeArraySize(0), length = 0, sum = 0, startPositionSize = 0;
	for (int i = 0; i < 3; i++) {
		if (tableID[i]) {
			if (is_have_eyes && (dismantlingEyeSize[tableID[i]] - dismantlingEyeSize[tableID[i] - 1]) != 0) {
				for (int j = dismantlingEyeSize[tableID[i] - 1]; j < dismantlingEyeSize[tableID[i]]; j++) {
					throwSet[length++] = dismantlingTableResult_Eye[j];
				}
				throwSetPosition[typeArraySize] = sum;
				sum += dismantlingEyeSize[tableID[i]] - dismantlingEyeSize[tableID[i] - 1];
				typeArray[typeArraySize++] = i;
			}
			else if ((dismantlingSize[tableID[i]] - dismantlingSize[tableID[i] - 1]) != 0) {
				for (int j = dismantlingSize[tableID[i] - 1]; j < dismantlingSize[tableID[i]]; j++) {
					throwSet[length++] = dismantlingTableResult[j];
				}
				throwSetPosition[typeArraySize] = sum;
				sum += dismantlingSize[tableID[i]] - dismantlingSize[tableID[i] - 1];
				typeArray[typeArraySize++] = i;
			}
		}
	}
	if (tableID[3]) {
		if (is_have_eyes && (dismantlingWordEyeSize[tableID[3]] - dismantlingWordEyeSize[tableID[3] - 1]) != 0) {
			for (int j = dismantlingWordEyeSize[tableID[3] - 1]; j < dismantlingWordEyeSize[tableID[3]]; j++) {
				throwSet[length++] = dismantlingTableResultWord_Eye[j];
			}
			throwSetPosition[typeArraySize++] = sum;
			sum += dismantlingWordEyeSize[tableID[3]] - dismantlingWordEyeSize[tableID[3] - 1];
		}
		else if ((dismantlingWordSize[tableID[3]] - dismantlingWordSize[tableID[3] - 1]) != 0) {
			for (int j = dismantlingWordSize[tableID[3] - 1]; j < dismantlingWordSize[tableID[3]]; j++) {
				throwSet[length++] = dismantlingTableResultWord[j];
			}
			throwSetPosition[typeArraySize++] = sum;
			sum += dismantlingWordSize[tableID[3]] - dismantlingWordSize[tableID[3] - 1];
		}
	}
	throwSetPosition[typeArraySize] = sum;

	if (length == 0) {
		printf("***** FindDismantlingGroup Error!, size = 0 *****\n");
		fflush(stdout);
		throw std::invalid_argument("FindDismantlingGroup Error!");
	}
	//printf("%d\n", length);
	if (length > 50) {
		printf("getDismantling length Overed: %d\n", length);
		fflush(stdout);
		throw std::out_of_range("length Overed.");		
	}

	// DFS find group
	std::stack<vector<ThrowSet>> stack;
	std::stack<int*> typeStack;
	std::stack<int> labels;
	int temp[4];
	int index = 0;
	int minNotGroup = 99;

	for (int i = 0; i < throwSetPosition[1]; i++) { //get 3 length 3 value
		vector<ThrowSet> result;
		result.resize(4);
		result[0] = throwSet[i];
		temp[0] = typeArray[0];
		stack.push(result);
		typeStack.push(temp);
		labels.push(0);
	}

	while (!stack.empty()) {
		vector<ThrowSet> topResult = stack.top();
		int* content = typeStack.top();
		int pos(labels.top() + 1);
		stack.pop();
		typeStack.pop();
		labels.pop();	

		if (typeArraySize > pos) {
			for (int i = throwSetPosition[pos]; i < throwSetPosition[pos + 1]; i++) //get 3 length 3 value
			{
				vector<ThrowSet> tempResult = topResult;
				int* tempConent = content;
				tempConent[pos] = typeArray[pos];
				tempResult[pos] = throwSet[i];
				stack.push(tempResult);
				typeStack.push(tempConent);
				labels.push(pos);
			}
		}
		else if (pos != 0) {
			int groups = 0;
			bool findEye = false;
			// find the best group of dismantling result
			for (int i = 0; i < pos; i++) {
				groups += topResult[i].oneSize + topResult[i].tatsuSize + topResult[i].pairSize;
				// find eyes
				if (!findEye && topResult[i].pairSize > 0) {
					findEye = true;
					groups--;
				}
			}
			if (groups < minNotGroup) {
				minNotGroup = groups;
				index = 0;
				startPositionSize = 0;
				for (int i = index, g = 0; i < index + pos; i++, g++) {
					dismantlingSearchResult[i] = topResult[g];
					throwContentType[startPositionSize][g] = content[g];
				}
				startPosition[startPositionSize++] = index;
				index += pos;
			}
			else if (groups == minNotGroup) {
				for (int i = index, g = 0; i < index + pos; i++, g++) {
					dismantlingSearchResult[i] = topResult[g];
					throwContentType[startPositionSize][g] = content[g];
				}
				startPosition[startPositionSize++] = index;
				index += pos;
			}

			if (index >= 250) {
				printf("%d %d %d %d\n", tableID[0], tableID[1], tableID[2], tableID[3]);
				printf("%d\n", index);
				printf("index Overed\n");
				fflush(stdout);
				throw std::out_of_range("index Overed."); //131875 82150 35000
			}
			else if (startPositionSize >= 90) {
				printf("%d\n", startPositionSize);
				printf("startPositionSize Overed\n");
				fflush(stdout);
				throw std::out_of_range("startPositionSize Overed.");
			}
		}
	}
	startPosition[startPositionSize] = index;

	//delete[] throwSet;
	return startPositionSize;

	//return FindDismantlingGroup(throwSet, dismantlingSearchResult, throwSetPosition, typeArray, startPosition, typeArraySize, throwContentType);
}

// DFS Non-recussive 
inline int Table::FindDismantlingGroup(ThrowSet *throwSet, ThrowSet* dismantlingSearchResult, int* throwSetPosition, int* typeArray, int *startPosition, int &typeArraySize, int throwContentType[][4]) const{
	std::stack<vector<ThrowSet>> stack;
	std::stack<vector<int>> typeStack;
	std::stack<int> labels;
	vector<int> temp;
	int index = 0, startPositionSize = 0;

	temp.resize(4);

	for (int i = 0; i < throwSetPosition[1]; i++) { //get 3 length 3 value
		vector<ThrowSet> result;
		result.resize(4);		
		result[0] = throwSet[i];
		temp[0] = typeArray[0];
		stack.push(result);
		typeStack.push(temp);
		labels.push(0);
	}

	while (!stack.empty()) {
		vector<ThrowSet> topResult = stack.top();
		vector<int> content = typeStack.top();
		int pos(labels.top() + 1);
		stack.pop();
		typeStack.pop();
		labels.pop();

		if (typeArraySize > pos) {
			for (int i = throwSetPosition[pos]; i < throwSetPosition[pos + 1]; i++) //get 3 length 3 value
			{
				vector<ThrowSet> tempResult = topResult;
				vector<int> tempConent = content;
				tempConent[pos] = typeArray[pos];
				tempResult[pos] = throwSet[i];
				stack.push(tempResult);
				typeStack.push(tempConent);
				labels.push(pos);
			}
		}
		else if (pos != 0) {
			for (int i = index, g = 0; i < index + pos; i++, g++) {
				dismantlingSearchResult[i] = topResult[g];
				throwContentType[startPositionSize][g] = content[g];
			}
			startPosition[startPositionSize++] = index;
			index += pos;
		}
	}
	startPosition[startPositionSize] = index;

	delete[] throwSet;
	return startPositionSize;
}

inline vector<vector<vector<int>>> Table::FindDismantlingGroup(ThrowSet* throwSet, int* throwSetPosition, int* typeArray, int& typeArraySize) const {
	std::stack<vector<vector<int>>> stack;
	vector<vector<vector<int>>> dismantlingSearchResult;
	vector<int> labels;
	labels.reserve(150);

	for (int i = 0; i < throwSetPosition[1]; i++) //get 3 length 3 value
	{
		vector<vector<int>> tempResult;
		for (int j = 0; j < throwSet[i].pairSize; j++) {
			vector<int> b(2, throwSet[i].pair[j] + typeArray[0] * 9);
			tempResult.push_back(b);
		}
		for (int j = 0; j < throwSet[i].tatsuSize << 1; j+=2) {
			vector<int> b;
			b.push_back(throwSet[i].tatsu[j] + typeArray[0] * 9);
			b.push_back(throwSet[i].tatsu[j + 1] + typeArray[0] * 9);
			tempResult.push_back(b);
		}
		for (int j = 0; j < throwSet[i].oneSize; j++){
			vector<int> b(1, throwSet[i].one[j] + typeArray[0] * 9);
			tempResult.push_back(b);
		}
		stack.push(tempResult);
		labels.push_back(0);
	}

	while (!stack.empty()) {
		vector<vector<int>> topResult(stack.top());
		int pos(labels.back() + 1);
		stack.pop();
		labels.pop_back();

		if (typeArraySize > pos) {
			for (int i = throwSetPosition[pos]; i < throwSetPosition[pos + 1]; i++) //get 3 length 3 value
			{
				vector<vector<int>> tempResult(topResult);
				for (int j = 0; j < throwSet[i].pairSize; j++) {
					vector<int> b(2, throwSet[i].pair[j] + typeArray[pos] * 9);
					tempResult.push_back(b);
				}
				for (int j = 0; j < throwSet[i].tatsuSize << 1; j+= 2) {
					vector<int> b;
					b.push_back(throwSet[i].tatsu[j] + typeArray[pos] * 9);
					b.push_back(throwSet[i].tatsu[j + 1] + typeArray[pos] * 9);
					tempResult.push_back(b);
				}
				for (int j = 0; j < throwSet[i].oneSize; j++) {
					vector<int> b(1, throwSet[i].one[j] + typeArray[pos] * 9);
					tempResult.push_back(b);
				}
				stack.push(tempResult);
				labels.push_back(pos);
			}
		}
		else if (!topResult.empty()) {
			dismantlingSearchResult.push_back(topResult);
		}
	}

	delete[] throwSet;
	return dismantlingSearchResult;
}

inline void Table::FindSequence(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, bool first, int start) {
	vector<vector<int>> tempResult = result;
	int pileNumArray[17] = { 0 };
	for (int i = 0; i < length; i++)
		pileNumArray[i] = tempNumArray[i];

	for (int i = start; i < length - 2; i++) {
		if (pileArray[i] / 9 == 3) break;
		if (i >= 0 && i < 17 && pileNumArray[i] > 0 && pileNumArray[i + 1] > 0 && pileNumArray[i + 2] > 0 &&
			pileArray[i] + 1 == pileArray[i + 1] && pileArray[i] + 2 == pileArray[i + 2] &&
			pileArray[i] / 9 == pileArray[i + 1] / 9 && pileArray[i] / 9 == pileArray[i + 2] / 9) {
			//vector<int> temp;
			pileNumArray[i]--;
			pileNumArray[i + 1]--;
			pileNumArray[i + 2]--;
			FindSequence(pileArray, pileNumArray, length, tempResult, first, i);
			pileNumArray[i]++;
			pileNumArray[i + 1]++;
			pileNumArray[i + 2]++;
		}
	}
	if (first)
		Find3(pileArray, pileNumArray, length, tempResult, false, 0);
	else
		FindPair(pileArray, pileNumArray, length, tempResult);
}

inline void Table::Find3(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, bool first, int start) {
	vector<vector<int>> tempResult = result;
	int pileNumArray[17] = { 0 };
	for (int i = 0; i < length; i++)
		pileNumArray[i] = tempNumArray[i];
	for (int i = start; i < length; i++) {
		//如果符合刻子條件
		if (i >= 0 && i < 17 && pileNumArray[i] >= 3) {
			//vector<int> temp(3, pileArray[i]);
			pileNumArray[i] -= 3;
			//tempResult.push_back(temp);
			Find3(pileArray, pileNumArray, length, tempResult, first, i + 1);
			//tempResult.pop_back();
			pileNumArray[i] += 3;
		}
	}
	if (first) {
		FindSequence(pileArray, pileNumArray, length, tempResult, false, 0);
	}
	else
		FindPair(pileArray, pileNumArray, length, tempResult);
}

inline void Table::FindPair(int* pileArray, int* pileNumArray, int length, vector<vector<int>> result) {
	for (int i = 0; i < length; i++) {
		if (pileNumArray[i] >= 2) {
			pileNumArray[i] -= 2;
			vector<int> temp(2, pileArray[i]);
			result.push_back(temp);
		}
	}
	FindHole(pileArray, pileNumArray, length, result, 0, 1);
}

inline void Table::FindHole(int* pileArray, int* tempNumArray, int length, vector<vector<int>> result, int start1, int start2) {
	int pileNumArray[17] = { 0 };
	for (int i = 0; i < length; i++)
		pileNumArray[i] = tempNumArray[i];
	for (int i = start1; i < length - 1; i++) {
		if (pileArray[i] / 9 == 3) break;
		// Find the starting point and ending point, a is the starting point, b is the ending point, count to see if both have values
		if (i >= 0 && i < 17 && pileNumArray[i] <= 0) continue;
		for (int j = start2; j < length; j++) {
			if (i >= 0 && i < 17 && j >= 0 && j < 17 && (pileNumArray[i] <= 0 || pileNumArray[j] <= 0 || pileArray[i] / 9 != pileArray[j] / 9)) continue;
			if (i >= 0 && i < 17 && j >= 0 && j < 17 && ((pileArray[i] % 9 + 1 < 9 && pileArray[i] + 1 == pileArray[j]) || (pileArray[i] % 9 + 2 < 9 && pileArray[i] + 2 == pileArray[j]))) {
				vector<int> temp;
				temp.push_back(pileArray[i]);
				temp.push_back(pileArray[j]);
				result.push_back(temp);
				pileNumArray[i]--;
				pileNumArray[j]--;
				FindHole(pileArray, pileNumArray, length, result, i, j);
				result.pop_back();
				pileNumArray[i]++;
				pileNumArray[j]++;
			}
		}
		start2 = i + 1;
	}
	FindPair(pileArray, pileNumArray, length, result);
	//FindOne(pileArray, pileNumArray, length, result);
}

inline void Table::FindOne(int* pileArray, int* pileNumArray, int length, vector<vector<int>> result) {
	for (int i = 0; i < length; i++) {	
		if (pileNumArray[i] > 0) {
			pileNumArray[i]--;
			vector<int> temp(1, pileArray[i]);
			result.push_back(temp);
			i--;
		}
	}
	vector<vector<int>> temp = result;
	std::sort(temp.begin(), temp.end());
	if (u_map.find(vectorToString(temp)) != u_map.end())
		return;
	else
		u_map[vectorToString(temp)] = true;

	//if(!result.empty())
	dismantlingResult.push_back(result);
}

void Table::GenerateDismantlingTable(FILE* fp) {
	int *tableLength = new int[MAX_ID];
	int* tableWordLength = new int[MAX_WORD_ID];
	int tempArray[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	int tempNumArray[9] = { 0 };
	int tempArrayChar[7] = { 27, 28, 29, 30, 31, 32, 33 };
	int totalIndex = 0;

	/// Initialization
	for (unsigned i = 0; i < MAX_ID; i++) {
		tableLength[i] = 0;
	}
	for (unsigned i = 0; i < MAX_WORD_ID; i++) {
		tableWordLength[i] = 0;
	}

	for (unsigned i = 1; i < MAX_ID; i++) // Generate all possible condition /// i: 所有可能的牌組
	{
		int a = i, j = 0; /// Copy一份i到a
		int coun = 0; // error judgement
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		
		while (a) { /// 轉為十進位的每個bit都表示五進位數字的一位
			tempNumArray[j++] = a % 5;
			coun += a % 5; // add all tempNumArray's numbers /// 看該花色有多少張牌
			a /= 5;
		}
		if (coun > 17) {
			tableLength[i - 1] = totalIndex;
			continue;
		}
		std::vector<std::vector<std::vector<int>>> tmp = findThrowlist(tempArray, tempNumArray, 9, false);
		for (unsigned a = 0; a < tmp.size(); a++) {
			ThrowSet throwSet;
			for (unsigned j = 0; j < tmp[a].size(); j++) {
				for (unsigned z = 0; z < tmp[a][j].size(); z++) {
					if (tmp[a][j].size() == 2 && tmp[a][j][0] == tmp[a][j][1]) {
						throwSet.pair[throwSet.pairSize] = tmp[a][j][0];
						throwSet.pairSize++;
						break;
					}
					else if (tmp[a][j].size() == 2) {
						throwSet.tatsu[throwSet.tatsuSize * 2 + z] = tmp[a][j][z];
						if(z == 1) throwSet.tatsuSize++;
					}
					else {
						throwSet.one[throwSet.oneSize] = tmp[a][j][z];
						throwSet.oneSize++;
					}					
				}
			}
			fwrite((char*)&throwSet, sizeof(ThrowSet), 1, fp);
		}		
		tableLength[i - 1] = totalIndex;
		totalIndex += (int)tmp.size();
	}
	fwrite(tableLength, sizeof(int), MAX_ID, fp);
	totalIndex = 0;

	for (unsigned i = 1; i < MAX_WORD_ID; i++) // Generate all possible condition
	{
		int a = i, j = 0;
		int coun = 0;
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5;
			a /= 5;
		}
		if (coun > 17) {
			tableWordLength[i - 1] = totalIndex;
			continue;
		}
		vector<vector<vector<int>>> tmp = findThrowlist(tempArrayChar, tempNumArray, 7, false);
		for (unsigned a = 0; a < tmp.size(); a++) {
			ThrowSet throwSet;
			for (unsigned j = 0; j < tmp[a].size(); j++) {
				for (unsigned z = 0; z < tmp[a][j].size(); z++) {
					if (tmp[a][j].size() == 2 && tmp[a][j][0] == tmp[a][j][1]) {
						throwSet.pair[throwSet.pairSize] = tmp[a][j][0];
						throwSet.pairSize++;
						break;
					}
					else if (tmp[a][j].size() == 2) {
						throwSet.tatsu[throwSet.tatsuSize * 2 + z] = tmp[a][j][z];
						if (z == 1) throwSet.tatsuSize++;
					}
					else {
						throwSet.one[throwSet.oneSize] = tmp[a][j][z];
						throwSet.oneSize++;
					}
				}
			}
			fwrite((char*)&throwSet, sizeof(ThrowSet), 1, fp);
		}
		tableWordLength[i - 1] = totalIndex;
		totalIndex += (int)tmp.size();
	}
	fwrite(tableWordLength, sizeof(int), MAX_WORD_ID, fp);
	totalIndex = 0;

	for (unsigned i = 1; i < MAX_ID; i++) // Generate all possible condition
	{
		int a = i, j = 0;
		int coun = 0; // error judgement
		memset(&tempNumArray, 0, sizeof(tempNumArray));

		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5; // add all tempNumArray's numbers
			a /= 5;
		}
		if (coun > 17) {
			tableLength[i - 1] = totalIndex;
			continue;
		}
		vector<vector<vector<int>>> tmp = findThrowlist(tempArray, tempNumArray, 9, true);
		for (unsigned a = 0; a < tmp.size(); a++) {
			ThrowSet throwSet;
			for (unsigned j = 0; j < tmp[a].size(); j++) {
				for (unsigned z = 0; z < tmp[a][j].size(); z++) {
					if (tmp[a][j].size() == 2 && tmp[a][j][0] == tmp[a][j][1]) {
						throwSet.pair[throwSet.pairSize] = tmp[a][j][0];
						throwSet.pairSize++;
						break;
					}
					else if (tmp[a][j].size() == 2) {
						throwSet.tatsu[throwSet.tatsuSize * 2 + z] = tmp[a][j][z];
						if (z == 1) throwSet.tatsuSize++;
					}
					else {
						throwSet.one[throwSet.oneSize] = tmp[a][j][z];
						throwSet.oneSize++;
					}
				}
			}
			fwrite((char*)&throwSet, sizeof(ThrowSet), 1, fp);
		}
		tableLength[i - 1] = totalIndex;
		totalIndex += (int)tmp.size();
	}
	fwrite(tableLength, sizeof(int), MAX_ID, fp);
	totalIndex = 0;

	for (unsigned i = 1; i < MAX_WORD_ID; i++) // Generate all possible condition
	{
		int a = i, j = 0;
		int coun = 0;
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5;
			a /= 5;
		}
		if (coun > 17) {
			tableWordLength[i - 1] = totalIndex;
			continue;
		}
		vector<vector<vector<int>>> tmp = findThrowlist(tempArrayChar, tempNumArray, 7, true);
		for (unsigned a = 0; a < tmp.size(); a++) {
			ThrowSet throwSet;
			for (unsigned j = 0; j < tmp[a].size(); j++) {
				for (unsigned z = 0; z < tmp[a][j].size(); z++) {
					if (tmp[a][j].size() == 2 && tmp[a][j][0] == tmp[a][j][1]) {
						throwSet.pair[throwSet.pairSize] = tmp[a][j][0];
						throwSet.pairSize++;
						break;
					}
					else if (tmp[a][j].size() == 2) {
						throwSet.tatsu[throwSet.tatsuSize * 2 + z] = tmp[a][j][z];
						if (z == 1) throwSet.tatsuSize++;
					}
					else {
						throwSet.one[throwSet.oneSize] = tmp[a][j][z];
						throwSet.oneSize++;
					}
				}
			}
			fwrite((char*)&throwSet, sizeof(ThrowSet), 1, fp);
		}
		tableWordLength[i - 1] = totalIndex;
		totalIndex += (int)tmp.size();
	}
	fwrite(tableWordLength, sizeof(int), MAX_WORD_ID, fp);

	delete[] tableLength;
	delete[] tableWordLength;
}

// ======= Dismantling Table (Listen) ===================================

int Table::initDismantlingListenTable() {
	FILE* fp;
	int exist = cfileexists(kDismantlingListenTableName);

	std::chrono::system_clock::time_point begin, end;
	std::chrono::duration<double> elapsed_time;
	if (!exist) {
		printf("Can't Find Table (%s)\n", kDismantlingListenTableName);
		printf("Generating Table (%s)\n", kDismantlingListenTableName);
		fflush(stdout);
		fp = fopen(kDismantlingListenTableName, "wb");

		begin = std::chrono::system_clock::now();
		time_t start_time_t = std::chrono::system_clock::to_time_t(begin);
		char buff[21];
		strftime(buff, 21, "%Y-%m-%d %H:%M:%S.", localtime(&start_time_t));

		char bit = '!';
		char line = '\n';
		fwrite(&bit, 1, 1, fp);
		fwrite(&buff, sizeof(buff), 1, fp);
		fwrite(&line, 1, 1, fp);

		GenerateDismantlingListenTable(fp);

		// reset the check bit to 1
		bit = '#';
		fseek(fp, SEEK_SET, 0);
		fwrite(&bit, 1, 1, fp);

		fclose(fp);
		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		printf(">> Finish Generating Table (%s) --- %lf(s)\n", kDismantlingListenTableName, elapsed_time.count());
		fflush(stdout);
	}
	printf(">> Start Reading Table(%s)\n", kDismantlingListenTableName);
	begin = std::chrono::system_clock::now();

	char bit[1];
	fp = fopen(kDismantlingListenTableName, "rb");
	fseek(fp, SEEK_SET, 0);
	fread(bit, 1, 1, fp);

	// !: unfinish #: finish
	if (bit[0] != '#') {
		// calculate file size
		fclose(fp);
		int ret = remove(kDismantlingListenTableName);

		if (ret == 0) {
			printf("*** Delete the Unfinish Table (%s). ***\n", kDismantlingListenTableName);
			fflush(stdout);
		}
		else {
			printf("*** Error: Unable to delete the file! ***\n");
			fflush(stdout);
		}
		return 0;
	}
	else {
		bool charTable = false;
		// read created date
		char date[50];
		fgets(date, 50, fp);
		printf("=== Table Create Time: %s ===\n", date);
		fflush(stdout);
		char name;
		vector<int> temp1(0);
		vector<vector<int>> temp2(0);
		dismantlingListenTableResult.reserve(MAX_ID);
		dismantlingListenTableResultWord.reserve(MAX_WORD_ID);
		while (true) {
			fread(&name, 1, 1, fp);
			if (name == '|') {
				charTable = true;
			}
			else if (name == '\n') {
				if (charTable) dismantlingListenTableResultWord.push_back(temp2);
				else dismantlingListenTableResult.push_back(temp2);
				temp2.clear();
			}
			else if (name == ',') {
				temp2.push_back(temp1);
				temp1.clear();
			}
			else {
				temp1.push_back((int)name);
			}

			if (feof(fp)) break;
		}

		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		// calculate file size
		double file_size = ((double)ftell(fp)) / 1024 / 1024;
		fclose(fp);
		//printf("=== File Size : %lfMB ===\n", file_size);
		printf(">> Finish Reading Table(%s)\n\n", kDismantlingListenTableName);
		printf("+------------------+-------------+--------------+\n|    Table Name    | Spend Time  |  Table Size  |\n+------------------+-------------+--------------+\n| %s | %lf(s) | %lf MB |\n+------------------+-------------+--------------+\n\n"
			, kDismantlingListenTableName, elapsed_time.count(), file_size);
		fflush(stdout);
	}
	return 1;
}

void Table::GenerateDismantlingListenTable(FILE* fp) {
	char type1 = ',', type2 = '\n', type3 = '|';
	int tempArray[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	int tempArrayChar[7] = { 27,28,29,30,31,32,33 };
	int tempNumArray[9];
	for (unsigned i = 1; i < MAX_ID; i++) // Generate all possible condition
	{
		int a = i, j = 0;
		memset(&tempNumArray, 0, sizeof(tempNumArray));
		int coun = 0; // error judgement
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5; // add all tempNumArray's numbers
			a /= 5;
		}
		if (coun > 17) {
			fwrite(&type2, 1, 1, fp);
			continue;
		}
		for (unsigned c = 0; c < 9; c++) {
			if (tempNumArray[c] > 0) {
				fwrite((char*)&tempArray[c], 1, 1, fp);
			}
		}
		fwrite(&type1, 1, 1, fp);
		for (int g = 1; g <= 5; g++) {
			bool is_have_eyes;
			int originListenNum = getTilesListenNum(g, tempArray, tempNumArray, 9, is_have_eyes);
			vector<int> throwArray;
			for (int b = 0; b < 9; b++) {
				if (tempNumArray[b] == 0) continue;
				tempNumArray[b]--;
				int currentListenNum = getTilesListenNum(g, tempArray, tempNumArray, 9, is_have_eyes);
				if (currentListenNum <= originListenNum) {
					throwArray.push_back(tempArray[b]);
				}
				tempNumArray[b]++;
			}
			for (unsigned c = 0; c < throwArray.size(); c++) {
				fwrite((char*)&throwArray[c], 1, 1, fp);
			}
			fwrite(&type1, 1, 1, fp);
		}
		fwrite(&type2, 1, 1, fp);
	}
	fwrite(&type3, 1, 1, fp);
	for (unsigned i = 1; i < MAX_WORD_ID; i++) // Generate all possible condition
	{
		int a = i, j = 0;

		memset(&tempNumArray, 0, sizeof(tempNumArray));
		int coun = 0;
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5;
			a /= 5;
		}
		if (coun > 17) {
			fwrite(&type2, 1, 1, fp);
			continue;
		}
		for (unsigned c = 0; c < 7; c++) {
			if (tempNumArray[c] > 0) {
				fwrite((char*)&tempArrayChar[c], 1, 1, fp);
			}
		}
		fwrite(&type1, 1, 1, fp);
		for (int g = 1; g <= 5; g++) {
			bool is_have_eyes;
			int originListenNum = getTilesListenNum(g, tempArrayChar, tempNumArray, 7, is_have_eyes);
			vector<int> throwArray;
			for (int b = 0; b < 7; b++) {
				if (tempNumArray[b] == 0) continue;
				tempNumArray[b]--;
				int currentListenNum = getTilesListenNum(g, tempArrayChar, tempNumArray, 7, is_have_eyes);
				if (currentListenNum <= originListenNum) {
					throwArray.push_back(tempArrayChar[b]);
				}
				tempNumArray[b]++;
			}
			for (unsigned c = 0; c < throwArray.size(); c++) {
				fwrite((char*)&throwArray[c], 1, 1, fp);
			}
			fwrite(&type1, 1, 1, fp);
		}
		fwrite(&type2, 1, 1, fp);
	}
}

vector<int> Table::getListenDismantling(int* pileArray, int* pileNumArray, const int& length, vector<int>& recordGroup, vector<vector<int>>& result_diff) {
	unsigned int ID = 0;
	vector<int> temp;
	for (int i = 0; i < length; i++) {
		ID += pileNumArray[i] * powFive[pileArray[i] % 9];
		if ((i + 1 < length && pileArray[i] / 9 != pileArray[i + 1] / 9) || i + 1 == length) {
			int group = recordGroup[pileArray[i] / 9];
			if (group > 0 && result_diff[pileArray[i] / 9][group - 1] != 0 && result_diff[pileArray[i] / 9][group + 5] != 89) {
				group--;
			}
			if (pileArray[i] / 9 != 3) {
				for (unsigned j = 0; j < dismantlingListenTableResult[ID - 1][group].size(); j++) {
					temp.push_back(dismantlingListenTableResult[ID - 1][group][j] + pileArray[i] / 9 * 9);
				}
			}
			else {
				for (unsigned j = 0; j < dismantlingListenTableResultWord[ID - 1][group].size(); j++) {
					temp.push_back(dismantlingListenTableResultWord[ID - 1][group][j]);
				}
			}
			ID = 0;
		}
	}
	for (unsigned j = 0; j < recordGroup.size(); j++) {
		printf("%d ", recordGroup[j]);
	}
	printf("\n");
	return temp;
}

vector<int> Table::getListenDismantling(int* tableID, vector<int>& listenRecordGroup, bool& is_have_eyes, const int& takeTile, const int& throwTile) {
	int takePos = takeTile / 9, throwPos = throwTile / 9;
	vector<int> result;
	if (takeTile == -1 || throwTile == -1) {
		for (int i = 0; i < 4; i++) {
			int group = listenRecordGroup[i] - 1 >= 0 ? listenRecordGroup[i] - 1 : 0;
			if (i != 3 && !dismantlingListenTableResult[tableID[takePos] - 1].empty() && !dismantlingListenTableResult[tableID[i] - 1][group].empty()) {
				for (unsigned j = 0; j < dismantlingListenTableResult[tableID[i] - 1][group].size(); j++) {
					result.push_back(dismantlingListenTableResult[tableID[i] - 1][group][j] + i * 9);
				}
			}
			else if (i == 3) {
				for (unsigned j = 0; j < dismantlingListenTableResultWord[tableID[i] - 1][group].size(); j++) {
					result.push_back(dismantlingListenTableResultWord[tableID[i] - 1][group][j]);
				}
			}
		}
	}
	else {
		int group = listenRecordGroup[takePos] - 1 >= 0 ? listenRecordGroup[takePos] - 1 : 0;
		if (takePos != 3 && !dismantlingListenTableResult[tableID[takePos] - 1].empty() && !dismantlingListenTableResult[tableID[takePos] - 1][group].empty()) {
			for (unsigned j = 0; j < dismantlingListenTableResult[tableID[takePos]][group].size(); j++) {
				result.push_back(dismantlingListenTableResult[tableID[takePos] - 1][group][j] + takePos * 9);
			}
		}
		else if (takePos == 3 && !dismantlingListenTableResultWord[tableID[takePos] - 1].empty() && !dismantlingListenTableResultWord[tableID[takePos] - 1][group].empty()) {
			for (unsigned j = 0; j < dismantlingListenTableResultWord[tableID[takePos] - 1][group].size(); j++) {
				result.push_back(dismantlingListenTableResultWord[tableID[takePos] - 1][group][j]);
			}
		}

		group = listenRecordGroup[throwPos] - 1 >= 0 ? listenRecordGroup[throwPos] - 1 : 0;
		if (throwPos != 3 && !dismantlingListenTableResult[tableID[throwPos] - 1].empty() && !dismantlingListenTableResult[tableID[throwPos] - 1][group].empty()) {
			for (unsigned j = 0; j < dismantlingListenTableResult[tableID[throwPos] - 1][group].size(); j++) {
				result.push_back(dismantlingListenTableResult[tableID[throwPos] - 1][group][j] + throwPos * 9);
			}
		}
		else if (throwPos == 3 && !dismantlingListenTableResultWord[tableID[throwPos] - 1].empty() && !dismantlingListenTableResultWord[tableID[throwPos] - 1][group].empty()) {
			for (unsigned j = 0; j < dismantlingListenTableResultWord[tableID[throwPos] - 1][listenRecordGroup[throwPos]].size(); j++) {
				result.push_back(dismantlingListenTableResultWord[tableID[throwPos] - 1][listenRecordGroup[throwPos]][j]);
			}
		}
	}

	for (unsigned j = 0; j < listenRecordGroup.size(); j++) {
		printf("%d ", listenRecordGroup[j]);
	}
	printf("\n");
	return result;
}

// ======= Valid Tiles Table ===================================

int Table::initValidTileTable() {
	FILE* fp;
	int exist = cfileexists(kValidTileTableName);

	std::chrono::system_clock::time_point begin, end;
	std::chrono::duration<double> elapsed_time;
	if (!exist) {
		printf("Can't Find Table (%s)\n", kValidTileTableName);
		printf("Generating Table (%s)\n", kValidTileTableName);
		fflush(stdout);
		fp = fopen(kValidTileTableName, "wb");

		begin = std::chrono::system_clock::now();
		time_t start_time_t = std::chrono::system_clock::to_time_t(begin);
		char buff[21];
		strftime(buff, 21, "%Y-%m-%d %H:%M:%S.", localtime(&start_time_t));

		char bit = '!';
		char line = '\n';
		fwrite(&bit, 1, 1, fp);
		fwrite(&buff, sizeof(buff), 1, fp);
		fwrite(&line, 1, 1, fp);

		GenerateValidTileTable(fp);

		// reset the check bit to 1
		bit = '#';
		fseek(fp, SEEK_SET, 0);
		fwrite(&bit, 1, 1, fp);

		fclose(fp);
		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		printf(">> Finish Generating Table (%s) --- %lf(s)\n", kValidTileTableName, elapsed_time.count());
		fflush(stdout);
	}
	printf(">> Start Reading Table(%s)\n", kValidTileTableName);
	begin = std::chrono::system_clock::now();

	char bit[1];
	fp = fopen(kValidTileTableName, "rb");
	fseek(fp, SEEK_SET, 0);
	fread(bit, 1, 1, fp);

	// !: unfinish #: finish
	if (bit[0] != '#') {
		// calculate file size
		fclose(fp);
		int ret = remove(kValidTileTableName);

		if (ret == 0) {
			printf("*** Delete the Unfinish Table (%s). ***\n", kValidTileTableName);
			fflush(stdout);
		}
		else {
			printf("*** Error: Unable to delete the file! ***\n");
			fflush(stdout);
		}
		return 0;
	}
	else {
		// read created date
		char date[50];
		fgets(date, 50, fp);
		printf("=== Table Create Time: %s ===\n", date);
		fflush(stdout);

		fread(validTiles, sizeof(char), VALID_TABLE_SIZE, fp);
		fread(validTiles_Size, sizeof(int), MAX_ID, fp);

		fread(validTilesChar, sizeof(char), VALID_TABLE_WORD_SIZE, fp);
		fread(validTilesChar_Size, sizeof(int), MAX_WORD_ID, fp);

		fread(validTiles_Eye, sizeof(char), VALID_TABLE_EYE_SIZE, fp);
		fread(validTiles_Eye_Size, sizeof(int), MAX_ID, fp);

		fread(validTilesChar_Eye, sizeof(char), VALID_TABLE_WORD_EYE_SIZE, fp);
		fread(validTilesChar_Eye_Size, sizeof(int), MAX_WORD_ID, fp);

		end = std::chrono::system_clock::now();
		elapsed_time = end - begin;
		// calculate file size
		double file_size = ((double)ftell(fp)) / 1024 / 1024;
		fclose(fp);
		//printf("=== File Size : %lfMB ===\n", file_size);
		printf(">> Finish Reading Table(%s)\n\n", kValidTileTableName);
		printf("+--------------------+-------------+--------------+\n|     Table Name     | Spend Time  |  Table Size  |\n+--------------------+-------------+--------------+\n| %s | %lf(s) | %lf MB |\n+--------------------+-------------+--------------+\n\n"
			, kValidTileTableName, elapsed_time.count(), file_size);
		fflush(stdout);
	}

	/*bool ishaveEye = true;
	int validArray[34] = {0}, tableID[4] = {484530, 18776, 406252, 0};
	char TileType[4] = { 'w', 't', 's', 'z' };
	int length = getValidTiles(tableID, validArray, ishaveEye);
	printf("Length: %d\n", length);
	for (int i = 0; i < length; i++) {
		if (i > 0 && validArray[i - 1] / 9 != validArray[i] / 9) {
			printf("%c, ", TileType[validArray[i - 1] / 9]);
		}
		printf("%d", validArray[i] % 9 + 1);
		if (i + 1 == length) {
			printf("%c\n", TileType[validArray[i] / 9]);
		}
	}
	maxGroup = 0;
	int tempArray[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	int a = 484530, j = 0;
	int tempNumArray[9] = { 0 };
	int coun = 0; // error judgement
	while (a) {
		tempNumArray[j++] = a % 5;
		coun += a % 5; // add all tempNumArray's numbers
		a /= 5;
	}
	FindSequence(tempArray, tempNumArray, 9, 0, true, 0);
	printf("maxGroup: %d\n", maxGroup);
	int originListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, ishaveEye);
	for (int b = 0; b < 9; b++) {
		if (tempNumArray[b] == 4) continue;
		tempNumArray[b]++;
		int currentListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, ishaveEye);
		if (currentListenNum < originListenNum) {

		}
		tempNumArray[b]--;
		printf("%d %d\n", currentListenNum, originListenNum);
	}*/

	return 1;
}

void Table::GenerateValidTileTable(FILE* fp) {
	int tempArray[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 }; /// 9種數字
	int tempCharArray[7] = { 27,28,29,30,31,32,33 }; /// 7種字牌
	int size = 0; /// Table的大小
	int* tableLength = new int[MAX_ID]; /// 5^9種牌型的有效牌再表中的index起始值
	int* tableWordLength = new int[MAX_WORD_ID];
	
	/// Initialize
	for (unsigned i = 0; i < MAX_ID; i++) {
		tableLength[i] = 0;
	}
	for (unsigned i = 0; i < MAX_WORD_ID; i++) {
		tableWordLength[i] = 0;
	}

	for (unsigned i = 1; i < MAX_ID; i++) { // Generate all possible condition
		int a = i, j = 0;
		int tempNumArray[9] = {0};
		int coun = 0; // error judgement /// 該花色的牌數
		while (a) {
			tempNumArray[j++] = a % 5; /// 將5進位每一位存在array中
			coun += a % 5; // add all tempNumArray's numbers
			a /= 5;
		}
		if (coun > 16) { /// Invalid (同花色的牌超過16張)
			tableLength[i] = size;
			continue;
		}
		bool is_have_eyes;
		maxGroup = 0;
		FindSequence(tempArray, tempNumArray, 9, 0, true, 0); /// 初始組數0
		int originListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, is_have_eyes);
		
		/// 看看每個數字的牌如果多一張是否會減少進聽數
		for (int b = 0; b < 9; b++) {
			if (tempNumArray[b] == 4) continue; /// 沒有牌可以再進了
			tempNumArray[b]++;
			int currentListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, is_have_eyes);
			if (currentListenNum < originListenNum) {				
				size++;
				fwrite((char*)&tempArray[b], sizeof(char), 1, fp);
			}
			tempNumArray[b]--;
		}
		tableLength[i] = size;
	}
	fwrite(tableLength, sizeof(int), MAX_ID, fp);
	printf("%d\n", size);
	size = 0;
	for (unsigned i = 1; i < MAX_WORD_ID; i++) { // Generate all possible condition
		int a = i, j = 0;
		int tempNumArray[7] = { 0 };
		int coun = 0;
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5;
			a /= 5;
		}
		if (coun > 16) {
			tableWordLength[i] = size;
			continue;
		}
		bool is_have_eyes;
		maxGroup = 0;
		FindSequence(tempCharArray, tempNumArray, 7, 0, true, 0);
		int originListenNum = getTilesListenNum(maxGroup, tempCharArray, tempNumArray, 7, is_have_eyes);
		for (int b = 0; b < 7; b++) {
			if (tempNumArray[b] == 4) continue;
			tempNumArray[b]++;
			int currentListenNum = getTilesListenNum(maxGroup, tempCharArray, tempNumArray, 7, is_have_eyes);
			if (currentListenNum < originListenNum) {
				size++;
				fwrite((char*)&tempArray[b], sizeof(char), 1, fp);
			}
			tempNumArray[b]--;
		}
		tableWordLength[i] = size;
	}
	fwrite(tableWordLength, sizeof(int), MAX_WORD_ID, fp);
	printf("%d\n", size);
	size = 0;
	for (unsigned i = 1; i < MAX_ID; i++) { // Generate all possible condition
		int a = i, j = 0;
		int tempNumArray[9] = { 0 };
		int coun = 0; // error judgement
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5; // add all tempNumArray's numbers
			a /= 5;
		}
		if (coun > 16) {
			tableLength[i] = size;
			continue;
		}
		bool is_have_eyes;
		maxGroup = 0;
		FindSequence(tempArray, tempNumArray, 9, 0, false, 0);
		int originListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, is_have_eyes);
		for (int b = 0; b < 9; b++) {
			if (tempNumArray[b] == 4) continue;
			tempNumArray[b]++;
			int currentListenNum = getTilesListenNum(maxGroup, tempArray, tempNumArray, 9, is_have_eyes);
			if (currentListenNum < originListenNum) {
				size++;
				fwrite((char*)&tempArray[b], sizeof(char), 1, fp);
			}
			tempNumArray[b]--;
		}
		tableLength[i] = size;
	}
	fwrite(tableLength, sizeof(int), MAX_ID, fp);
	printf("%d\n", size);
	size = 0;
	for (unsigned i = 1; i < MAX_WORD_ID; i++) { // Generate all possible condition
		int a = i, j = 0;
		int tempNumArray[7] = { 0 };
		int coun = 0;		
		while (a) {
			tempNumArray[j++] = a % 5;
			coun += a % 5;
			a /= 5;
		}
		if (coun > 16) {
			tableWordLength[i] = size;
			continue;
		}
		bool is_have_eyes;
		maxGroup = 0;
		FindSequence(tempCharArray, tempNumArray, 7, 0, false, 0);
		int originListenNum = getTilesListenNum(maxGroup, tempCharArray, tempNumArray, 7, is_have_eyes);
		for (int b = 0; b < 7; b++) {
			if (tempNumArray[b] == 4) continue;
			tempNumArray[b]++;
			int currentListenNum = getTilesListenNum(maxGroup, tempCharArray, tempNumArray, 7, is_have_eyes);
			if (currentListenNum < originListenNum) {
				size++;
				fwrite((char*)&tempArray[b], sizeof(char), 1, fp);
			}
			tempNumArray[b]--;
		}
		tableWordLength[i] = size;
	}
	fwrite(tableWordLength, sizeof(int), MAX_WORD_ID, fp);
	printf("%d\n", size);
	delete[] tableLength;
	delete[] tableWordLength;
}

/// 回傳可下牌張數有多少
int Table::getValidTiles(int* tableID, PredictRemainTiles &predict, bool& is_have_eyes) const {
	int validNumber = 0;
	/// for每個不同種的花色
	for (int i = 0; i < 4; i++) {
		if ((tableID[i]) == 0) continue; /// 手牌中沒有該花色的牌
		if (i != 3) { /// 數牌
			if (!is_have_eyes) { /// 如果目前沒有眼牌
				for (int j = validTiles_Eye_Size[(tableID[i]) - 1]; j < validTiles_Eye_Size[tableID[i]]; j++) {
					int leftTileNumber = predict.getRemainTilesNum(validTiles_Eye[j] + i * 9); /// 算該有效排張數
					if (leftTileNumber > 0) {
						validNumber += leftTileNumber; /// 把每張有效牌的數加起來
					}
				}
			}
			else {
				for (int j = validTiles_Size[tableID[i] - 1]; j < validTiles_Size[tableID[i]]; j++) {
					int leftTileNumber = predict.getRemainTilesNum(validTiles[j] + i * 9);
					if (leftTileNumber > 0) {
						validNumber += leftTileNumber;
					}
				}
			}
		}
		else {
			if (!is_have_eyes) {
				for (int j = validTilesChar_Eye_Size[tableID[i] - 1]; j < validTilesChar_Eye_Size[tableID[i]]; j++) {
					int leftTileNumber = predict.getRemainTilesNum(validTilesChar_Eye[j]);
					if (leftTileNumber > 0) {
						validNumber += leftTileNumber;
					}
				}
			}
			else {
				for (int j = validTilesChar_Size[(tableID[i]) - 1]; j < validTilesChar_Size[tableID[i]]; j++) {
					int leftTileNumber = predict.getRemainTilesNum(validTilesChar[j]);
					if (leftTileNumber > 0) {
						validNumber += leftTileNumber;
					}
				}
			}
		}
	}
	return validNumber;
}

int Table::getValidTiles(const int* tableID, int* validArray, const bool& is_have_eyes) const {
	int index = 0;
	for (int i = 0; i < 4; i++) {
		if (i != 3) {
			if (!is_have_eyes) {
				for (int j = validTiles_Eye_Size[(tableID[i]) - 1]; j < validTiles_Eye_Size[tableID[i]]; j++) {
					validArray[index++] = validTiles_Eye[j] + i * 9;
				}
			}
			else {
				for (int j = validTiles_Size[(tableID[i]) - 1]; j < validTiles_Size[tableID[i]]; j++) {
					validArray[index++] = validTiles[j] + i * 9;
				}
			}
		}
		else {
			if (!is_have_eyes) {
				for (int j = validTilesChar_Eye_Size[(tableID[i]) - 1]; j < validTilesChar_Eye_Size[tableID[i]]; j++) {
					validArray[index++] = validTilesChar_Eye[j];
				}
			}
			else{
				for (int j = validTilesChar_Size[(tableID[i]) - 1]; j < validTilesChar_Size[tableID[i]]; j++) {
					validArray[index++] = validTilesChar[j];
				}
			}
		}
	}
	return index;
}

int Table::getValidTiles(int* tableID, bool& is_have_eyes) const {
	int index = 0;
	for (int i = 0; i < 4; i++) {
		if ((tableID[i]) == 0) continue;
		if (i != 3) {
			if (!is_have_eyes) {
				index += validTiles_Eye_Size[tableID[i]] - validTiles_Eye_Size[(tableID[i]) - 1];
			}
			else {
				index += validTiles_Size[tableID[i]] - validTiles_Size[(tableID[i]) - 1];
			}
		}
		else {
			if (!is_have_eyes) {
				index += validTilesChar_Eye_Size[tableID[i]] - validTilesChar_Eye_Size[(tableID[i]) - 1];
			}
			else {
				index += validTilesChar_Size[tableID[i]] - validTilesChar_Size[(tableID[i]) - 1];
			}
		}
	}
	return index;
}

inline void Table::FindSequence(int* pileArray, int* tempNumArray, int length, int group, bool haveEye, int start) {
	int pileNumArray[17] = { 0 };
	for (int i = 0; i < length; i++)
		pileNumArray[i] = tempNumArray[i];

	for (int i = start; i < length - 2; i++) {
		if (pileArray[i] / 9 == 3) break;
		if (i >= 0 && i < 17 && pileNumArray[i] > 0 && pileNumArray[i + 1] > 0 && pileNumArray[i + 2] > 0 &&
			pileArray[i] + 1 == pileArray[i + 1] && pileArray[i] + 2 == pileArray[i + 2] &&
			pileArray[i] / 9 == pileArray[i + 1] / 9 && pileArray[i] / 9 == pileArray[i + 2] / 9) {
			pileNumArray[i]--;
			pileNumArray[i + 1]--;
			pileNumArray[i + 2]--;
			FindSequence(pileArray, pileNumArray, length, group + 1, haveEye, i); /// group 多湊一組
			pileNumArray[i]++;
			pileNumArray[i + 1]++;
			pileNumArray[i + 2]++;
		}
	}
	Find3(pileArray, pileNumArray, length, group, haveEye, 0);
}

inline void Table::Find3(int* pileArray, int* tempNumArray, int length, int group, bool haveEye, int start) {
	int pileNumArray[17] = { 0 };
	for (int i = 0; i < length; i++)
		pileNumArray[i] = tempNumArray[i];
	for (int i = start; i < length; i++) {
		//如果符合刻子條件
		if (i >= 0 && i < 17 && pileNumArray[i] >= 3) {
			pileNumArray[i] -= 3;
			Find3(pileArray, pileNumArray, length, group + 1, haveEye, i + 1);
			pileNumArray[i] += 3;
		}
	}
	if (haveEye) {
		FindPair(pileArray, pileNumArray, length, group);
	}	
	else { /// !haveEye
		bool remainder = false;
		/// 找看看是否還有任何不成搭或組的牌
		for (int i = 0; i < length; i++) {
			if (pileNumArray[i] > 0) {
				remainder = true;
				break;
			}
		}
		/// 只要remainder為true，mapGroup = max{group + 1, maxgroup}
		if (remainder && group + 1 > maxGroup) maxGroup = ((group + 1) <= 5 ? (group + 1) : 5);
		else if (!remainder && group > maxGroup) maxGroup = (group <= 5 ? group : 5);
	}
}

inline void Table::FindPair(int* pileArray, int* pileNumArray, int length, int group) {
	for (int i = 0; i < length; i++) {
		if (pileNumArray[i] >= 2) {
			pileNumArray[i] -= 2;
			group++;
			break;
		}
	}
	bool remainder = false;
	for (int i = 0; i < length; i++) {
		if (pileNumArray[i] > 0) {
			remainder = true;
			break;
		}
	}
	if (remainder && group + 1 > maxGroup) maxGroup = ((group + 1) <= 5 ? (group + 1) : 5);
	else if(!remainder && group > maxGroup) maxGroup = (group <= 5 ? group : 5);
}

// ======= Tools ===================================

void Table::UpdateTableID(int* tableID, const int& card, const int& type) { // type -> 0: take, 1: throw
	if (type == 0) { /// 取牌
		tableID[card / 9] += powFive[card % 9];
	}
	else { /// 丟牌
		tableID[card / 9] -= powFive[card % 9];
	}

	if (tableID[card / 9] < 0) throw std::invalid_argument("Invalid Update.");
}

void Table::UpdateTableID(int* pileArray, int* pileNumArray, const int& length, int* tableID) // type -> 0: take, 1: throw
{
	for (int i = 0; i < 4; i++) {
		tableID[i] = 0;
	}
	for (int i = 0; i < length; i++) {
		tableID[pileArray[i] / 9] += (pileNumArray[i] * powFive[pileArray[i] % 9]);
	}
}