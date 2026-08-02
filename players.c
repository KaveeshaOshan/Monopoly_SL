#include <string.h>
#include "types.h"

extern Player players[4];

void initializeSinglePlayer(Player *p, short id, const char *name, StrategyType strategy)
{

    p->strategy = strategy;
    p->ID = id;

    p->cash_balance = 30000;
    p->networth = 30000;
    p->isBankrupt = false;

    p->current_position = 0;          // GO
    p->in_jail = false;
    p->turns_remaining_Injail = 0;

    p->railwayOwned = 0;
    p->UtilitiesOwned = 0;
    p->totalPropertiesOwned = 0;

    for (int i = 0; i < 40; i++)
    {
        p->ownedProperties[i] = false;
    }

    p->loan.loanAmount = 0;
    p->loan.loanOutstanding = 0;
    p->loan.startround = 0;
    p->loan.status = LOAN_NONE;
    p->loan.principal = 0;
    p->loan.interestRate = 0;
    p->loan.roundsRemaining = 0;

}

void initializePlayers()
{
    initializeSinglePlayer(&players[0], 0, "Aggressive Investor", STRATEGY_AGGRESSIVE);
    initializeSinglePlayer(&players[1], 1, "Conservative Banker", STRATEGY_CONSERVATIVE);
    initializeSinglePlayer(&players[2], 2, "Risk Taker", STRATEGY_RISK_TAKER);
    initializeSinglePlayer(&players[3], 3, "Opportunistic Trader", STRATEGY_OPPORTUNISTIC);
}