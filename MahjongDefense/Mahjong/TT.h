#pragma once

#ifndef MAHJONG_TT_H_
#define MAHJONG_TT_H_

#include <boost/multiprecision/cpp_int.hpp>

#define TT_SIZE 33554432 // 2^25 // Modified

typedef struct TT_ENTRY { // Modified
	boost::multiprecision::uint256_t key;
	unsigned int simNum = 0;
	unsigned int simScore = 0;
} TTEntry;

extern TTEntry tt[TT_SIZE];
extern boost::multiprecision::uint256_t tileNumHash[136];
extern boost::multiprecision::uint256_t oppTileNumHash[136];
extern boost::multiprecision::uint256_t mask; // For convert hash key to hash index // Modified

#endif
