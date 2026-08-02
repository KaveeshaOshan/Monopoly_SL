#include <string.h>
#include "types.h"

extern player players[4];

void initializeSinglePlayer(player *p, short id, const char *name, StrategyType strategy)
{

    strcpy(p->name, name);
    p->strategy = strategy;
    p->ID = id;

    p->cash_balance = 30000;
    p->networth = 30000;
    p->isBankrupt = 0;

    p->current_position = 0;          // GO
    p->in_jail = 0;
    p->turns_remaining_Injail = 0;

    p->railwayOwned = 0;
    p->UtilitiesOwned = 0;
    p->totalPropertiesOwned = 0;

    for (int i = 0; i < 40; i++)
    {
        p->ownedProperties[i] = -1;
    }

    p->loan.active = 0;
    p->loan.principal = 0;
    p->loan.interestRate = 0;
    p->loan.interestRate = 0;
    p->loan.roundsRemaining = 0;
}

void initializePlayers()
{
    initializeSinglePlayer(&players[0], 1, "Player 1", STRATEGY_AGGRESSIVE);
    initializeSinglePlayer(&players[1], 2, "Player 2", STRATEGY_CONSERVATIVE);
    initializeSinglePlayer(&players[2], 3, "Player 3", STRATEGY_RISK_TAKER);
    initializeSinglePlayer(&players[3], 4, "Player 4", STRATEGY_OPPORTUNISTIC);
}