#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "players.h"
#include "types.h"
#include "game.h"
#include "events.h"
#include "board.h"

Player player[NUM_PLAYERS];
Square gameboard[NUM_SQUARES];
EventCard cards[NUM_EVENTS];
int turnOrder[NUM_PLAYERS];
EventDeck deck; 



/**typedef struct {

    int currentRounds = 0;
    int maxRounds = 500;
    int GameState;
    int Winner;
    int NumberOfPlayers = 4;
    int NumberOfSolventPlayers = 4;
    int CurrentPlayer;
    int CurrentInflation = 0;
    int CurrentMarketBoom;
    int CurrentEchonomicEvent;
    int CurrentReginolEvent;
    int CurrentGovermentRegulation;

} GameState;**/

typedef struct{

    int dice1;
    int dice2;
    int sum_of_dice;

} DiceOperations;

typedef struct{

    int playerID;
    int diceTotal;

} TurnResult;

const char *getPlayerName(int PlayerID){

    switch (PlayerID)
    {
        case OWNER_AGGRESSIVE_INVESTOR:
            return "Aggressive Investor";

        case OWNER_CONSERVATIVE_BANKER:

            return "Conservative Banker";

        case OWNER_RISK_TAKER:
            return "Risk Taker";

        case OWNER_OPPORTUNISTIC_TRADER:
            return "Opportunistic Trader";

        default:
            return "Unknown Player";
    }
}

DiceOperations get_random_dice_values(){

    DiceOperations dice;

    dice.dice1 = (rand()%6) + 1;

    dice.dice2 = (rand()%6) + 1;

    dice.sum_of_dice = dice.dice1 + dice.dice2;

    return dice;


}

void sortTurnResults(TurnResult results[], int start, int end){

    for (int i = start; i < end - 1; i++)
    {
        for (int j = start; j < end - 1; j++)
        {
            if (results[j].diceTotal < results[j + 1].diceTotal)
            {
                TurnResult temporary = results[j];
                results[j] = results[j + 1];
                results[j + 1] = temporary;
            }
        }
    }
}

void rankPlayerGroup(TurnResult results[], int start, int end){

    /* Every player in this group rolls */
    for (int i = start; i < end; i++)
    {
        DiceOperations dice = get_random_dice_values();

        results[i].diceTotal = dice.sum_of_dice;

        printf(
            "%s rolled %d and %d. Total = %d\n",
            getPlayerName(results[i].playerID),
            dice.dice1,
            dice.dice2,
            dice.sum_of_dice
        );
    }

    /* Arrange this group from highest to lowest */
    sortTurnResults(results, start, end);

    /* Search for equal totals */
    int groupStart = start;

    while (groupStart < end)
    {
        int groupEnd = groupStart + 1;

        while (groupEnd < end && results[groupEnd].diceTotal == results[groupStart].diceTotal)
        {
            groupEnd++;
        }

        int tiedPlayers = groupEnd - groupStart;

        if (tiedPlayers > 1)
        {
            printf("\n%d players are tied with %d.\n", tiedPlayers, results[groupStart].diceTotal);

            printf("Only these tied players reroll.\n\n");

            rankPlayerGroup(results, groupStart, groupEnd);
        }

        groupStart = groupEnd;
    }
}

void determineTurnOrder(int turnOrder[4]){

    TurnResult results[4];

    /* Connect each position to one player */
    for (int i = 0; i < 4; i++)
    {
        results[i].playerID = i;
        results[i].diceTotal = 0;
    }

    printf("Determining Player Turn Order\n");
    printf("=============================\n");

    /*
     * All four players roll first.
     * Ties are handled inside this function.
     */
    rankPlayerGroup(results, 0, 4);

    /* Save the final player IDs */
    for (int i = 0; i < 4; i++)
    {
        turnOrder[i] = results[i].playerID;
    }

    printf("\nFinal Turn Order\n");
    printf("================\n");

    for (int i = 0; i < 4; i++)
    {
        printf(
            "%d. %s\n",
            i + 1,
            getPlayerName(turnOrder[i])
        );
    }
}

void movePlayer(Player *p, int dice){

    p->current_position += dice;

    if(p->current_position >= 40)
    {
        p->current_position -= 40;
        p->cash_balance += 2000;
    }
}

void player_move(struct Player *player){

    DiceOperations dice_value = get_random_dice_values();

    int is_double = (dice_value.dice1 == dice_value.dice2);

    if(player->in_jail)
    {
        if(player->turns_remaining_Injail==0)
        {
            player->in_jail = false;
            printf("%s left Jail after 3 turns.\n\n", getPlayerName(player->ID));
        }

        else if(is_double)
        {
            player->in_jail = false;
            player->turns_remaining_Injail = 0;
            printf("%s rolled doubles and left Jail.\n\n", getPlayerName(player->ID));
        }
        else if (player->cash_balance >= 300) {
            player->in_jail = false;
            player->turns_remaining_Injail = 0;
            printf("%s paid bail of LKR 300.\n\n", getPlayerName(player->ID));
        }
        else {
            player->turns_remaining_Injail--;
            printf("%s remains in jail and must skip the turn.\n", getPlayerName(player->ID));
        }
    }

    int current_position = player->current_position;
    int new_position = (player->current_position + dice) % 40;
    printf("%s rolled %d.\n", getPlayerName(player->ID), dice);
    printf("%s moved from Square %d to square %d.\n", getPlayerName(player->ID), player->current_position, new_position);

    if (current_position + dice >= 40) {
        player->passed_go++;
        player->cash_balance += 2000;
        printf("%s passed GO\n", getPlayerName(player->ID));
        printf("Collected LKR 2000.\n");
        printf("Current Balance : LKR %d.\n", player->cash_balance);
    }
    player->current_position = new_position;

}

void startgame(){

    initializePlayers(player);

    initialize_board(gameboard);

    determineTurnOrder(turnOrder);

    initializeEventCards(cards);

    shuffleEventDeck(&deck);


}