#include <string.h>
#include <stdio.h>
#include "types.h"

void initializeSinglePlayer(Player *p, short id, const char *name, StrategyType strategy){

    p->strategy = strategy;
    p->ID = id;
    snprintf(p->name, sizeof(p->name), "%s", name);

    p->cash_balance = 30000;
    p->networth = 30000;
    p->isBankrupt = false;

    p->current_position = 0;          // GO
    p->passed_go = 0;
    p->in_jail = false;
    p->turns_remaining_Injail = 0;

    p->railwayOwned = 0;
    p->UtilitiesOwned = 0;
    p->totalPropertiesOwned = 0;

    for (int i = 0; i < 40; i++)
    {
        p->ownedAssets[i] = false;
    }

    p->loan.loanAmount = 0;
    p->loan.loanOutstanding = 0;
    p->loan.startround = 0;
    p->loan.status = LOAN_NONE;
    p->loan.interestRate = 0;
    p->loan.roundsRemaining = 0;

}

void initializePlayers(Player players[4]){
    initializeSinglePlayer(&players[0], OWNER_AGGRESSIVE_INVESTOR, "Aggressive Investor", STRATEGY_AGGRESSIVE);
    initializeSinglePlayer(&players[1], OWNER_CONSERVATIVE_BANKER, "Conservative Banker", STRATEGY_CONSERVATIVE);
    initializeSinglePlayer(&players[2], OWNER_RISK_TAKER, "Risk Taker", STRATEGY_RISK_TAKER);
    initializeSinglePlayer(&players[3], OWNER_OPPORTUNISTIC_TRADER, "Opportunistic Trader", STRATEGY_OPPORTUNISTIC);
}

