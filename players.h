#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

void initializeSinglePlayer(Player *p, short id, const char *name, StrategyType strategy);

void initializePlayers(Player players[4]);

#endif