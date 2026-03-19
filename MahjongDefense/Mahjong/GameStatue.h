//======= Copyright (c) Cheng-Han Yeh, All rights reserved. ===================
//
// Purpose: Game state converter.
//
//=============================================================================

#pragma once
#ifndef __GameStatue_H__
#define __GameStatue_H__

enum Action : int { TAKE, EAT, PONG, MING_GONG, DARK_GONG, BU_GONG, THROW, HU };
enum Players : int { PLAYER1, PLAYER2, PLAYER3, PLAYER4, NONE };
enum Status : int { OnePlay, TwoPlay, ThreePlay, FourPlay, OneWin, TwoWin, ThreeWin, FourWin, Draw };

#ifdef _MSC_VER
static const char* getActionName[] = { "Take", "Eat", "Pong", "Ming Gong", "Dark Gong", "Bu Gong", "Throw", "Hu" };
static const char* getPlayerName[] = { "Player1", "Player2", "Player3", "Player4", "None" };
static const char* getStatusName[] = { "OnePlay", "TwoPlay", "ThreePlay", "FourPlay", "OneWin", "TwoWin", "ThreeWin", "FourWin", "Draw" };
#endif // DEBUG

#endif