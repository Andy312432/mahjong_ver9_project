#include<random>
#include<assert.h>

#include "AI.h"


int main()
{
	srand(time(NULL));
	/// Num: Tiles[i] % 9, Suit: Tiles[i] / 9
	std::vector<int> randomCards; // Cards of a player
	// Generate random cards
	//{
	//	std::vector<int> allCards; // All possible cards
	//	for (int i = 110; i <= 473; i++)
	//		if (((i / 10) % 10) >= 1 && ((i / 10) % 10) <= 9 && (i % 10) >= 0 && (i % 10) < 4) allCards.push_back(i);
	//	std::random_device rd;
	//	std::mt19937 generator(rd());
	//	std::uniform_int_distribution<int> distribution(0, 135); // Index range of allCards
	//	for (int i = 0; i < 17; i++) randomCards.push_back(allCards[distribution(generator)]); // Randomly choose the index of allCards
	//	assert(randomCards.size() == 17);
	//}
	randomCards.push_back(170);
	randomCards.push_back(190);
	randomCards.push_back(210);
	randomCards.push_back(220);
	randomCards.push_back(230);
	randomCards.push_back(231);
	randomCards.push_back(232);
	randomCards.push_back(240);
	randomCards.push_back(270);
	randomCards.push_back(290);
	randomCards.push_back(320);
	randomCards.push_back(330);
	randomCards.push_back(370);
	randomCards.push_back(380);
	randomCards.push_back(410);
	randomCards.push_back(440);
	randomCards.push_back(450);
	// 7w 9w 1t 2t 3t 3t 3t 4t 7t 9t 2s 3s 7s 8s 1z 4z 5z
	const int playerID = 2;
	Table table;
	Table* tablePtr = &table;
	AI ai(playerID, randomCards, tablePtr);

	ai.ShowPrivateHand();
	int command = -2, gameCount = 1;
	ai.dealCommand(command, gameCount);
	ai.ShowPrivateHand();

	return 0;
}