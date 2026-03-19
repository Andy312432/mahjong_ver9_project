#ifndef TT
#include <boost/multiprecision/cpp_int.hpp>

#define TT
#define TT_SIZE 33554432 // 2^25 // Modified
// Entry of TT
typedef struct TT_ENTRY { // Modified
	boost::multiprecision::uint256_t key;
	unsigned int simNum = 0;
	unsigned int simScore = 0;
	//std::string boardLog;
} TTEntry;

static TTEntry tt[TT_SIZE]; // Modified
//TTEntry* tt = (TTEntry*)calloc(1073741824, sizeof(TTEntry)); // Modified
static boost::multiprecision::uint256_t tileNumHash[136];
static boost::multiprecision::uint256_t oppTileNumHash[136];
static boost::multiprecision::uint256_t mask = TT_SIZE - 1; // For convert hash key to hash index // Modified
#endif
