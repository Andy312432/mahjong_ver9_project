#include "TT.h"

TTEntry tt[TT_SIZE];
boost::multiprecision::uint256_t tileNumHash[136];
boost::multiprecision::uint256_t oppTileNumHash[136];
boost::multiprecision::uint256_t mask = TT_SIZE - 1;
