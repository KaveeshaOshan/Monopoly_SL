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
        printf("%d. %s\n", i + 1, getPlayerName(turnOrder[i]));
        //printf(" %d\n ",turnOrder[i]);
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

void player_move(Player *theplayer){

    DiceOperations dice_value = get_random_dice_values();

    int is_double = (dice_value.dice1 == dice_value.dice2);

    if(theplayer->in_jail)
    {
        if(theplayer->turns_remaining_Injail==0)
        {
            theplayer->in_jail = false;
            printf("%s left Jail after 3 turns.\n\n", getPlayerName(theplayer->ID));
        }

        else if(is_double)
        {
            theplayer->in_jail = false;
            theplayer->turns_remaining_Injail = 0;
            printf("%s rolled doubles and left Jail.\n\n", getPlayerName(theplayer->ID));
        }
        else if (theplayer->cash_balance >= 300) {

            theplayer->cash_balance -= 300;
            theplayer->in_jail = false;
            theplayer->turns_remaining_Injail = 0;
            printf("%s paid bail of LKR 300.\n\n", getPlayerName(theplayer->ID));
        }
        else {
            theplayer->turns_remaining_Injail--;
            printf("%s remains in jail and must skip the turn.\n", getPlayerName(theplayer->ID));

            return;
        }
    }

    int current_position = theplayer->current_position;
    int new_position = (theplayer->current_position + dice_value.sum_of_dice) % 40;
    printf("%s rolled %d.\n", getPlayerName(theplayer->ID), dice_value.sum_of_dice);
    printf("%s moved from Square %d to square %d.\n", getPlayerName(theplayer->ID), theplayer->current_position, new_position);

    if (current_position + dice_value.sum_of_dice >= 40) {
        theplayer->passed_go++;
        theplayer->cash_balance += 2000;
        printf("%s passed GO\n", getPlayerName(theplayer->ID));
        printf("Collected LKR 2000.\n");
        printf("Current Balance : LKR %d.\n", theplayer->cash_balance);
    }
    theplayer->current_position = new_position;

}

bool allPlayersPassedGo(const Player players[NUM_PLAYERS], const int passesAtRoundStart[NUM_PLAYERS]){
    
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        /*
         * If the value has not increased, this player
         * has not passed GO during the current Turn.
         */
        if (players[i].passed_go <= passesAtRoundStart[i])
        {
            return false;
        }
    }

    return true;
}

void startgame(){

    GameState gameState = {0};

    initializePlayers(player);

    initialize_board(gameboard);

    determineTurnOrder(turnOrder);

    initializeEventCards(cards);

    initializeEventDeck(&deck);

    shuffleEventDeck(&deck);

    while (gameState.current_round <= gameState.max_rounds)
    {
        gameState.current_round = 1;
        int passesAtRoundStart[NUM_PLAYERS];

        /*
         * Remember how many times each player had passed GO
         * at the beginning of this round.
         */
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            passesAtRoundStart[i] = player[i].passed_go;
        }

        gameState.current_turn = 1;

        printf("\n====================================\n""ROUND %d STARTED\n""====================================\n", gameState.current_round);

        /*
         * Continue playing turns until every player
         * passes GO during this round.
         */
        while (!allPlayersPassedGo(player, passesAtRoundStart))
        {
            printf("\n---------- Turn %d ----------\n", gameState.current_turn);

            /*
             * One complete turn:
             * every player rolls and moves.
             */
            for (int i = 0; i < NUM_PLAYERS; i++)
            {
                gameState.current_player = turnOrder[i];

                Player *currentPlayer =
                    &player[gameState.current_player];

                printf("\n--- %s's movement ---\n\n", getPlayerName(currentPlayer->ID));

                player_move(currentPlayer);

                printf("%s is now on %s.\n\n", getPlayerName(currentPlayer->ID), gameboard[currentPlayer->current_position].name);

                switch(gameboard[currentPlayer->current_position].Types){
                    
                    case Square_Property:
                        
                        break;

                    case Square_Event:

                        break; 

                    case Square_Tax:

                        break;
                    
                    case Square_Railway:

                        break;
                        
                    case Square_Utility:

                        break;

                    case Square_Jail:

                        break;

                    case Square_Goto_Jail:

                        break;

                    case Square_Free_Parking:
                        
                        break;
                        
                    case Square_Bank:
                        
                        break;

                    case Square_Insurance:

                        break;

                }
            }

            printf(
                "\nTurn %d completed.\n",
                gameState.current_turn
            );

            gameState.current_turn++;
        }

        printf(
            "\n====================================\n"
            "ROUND %d COMPLETED\n"
            "Every player passed GO.\n"
            "====================================\n",
            gameState.current_round
        );

        gameState.current_round++;
    }

    printf(
        "\nGame ended after %d rounds.\n",
        gameState.max_rounds
    );
}