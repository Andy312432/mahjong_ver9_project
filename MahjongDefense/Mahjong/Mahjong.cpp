//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Old data save.
//
//=============================================================================

#include "mahjong.h"

int number = 1;

//快速排序法基準值(字串)
int partition_C(char number[], int left, int right) {
	int i = left - 1;
	int j;
	for (j = left; j < right; j++) {
		if (number[j] <= number[right]) {
			i++;
			std::swap(number[i], number[j]); /// 把所有小於pivot的值都放到左邊
		}
	}

	/// 最後把pivot插在對的位置
	std::swap(number[i + 1], number[right]);
	return i + 1;
}

//隨機搜尋(有bug)
int Mahjong::randomized_search(int* array, int n, int num) {
	for (int i = 0; i < n - 1; i++) {
		// 從其餘位置當中隨便挑一個位置
		int j = i + rand() % (n - i);
		if (array[j] != num) return j;  //bug
		// 已經找過的數字，挪至前面，永不再找。
		std::swap(array[i], array[j]);
	}
	return -1;
}

//快速排序法基準值
int Mahjong::partition(int number[], int left, int right) {
	int i = left - 1;
	int j;
	for (j = left; j < right; j++) {
		if (number[j] <= number[right]) {
			i++;
			std::swap(number[i], number[j]);
		}
	}

	std::swap(number[i + 1], number[right]);
	return i + 1;
}

//快速排序法
void Mahjong::QuickSort(int number[], int left, int right) {
	if (left < right) {
		int q = partition(number, left, right);
		/// Divide and conquar的部分
		QuickSort(number, left, q - 1);
		QuickSort(number, q + 1, right);
	}
}

//快速排序法(字串)
void QuickSort_C(char number[], int left, int right) {
	if (left < right) {
		int q = partition_C(number, left, right);
		QuickSort_C(number, left, q - 1);
		QuickSort_C(number, q + 1, right);
	}
}

void QuickSort_int(std::vector<int>& sortList, int left, int right) {
	if (left >= right) return;
	int i = left, j = right, pivot = sortList[left];
	do {
		do i++; while (i < right && sortList[i] < pivot);
		do j--; while (j > left && sortList[j] > pivot);
		if (i < j) std::swap(sortList[i], sortList[j]);
	} while (i < j);
	std::swap(sortList[left], sortList[j]);
	QuickSort_int(sortList, left, j);
	QuickSort_int(sortList, j + 1, right);
}

//隨機手牌
void Mahjong::RandomTile() {
	for (int i = 0; i < 17; ) {
		int n = randomized_search(tile, 34, 4);
		if (n != -1) {
			tile[n]++;
			char string[20];
			data[i] = n;
			i++;
		}
	}
	QuickSort(data, 0, 16);
	for (int i = 0; i < 17; i++)
		printf("%d%c ", data[i] % 9 + 1, tileType[data[i] / 9]);
	printf("\n");
}

void Mahjong::Test() {
	//data
	for (int i = 0; i < DATANUM; i++) {
		if (i == 0)
			printf("%d", data[i] % 9 + 1);
		else if (tileType[data[i - 1] / 9] != tileType[data[i] / 9] && i == DATANUM - 1)
			printf("%c, %d%c", tileType[data[i - 1] / 9], data[i] % 9 + 1, tileType[data[i] / 9]);
		else if (tileType[data[i - 1] / 9] != tileType[data[i] / 9])
			printf("%c, %d", tileType[data[i - 1] / 9], data[i] % 9 + 1);
		else if (i == DATANUM - 1)
			printf("%d%c", data[i] % 9 + 1, tileType[data[i] / 9]);
		else
			printf("%d", data[i] % 9 + 1);
	}

}

void del(char sum[], int del, int len) {
	int i = 0;
	for (; i < strlen(sum) - len - del; i++) {
		sum[del + i] = sum[len + del + i];
	}
	sum[del + (strlen(sum) - len - del)] = '\0';

}

//找順子
void Mahjong::FindSequence(int pileArray[], int pileNumArray[], int length, bool first, char string[], int start) {
	int count = 0;
	for (int i = start; i < length - 2; i++) {
		//判斷順子條件
		/// 先看三個連續的牌種是否都有牌
		/// 再看三個牌種數字是否連續
		/// 最後看三個牌種是否同花色
		if (pileNumArray[i] > 0 && pileNumArray[i + 1] > 0 && pileNumArray[i + 2] > 0 &&
			pileArray[i] + 1 == pileArray[i + 1] && pileArray[i] + 2 == pileArray[i + 2] &&
			pileArray[i] / 9 == pileArray[i + 1] / 9 && pileArray[i] / 9 == pileArray[i + 2] / 9 && pileArray[i] / 9 != 3) {
			count++;//紀錄是否有滿足順子條件，有就+1
			for (int j = 0; j < 3; j++) {
				pileNumArray[i + j]--;
				char buf[1024] = { '\0' };
				if (j == 2)
					sprintf_s(buf, "%d%c", pileArray[i + j] % 9 + 1, tileType[pileArray[i + j] / 9]);
				else
					sprintf_s(buf, "%d", pileArray[i + j] % 9 + 1);
				strcat_s(string, 100, buf);	//使用String紀錄被選中的資料
				//printf("%d%c ", pileArray[i+j] % 9 + 1, tileType[pileArray[i + j] / 9]);
			}
			strcat_s(string, 100, ",");
			FindSequence(pileArray, pileNumArray, length, false, string, i);//找下一個順子

			//回朔
			for (int a = 0; a < 3; a++)
				pileNumArray[i + a]++;
			//printf(",");
			del(string, strlen(string) - 5, 5); //String回朔，減掉上一個紀錄的值
		}
		if (count == 0 && i + 3 >= length) { /// 目前沒有順子而且i太大，都檢查玩了，不可能有順子了
			if (number <= 2)//只取前兩個(不好的作法)
			{
				number++;
				//printf("\n%2d. %s\n", number++, string);
				strncat_s(result, string, strlen(string) + 1);  //把result代替string，避免之後給值改變出錯
				int copy_pileNumArray[17] = { 0 };
				for (int p = 0; p < length; p++)
					copy_pileNumArray[p] = pileNumArray[p];  //把copy_pileNumArray代替pileNumArray，避免之後給值改變出錯
				if (first)
					Find3(pileArray, copy_pileNumArray, length, false, result);
				else
					FindPair(pileArray, copy_pileNumArray, length, result);
				memset(result, 0, sizeof(result)); //result初始化
			}

			return;
		}
	}
	//printf("\nString:%s\n", string);
}

//找刻子
void Mahjong::Find3(int pileArray[], int pileNumArray[], int length, bool first, char string[]) {
	for (int i = 0; i < length; i++) {//如果符合刻子條件
		if (pileNumArray[i] >= 3) {
			pileNumArray[i] -= 3;
			char buf[1024] = { '\0' };
			sprintf_s(buf, "%d%d%d%c,", pileArray[i] % 9 + 1, pileArray[i] % 9 + 1, pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
			strcat_s(string, 100, buf);
			//printf("%d%c ", pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
			//printf(",");
		}
	}
	/// 避免重複呼叫時又重檢查一次順子與對子
	if (first) {
		//number = 1;
		FindSequence(pileArray, pileNumArray, length, false, string, 0);
	}
	else
		FindPair(pileArray, pileNumArray, length, string);
}

//找單張
void Mahjong::FindOne(int pileArray[], int pileNumArray[], int length, char string[]) {
	for (int i = 0; i < length; i++) {
		if (pileNumArray[i] > 0) {
			char buf[1024] = { '\0' };
			sprintf_s(buf, "%d%c,", pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
			strcat_s(string, 100, buf);
		}
		//printf("%d%c ,", pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
	}
	printf("\nString:%s\n", string);
	ReGroup(string);
}

//對子
void Mahjong::FindPair(int pileArray[], int pileNumArray[], int length, char string[]) {
	for (int i = 0; i < length; i++) {
		if (pileNumArray[i] >= 2) {
			pileNumArray[i] -= 2;
			//printf("%d%c ", pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
			char buf[1024] = { '\0' };
			sprintf_s(buf, "%d%d%c,", pileArray[i] % 9 + 1, pileArray[i] % 9 + 1, tileType[pileArray[i] / 9]);
			strcat_s(string, 100, buf);
			//printf(",");
		}
	}
	FindHole(pileArray, pileNumArray, length, string);
}

//洞
void Mahjong::FindHole(int pileArray[], int pileNumArray[], int length, char string[]) {
	int count = 0, a = 0, b = 0;
	bool find = false;
	for (int i = 1; i < length; i++) {
		if (pileArray[i] / 9 == 3) break; /// 字牌沒有洞要找
		//找起點和終點，a為起點、b為終點，count看有沒有兩個都有值
		if (pileNumArray[i] > 0 && count == 0) {
			find = false;
			a = b;
			b = i; /// 把b設為有牌的index
			if (pileArray[a] / 9 != pileArray[b] / 9) /// 兩牌花色不一樣
				continue;
			else /// 如果兩牌花色一樣，就count++
				count++;
		}
		if (count >= 1 && pileNumArray[a] > 0 && pileNumArray[b] > 0) {
			//連續數
			/// 檢查a牌b牌是否為連續數
			if (pileArray[a] + 1 == pileArray[b]) {
				//小順or邊張
				find = true;
				pileNumArray[a]--;
				pileNumArray[b]--;
				char buf[1024] = { '\0' };
				sprintf_s(buf, "%d%d%c,", pileArray[a] % 9 + 1, pileArray[b] % 9 + 1, tileType[pileArray[b] / 9]);
				strcat_s(string, 100, buf);
				//printf("%d%c ", pileArray[a] % 9 + 1, tileType[pileArray[a] / 9]);
				//printf("%d%c ,", pileArray[b] % 9 + 1, tileType[pileArray[b] / 9]);
			}
			//中洞
			else if (pileArray[a] + 2 == pileArray[b]) {
				find = true;
				pileNumArray[a]--;
				pileNumArray[b]--;
				char buf[1024] = { '\0' };
				sprintf_s(buf, "%d%d%c,", pileArray[a] % 9 + 1, pileArray[b] % 9 + 1, tileType[pileArray[b] / 9]);
				strcat_s(string, 100, buf);
				//printf("%d%c ", pileArray[a] % 9 + 1, tileType[pileArray[a] / 9]);
				//printf("%d%c ,", pileArray[b] % 9 + 1, tileType[pileArray[b] / 9]);
			}
			/// 只要a牌或b牌數量為0，count就會變回0
			if (pileNumArray[a] <= 0 || pileNumArray[b] <= 0 || !find) count = 0;
			else {
				//printf("%d%c ", pileArray[a] % 9 + 1, tileType[pileArray[a] / 9]);
				//printf("%d%c ,", pileArray[b] % 9 + 1, tileType[pileArray[b] / 9]);
				i--; /// 重複檢查，避免手牌可以組合成不只一組同種搭
				/// 如果這次抓到的搭只有1組，下次就會進到上一個if，因為a牌或b牌有一個數量會是0
			}
		}
		if (pileNumArray[a] <= 0 || pileNumArray[b] <= 0 || !find) count = 0;
	}
	FindOne(pileArray, pileNumArray, length, string);
}

//判斷聽甚麼洞
void Mahjong::FindListenHole(char string[]) {
	char str[100];
	strcpy(str, string);
	char* s = strtok(str, ","); //分割的判斷字元
	char* put[100]; //分割後放入新的字串陣列
	int s_count = 0; //分幾個了

	while (s != NULL) {
		put[s_count++] = s;  //把分出來的丟進去 結果陣列
		s = strtok(NULL, ",");
	}
	for (int x = 0; x < s_count; x++) //找聽哪個洞
	{
		if (strlen(put[x]) - 1 > 3) {
			int tempArray[9] = { 0,1,2,3,4,5,6,7,8 };
			int tempNumArray[10] = { 0 }, numArray[10] = { 0 };
			int min = 99, max = -99;
			for (int i = 0; i < strlen(put[x]) - 1; i++) {
				if (put[x][i] - '1' > max) max = put[x][i] - '1';
				if (put[x][i] - '1' < min) min = put[x][i] - '1';
				tempNumArray[put[x][i] - '1']++;
			}
			if (min != 0) min--;
			if (max != 8) max++;

			for (int i = min; i <= max; i++) {
				memcpy(numArray, tempNumArray, sizeof(tempNumArray));
				numArray[i]++;

				//方法1，刻->順->眼
				for (int j = min; j <= max; j++) {
					if (numArray[j] >= 3) numArray[j] -= 3;
				}
				for (int j = min; j <= max - 2;) {
					if (numArray[j] > 0 && numArray[j + 1] > 0 && numArray[j + 2] > 0) {
						numArray[j] --;
						numArray[j + 1] --;
						numArray[j + 2] --;
					}
					else j++;
				}
				int remain = 0;
				for (int j = min; j <= max; j++) {
					if (numArray[j] > 2)
						numArray[j] -= 2;
					else if (numArray[j] <= 0)
						remain++;
				}
				//printf("%d %d %d\n", i, remain, (max - min));
				if (remain - 1 == (max - min) - 1) {
					printf("%d ", i + 1);
					continue;
				}

				memcpy(numArray, tempNumArray, sizeof(tempNumArray));
				numArray[i]++;
				//方法2，順->刻->眼
				for (int j = min; j <= max - 2;)
					if (numArray[j] > 0 && numArray[j + 1] > 0 && numArray[j + 2] > 0) {
						numArray[j] --;
						numArray[j + 1] --;
						numArray[j + 2] --;
					}
					else j++;
				for (int j = min; j <= max; j++) {
					if (numArray[j] >= 3) numArray[j] -= 3;
				}
				remain = 0;
				for (int j = min; j <= max; j++) {
					if (numArray[j] > 2)
						numArray[j] -= 2;
					else if (numArray[j] <= 0)
						remain++;
				}
				//printf("%d %d %d\n", i, remain, (max - min));
				if (remain - 1 == (max - min) - 1) {
					printf("%d ", i + 1);
					continue;
				}

				//方法3，刻->順(眼先跳過)->眼
				memcpy(numArray, tempNumArray, sizeof(tempNumArray));
				numArray[i]++;
				for (int j = min; j <= max; j++) {
					if (numArray[j] >= 3) numArray[j] -= 3;
				}
				for (int j = min; j <= max - 2;) {
					if (numArray[j] > 0 && numArray[j + 1] > 0 && numArray[j + 2] > 0 && numArray[j] != 2 && numArray[j + 1] != 2 && numArray[j + 2] != 2) {
						numArray[j] --;
						numArray[j + 1] --;
						numArray[j + 2] --;
					}
					else j++;
				}
				remain = 0;
				for (int j = min; j <= max; j++) {
					if (numArray[j] > 2)
						numArray[j] -= 2;
					else if (numArray[j] <= 0)
						remain++;
				}
				if (remain - 1 == (max - min) - 1)
					printf("%d ", i + 1);
			}
			printf("\n");
		}
	}
}

//把字串拆解重新組合(聽洞)，並呼叫FindListenHole函式(聽哪個洞)
void Mahjong::ReGroup(char string[]) {
	char* pTmp = NULL; //確保安全性(NULL)
	char* s = strtok_s(string, ",", &pTmp); //分割字串
	char* new_string[100]; //存放分割後的字串
	int s_count = 0; //計算s分割幾個字串

	while (s != NULL) {
		new_string[s_count++] = s; //分割後存放陣列
		s = strtok_s(NULL, ",", &pTmp);
	}
	//for (int x = 0; x < s_count; x++) //印出是否分割正確
	//	printf("%d %s\n", x, new_string[x]);
	char resultString[100] = { '\0' };

	int index = 0;
	for (int type = 0; type < 4; type++) {
		int index1 = 0, index2 = 0, index_r = 0;
		int perfect1 = 0, perfect2 = 0;
		int pair = 0, one = 0;
		char max = '0', min = '10';
		char tempString1[100] = { '\0' };
		char tempString2[100] = { '\0' };
		char remaindingTile[100] = { '\0' };
		for (int i = 0; i < s_count; i++) {
			if (new_string[i][strlen(new_string[i]) - 1] == tileType[type]) //判斷是否在同一門
			{
				if (type != 4) {
					//方法1，完美2組以上+單x1(單不能包含在完美裡頭)
					if (strlen(new_string[i]) - 1 == 3 && max + 2 >= new_string[i][0] && min - 2 <= new_string[i][0]) //判斷是否為順或刻
					{
						for (int a = 0; a < strlen(new_string[i]) - 1; a++) {
							if (new_string[i][a] > max) max = new_string[i][a];
							if (new_string[i][a] < min) min = new_string[i][a];
							tempString1[index1++] = new_string[i][a];
						}
						perfect1++;
						//printf("FF:%s\n", tempString1);
					}
					else if (strlen(new_string[i]) - 1 == 1 && one == 0 && max + 1 >= new_string[i][0] && min - 1 <= new_string[i][0]) {
						int count = 0;
						for (int a = 0; a < index1; a++)//判斷單張不再完美中
							if (tempString1[a] != new_string[i][0]) count++;
						//printf("::%d,%d\n", index1, count);
						if (count == strlen(tempString1)) {
							tempString1[index1++] = new_string[i][0];
							one++;
						}
					}
					//printf("%d,%d\n", perfect1, one);

					//方法二，完美x1(刻)+對子2組以上
					if (strlen(new_string[i]) - 1 == 2 && new_string[i][0] == new_string[i][1]
						&& tempString2[0] == tempString2[1] && tempString2[1] == tempString2[2]) {
						int count = 0;
						for (int b = 0; b < index2; b++) //判斷對子不再完美中
							if (tempString2[b] != new_string[i][0]) count++;
						if (count == strlen(tempString2)) {
							tempString2[index2++] = new_string[i][0];
							pair++;
						}
					}
				}
				for (int a = 0; a < strlen(new_string[i]); a++)
					remaindingTile[index_r++] = new_string[i][a];
				remaindingTile[index_r++] = ',';
			}
		}
		if (perfect1 >= 2 && one) //符合方法1
		{
			QuickSort_C(tempString1, 0, index1 - 1); //字串重新排序
			tempString1[index1++] = tileType[type];
			tempString1[index1++] = ',';
			for (int i = 0; i < index1; i++)
				resultString[index++] = tempString1[i];
		}
		else if (perfect1 == 1 && pair >= 2) //符合方法2
		{
			QuickSort_C(tempString2, 0, index2 - 1);//字串重新排序
			tempString2[index2++] = tileType[type];
			tempString2[index2++] = ',';
			for (int i = 0; i < index2; i++)
				resultString[index++] = tempString2[i];
		}
		else {
			strcat(resultString, remaindingTile);
		}
	}
	if (index != 0) {
		FindListenHole(resultString);
		printf("\nReGroup: %s\n", resultString);
	}
	else
		printf("\nNo cards to listen (ReGroup)\n");
}

//手牌分析
void Mahjong::HandTileAnalysis() {
	int index = 0;
	memcpy(&_pileArray, &data, DATANUM);
	for (int i = 0; i < DATANUM; i++) {
		if (_pileArray[index] != data[i]) { // 與我不同的值
			index++; // 下一個位置
			_pileArray[index] = data[i]; // 放進來
		}
		_pileNumArray[index]++;
	}
	//for (int i = 0; i < index+1; i++)
	//	printf("%d%c : %d\n", pileArray[i] % 9 + 1, tileType[pileArray[i] / 9], pileNumArray[i]);
	char string[100] = { '\0' };
	//FindOne(pileArray, pileNumArray, index + 1);
	//FindSequence(_pileArray, _pileNumArray, index + 1, true, string, 0);//先找順
	//FindPair(pileArray, pileNumArray, index + 1);
	/*
	Game g;
	if (g.isHu(_pileArray, _pileNumArray, index + 1))
	{
		printf("Hu Pai\n");
	}
	else
	{
		printf("Not Hu Pai\n");
		Find3(_pileArray, _pileNumArray, index + 1, true, string);//先找刻
		Table b;
		//b.Classification(_pileArray, _pileNumArray, index + 1);
		b.InitTable();
		b.SearchListenTable(data, DATANUM);
	}*/
}

//int main() {
//	Mahjong a;
//	srand(time(NULL));
//	//RandomTile();
//	a.Test();
//	printf("\n");
//	a.HandTileAnalysis();
//	printf("\n");
//}