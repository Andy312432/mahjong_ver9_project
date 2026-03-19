//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Add or delete tiles in the hand.
//
//=============================================================================

#include <algorithm>
#include <stdexcept>

#include "PrivateHand.h"


PrivateHand::PrivateHand() {
	//SortTile();
	quickSort(0, Tiles.size());
}

PrivateHand::PrivateHand(const vector<int>& cards) {
	Tiles.clear();
	Tiles = cards;
	//SortTile();
	quickSort(0, Tiles.size());
}

bool PrivateHand::findCard(const int& card) {
	return std::find(Tiles.begin(), Tiles.end(), card) != Tiles.end();
}

bool PrivateHand::findDarkGong() const{
	int count = 0;
	for (int i = 0; i < (int)Tiles.size() - 1; i++) {
		if ((Tiles[i] / 10) == (Tiles[i + 1] / 10)) {
			if (++count == 3) {
				return true;
			}
		}
		else {
			count = 0;
		}
	}
	return false;
}

/// 回傳自家暗牌牌種有幾種
int PrivateHand::getTileNumArray(int* tempArray, int* tempNumArray) { // parameter: new hand, hand number
	int index = 0;
	if (Tiles.size() > 0) {
		if (Tiles.size() > 17) {
			printf("***** Wrong size of private hand when get tiles! Size: %d *****\n", (int)Tiles.size());
			ShowPrivateHand();
			throw std::invalid_argument("Wrong size of private hand when get tiles!");
		}
		tempArray[index] = Tiles[0];
		tempNumArray[index]++;
	}
	for (int i = 1; i < (int)Tiles.size(); i++) {
		if (tempArray[index] != Tiles[i]) { // different values
			index++; // next position
			tempArray[index] = Tiles[i]; // put it in
		}
		tempNumArray[index]++;
	}
	return index + 1;
}

vector<int> PrivateHand::getTiles() const {
	vector<int> card;
	for (int i = 0; i < (int)Tiles.size(); i++) {
		card.push_back(Tiles[i]);
	}
	return card;
}

void PrivateHand::SetHand(const vector<int>& cards) {	
	Tiles.clear();
	Tiles.resize(cards.size());
	for (int i = 0; i < (int)cards.size(); i++) {
		Tiles[i] = cards[i];
	}
	//SortTile();
	quickSort(0, Tiles.size());
}

//void PrivateHand::SortTile() {
//	std::sort(Tiles.begin(), Tiles.end());
//
//}

void PrivateHand::AddHand(const int& card, boost::multiprecision::uint256_t& hashKey) {
	if (Tiles.size() == 17) {
		printf("***** Wrong size of private hand when adding tiles! Size: %d *****\n", (int)Tiles.size());
		ShowPrivateHand();
		throw std::invalid_argument("Wrong size of private hand when adding tiles!");
	}
	Tiles.push_back(card);
	short cardClass = card < 100 ? card  : (card / 100 - 1) * 9 + (card / 10 % 10 - 1);; // Modified
	if (tileNum[cardClass] != 0) hashKey ^= tileNumHash[(cardClass << 2) + tileNum[cardClass] - 1]; // Modified
	hashKey ^= tileNumHash[(cardClass << 2) + (tileNum[cardClass]++)]; // Modified
	//SortTile();
	quickSort(0, Tiles.size());
}

void PrivateHand::AddHand(const int& card) {
	if (Tiles.size() == 17) {
		printf("***** Wrong size of private hand when adding tiles! Size: %d *****\n", (int)Tiles.size());
		ShowPrivateHand();
		throw std::invalid_argument("Wrong size of private hand when adding tiles!");
	}
	Tiles.push_back(card);
	short cardClass = card < 100 ? card : (card / 100 - 1) * 9 + (card / 10 % 10 - 1);; // Modified
	tileNum[cardClass]++; // Modified
	//SortTile();
	quickSort(0, Tiles.size());
}

void PrivateHand::RemoveHand(const int& card, boost::multiprecision::uint256_t& hashKey) {
	vector<int>::iterator it = std::find(Tiles.begin(), Tiles.end(), card);
	if (it != Tiles.end()) {
		Tiles.erase(it);
		short cardClass = card < 100 ? card : (card / 100 - 1) * 9 + (card / 10 % 10 - 1);; // Modified
		hashKey ^= tileNumHash[(cardClass << 2) + tileNum[cardClass] - 1]; // Modified
		if (tileNum[cardClass] != 0) hashKey ^= oppTileNumHash[(cardClass << 2) + (--tileNum[cardClass]) - 1]; // Modified
	}
	else {
		printf("***** Remove Private Hand Error!, Card: %d *****\n", card);
		ShowPrivateHand();
		printf("ThrowedCard: %d\n", card);
		fflush(stdout);
		throw std::invalid_argument("Remove Private Hand Error.");
	}
}

void PrivateHand::RemoveHand(const int& card) {
	//std::cout << "remove " << card << "\n"; // Debug
	vector<int>::iterator it = std::find(Tiles.begin(), Tiles.end(), card);
	if (it != Tiles.end()) {
		Tiles.erase(it);
		//for (int i = 0; i < Tiles.size(); i++) std::cout << Tiles[i] << " "; // Debug
		//system("pause");
		short cardClass = card < 100 ? card : (card / 100 - 1) * 9 + (card / 10 % 10 - 1); // Modified
		tileNum[cardClass]--; // Modified
	}
	else {
		printf("***** Remove Private Hand Error!, Card: %d *****\n", card);
		ShowPrivateHand();
		printf("ThrowedCard: %d\n", card);
		fflush(stdout);
		throw std::invalid_argument("Remove Private Hand Error.");
	}
}

void  PrivateHand::RemoveHand(const vector<int>& card) {
	for (int i = 0; i < (int)card.size(); i++) {
		vector<int>::iterator it = std::find(Tiles.begin(), Tiles.end(), card[i]);
		if (it != Tiles.end()) {
			Tiles.erase(it);
		}
	}
}

void PrivateHand::ShowPrivateHand() const {
	for (int i = 0; i < (int)Tiles.size(); i++) {
		printf("%d ", Tiles[i]);
	}
	printf("\n");
	fflush(stdout);
}

std::string PrivateHand::PrivateHandStr() const { // Modified
	std::string ret;
	for (int i = 0; i < (int)Tiles.size(); i++) ret += std::to_string(Tiles[i]) + " ";
	return ret;
}

void PrivateHand::ShowTileNum() const { // Modified
	for (int i = 0; i < 34; i++) {
		std::cout << tileNum[i] << " ";
		if (i % 9 == 8) std::cout << "\n";
	}
	std::cout << "\n";
}

void PrivateHand::ShowPrivateHandName() const {
	for (int i = 0; i < (int)Tiles.size(); i++) {
		if((i + 1 < (int)Tiles.size() && Tiles[i] / 9 != Tiles[i + 1] / 9 ) || i + 1 == (int)Tiles.size())
			printf("%d%c, ", Tiles[i] % 9 + 1, TileType[Tiles[i] / 9]);
		else
			printf("%d", Tiles[i] % 9 + 1);
	}
	printf("\n");
	fflush(stdout);
}

void PrivateHand::quickSort(int left, int right) {
	if (left >= right) return;
	int i = left, j = right, pivot = Tiles[left];
	do {
		do i++; while (i < right && Tiles[i] < pivot);
		do j--; while (j > left && Tiles[j] > pivot);
		if (i < j) std::swap(Tiles[i], Tiles[j]);
	} while (i < j);
	std::swap(Tiles[left], Tiles[j]);
	quickSort(left, j);
	quickSort(j + 1, right);
}