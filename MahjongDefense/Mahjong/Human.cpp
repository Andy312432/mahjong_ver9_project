//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Create Human mode to player mahjong game.
//
//=============================================================================

#include "Human.h"

Human::Human(const int& ID, const PrivateHand& cards) : Player(ID, cards) {
	printf("Human Mode\n");
}

Human::Human(const int& ID, const vector<int>& cards) : Player(ID, cards) {
	printf("Human Mode\n");
	mode = 1;
}

vector<int> Human::dealEat() {
	char ans;
	int a, b;
	vector<int> result;
	printf("Ask Eat, Tiles: %d\n", wallTiles.getTileSea());
	scanf("%c\n", &ans);
	if (ans == 'y' || ans == 'Y') {
		scanf("%d %d\n", &a, &b);

		result.push_back(a);
		result.push_back(b);
	}
	return result;
}

vector<int> Human::dealPong() {
	char ans;
	int a, b;
	vector<int> result;
	printf("Ask Pong, Tiles: %d\n", wallTiles.getTileSea());
	scanf("%c\n", &ans);
	if (ans == 'y' || ans == 'Y') {
		scanf("%d %d\n", &a, &b);

		result.push_back(a);
		result.push_back(b);
	}
	return result;
}

vector<int> Human::dealMingGong() {
	//char ans;
	//int a, b;
	vector<int> result;
	/*printf("Ask Gong, Tiles: %d\n", wallTiles.getTileSea());
	scanf("%c\n", &ans);
	if (ans == 'y' || ans == 'Y')
	{
		scanf("%d %d\n", &a, &b);

		result.push_back(a);
		result.push_back(b);
	}*/
	return result;
}

vector<int> Human::dealDarkGong() {
	vector<int> result;
	return result;
}

vector<int> Human::dealBuGong(const int& takeTile) {
	vector<int> result;
	return result;
}

vector<int> Human::dealHu() {
	vector<int> result(1, 1);
	return result;
}

vector<int> Human::dealThrow() {
	vector<int> result;
	printf("Ask Throw:\n");
	int a;
	scanf("%d\n", &a);
	result.push_back(a);

	return result;
}

void Human::DealTake(const int& takeTile) {

}

void Human::DealRemainTiles(const int& card) {

}

void Human::MoveThrow(const vector<int>& throwTile) {

}

void Human::ShowFunctionTime() {

}