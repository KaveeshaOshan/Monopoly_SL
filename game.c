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

bool player_move(Player *theplayer, int *diceTotal){

    if (theplayer == NULL || diceTotal == NULL)
    {
        return false;
    }

    *diceTotal = 0;

    /*
     * A jailed player does not move while jail turns remain.
     */
    if (theplayer->in_jail)
    {
        if (theplayer->turns_remaining_Injail > 0)
        {
            printf(
                "%s is in jail and cannot move this turn.\n",
                getPlayerName(theplayer->ID)
            );

            theplayer->turns_remaining_Injail--;

            return false;
        }

        theplayer->in_jail = false;

        printf(
            "%s has been released from jail.\n",
            getPlayerName(theplayer->ID)
        );
    }

    DiceOperations dice = get_random_dice_values();

    *diceTotal = dice.sum_of_dice;

    printf(
        "%s rolled %d and %d. Total = %d\n",
        getPlayerName(theplayer->ID),
        dice.dice1,
        dice.dice2,
        dice.sum_of_dice
    );

    int oldPosition = theplayer->current_position;

    int newPosition =
        (oldPosition + dice.sum_of_dice) % NUM_SQUARES;

    printf(
        "%s moved from Square %d to Square %d.\n",
        getPlayerName(theplayer->ID),
        oldPosition,
        newPosition
    );

    /*
     * The player passed or landed on GO.
     */
    if (oldPosition + dice.sum_of_dice >= NUM_SQUARES)
    {
        theplayer->passed_go++;
        theplayer->cash_balance += 2000;
        theplayer->networth += 2000;

        printf(
            "%s passed GO and collected LKR 2000.\n",
            getPlayerName(theplayer->ID)
        );
    }

    theplayer->current_position = (short)newPosition;

    return true;
}

bool allPlayersPassedGo(const Player players[NUM_PLAYERS], const int passesAtRoundStart[NUM_PLAYERS]){
    
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (players[i].isBankrupt)
        {
            continue;
        }

        if (players[i].passed_go <= passesAtRoundStart[i])
        {
            return false;
        }
    }

    return true;
}

int calculatePropertyRent(const Property *property){

    if (property == NULL)
    {
        return 0;
    }

    //  cannot currently produce rent.
    if (property->isMortgaged ||
        property->isClosed ||
        property->isDamaged)
    {
        return 0;
    }

    //Index 5 stores the hotel rent.
    if (property->hasHotel)
    {
        return property->rent_with_buildings[5];
    }

    //Counting the rent if it is a normal house
    if (property->houses_count >= 0 &&
        property->houses_count <= 4)
    {
        return property->rent_with_buildings[
            property->houses_count
        ];
    }

    /* Invalid building information. */
    return 0;
}

void handleRent(Player *theplayer, int landed_square){

    int rent_to_pay = calculatePropertyRent(&gameboard[landed_square].Data.property);
        
    int ownerIndex = gameboard[landed_square].Data.property.ownerID;

    if (theplayer->cash_balance < rent_to_pay)
    {
        printf(
            "%s must pay LKR %d rent, but only has LKR %d.\n",
            getPlayerName(theplayer->ID),
            rent_to_pay,
            theplayer->cash_balance
        );
        return;

    }

    //Deducting the rent money from the tenant
    theplayer->cash_balance -= rent_to_pay;

    //Adding the rent money to the owner
    player[ownerIndex].cash_balance += rent_to_pay;

    //Updating the networth of the tenant
    theplayer->networth -= rent_to_pay;

    //Updating the networth of the owner
    player[ownerIndex].networth += rent_to_pay;

    printf("%s paid LKR %d rent to %s.\n", getPlayerName(theplayer->ID), rent_to_pay, getPlayerName(owner->ID));

    printf("%s now has LKR %d.\n", getPlayerName(theplayer->ID), theplayer->cash_balance);

}

void declareBankrupt(Player *bankruptPlayer){

    if (bankruptPlayer == NULL)
    {
        return;
    }

    /* Prevent declaring the same player bankrupt twice. */
    if (bankruptPlayer->isBankrupt)
    {
        return;
    }

    bankruptPlayer->isBankrupt = true;
    bankruptPlayer->cash_balance = 0;
    bankruptPlayer->networth = 0;

    printf(
        "%s has become bankrupt.\n",
        getPlayerName(bankruptPlayer->ID)
    );
}

void handlePropertySquare(Player *theplayer, int landed_square){

    if(gameboard[landed_square].Data.property.ownerID == OWNER_BANK){
        
        switch(theplayer->strategy)
        {
            case STRATEGY_AGGRESSIVE:
                if(theplayer->cash_balance >= (gameboard[landed_square].Data.property.property_purchase_price + gameboard[landed_square].Data.property.base_rent)){

                    theplayer->cash_balance -= (gameboard[landed_square].Data.property.property_purchase_price);
                    gameboard[landed_square].Data.property.ownerID = theplayer->ID;
                    theplayer->totalPropertiesOwned++;
                    theplayer->ownedAssets[landed_square] = true;

                    printf("%s bought %s for LKR %d\n", getPlayerName(theplayer->ID), gameboard[landed_square].name, gameboard[landed_square].Data.property.property_purchase_price);
                    printf("Remaining Balance: LKR %d\n", theplayer->cash_balance);

                }
                break;

            case STRATEGY_CONSERVATIVE:

                if(theplayer->cash_balance*0.5 <= theplayer->cash_balance - gameboard[landed_square].Data.property.property_purchase_price ){

                    theplayer->cash_balance -= (gameboard[landed_square].Data.property.property_purchase_price);
                    gameboard[landed_square].Data.property.ownerID = theplayer->ID;
                    theplayer->totalPropertiesOwned++;
                    theplayer->ownedAssets[landed_square] = true;

                    printf("%s bought %s for LKR %d\n", getPlayerName(theplayer->ID), gameboard[landed_square].name, gameboard[landed_square].Data.property.property_purchase_price);
                    printf("Remaining Balance: LKR %d\n", theplayer->cash_balance);

                }
                
                break;

            case STRATEGY_RISK_TAKER:

                if(theplayer->cash_balance >= gameboard[landed_square].Data.property.property_purchase_price){    

                    theplayer->cash_balance -= (gameboard[landed_square].Data.property.property_purchase_price);
                    gameboard[landed_square].Data.property.ownerID = theplayer->ID;
                    theplayer->totalPropertiesOwned++;
                    theplayer->ownedAssets[landed_square] = true;

                    printf("%s bought %s for LKR %d\n", getPlayerName(theplayer->ID), gameboard[landed_square].name, gameboard[landed_square].Data.property.property_purchase_price);
                    printf("Remaining Balance: LKR %d\n", theplayer->cash_balance);

                }  
                
                break;

            case STRATEGY_OPPORTUNISTIC:

                /**if(theplayer->cash_balance >= ){

                    //The fuuuuuuucccckkkkk

                } **/

                break;
            
        }

    }
    else if(gameboard[landed_square].Data.property.ownerID == theplayer->ID){
        
        printf("No rent is collected.\n");
        printf("%s already owns %s\n", getPlayerName(theplayer->ID), gameboard[landed_square].name);
        
    }
    else{

        if(gameboard[landed_square].Data.property.isMortgaged || gameboard[landed_square].Data.property.isDamaged || gameboard[landed_square].Data.property.isClosed || player[gameboard[landed_square].Data.property.ownerID].isBankrupt){

            printf("%s cannot pay rent\n", getPlayerName(theplayer->ID));
            return;

        }
        if(theplayer->cash_balance < calculatePropertyRent(&gameboard[landed_square].Data.property)){
            
            printf(
                "%s must pay LKR %d rent, but only has LKR %d.\n",
                getPlayerName(theplayer->ID),
                calculatePropertyRent(&gameboard[landed_square].Data.property),
                theplayer->cash_balance
            );
            int ownerIndex = gameboard[landed_square].Data.property.ownerID;

            player[ownerIndex].cash_balance += theplayer->cash_balance;
            player[ownerIndex].networth += theplayer->cash_balance;
            
            theplayer->cash_balance = 0;

            declareBankrupt(theplayer);

        }
        else{

                handleRent(theplayer, landed_square);

        }

        

    }
        

}
    
void startgame(){

    GameState gameState = {0};

    gameState.current_round = 1;
    gameState.max_rounds = MAX_ROUNDS;

    initializePlayers(player);

    initialize_board(gameboard);

    determineTurnOrder(turnOrder);

    initializeEventCards(cards);

    initializeEventDeck(&deck);

    shuffleEventDeck(&deck);

    while (gameState.current_round <= gameState.max_rounds)
    {
        int passesAtRoundStart[NUM_PLAYERS];

        
        // Remember how many times each player had passed GO
        // at the beginning of this round.
        
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            passesAtRoundStart[i] = player[i].passed_go;
        }

        gameState.current_turn = 1;

        printf("\n====================================\n""ROUND %d STARTED\n""====================================\n", gameState.current_round);

        
         // Continue playing turns until every player
         // passes GO during this round.
         
        while (!allPlayersPassedGo(player, passesAtRoundStart))
        {
            printf("\n---------- Turn %d ----------\n", gameState.current_turn);

             // One complete turn:
             // every player rolls and moves.
             
            for (int i = 0; i < NUM_PLAYERS; i++)
            {
                gameState.current_player = turnOrder[i];

                Player *currentPlayer = &player[gameState.current_player];

                if (currentPlayer->isBankrupt){
                    printf("%s is bankrupt and skips this turn.\n", getPlayerName(currentPlayer->ID));
                    
                    //Player is bankrupt so the turn will skip
                    continue;
                }

                printf("\n--- %s's movement ---\n\n", getPlayerName(currentPlayer->ID));

                int diceTotal = 0;

                bool moved = player_move(currentPlayer, &diceTotal);

                if (!moved){
                    // Player is injail so turn will be skiped
                    continue;
                }

                printf("%s is now on %s.\n\n", getPlayerName(currentPlayer->ID), gameboard[currentPlayer->current_position].name);

                switch(gameboard[currentPlayer->current_position].Types){
                    
                    case Square_Start:

                        break;

                    case Square_Property:

                        handlePropertySquare(currentPlayer, currentPlayer->current_position);

                        if(currentPlayer->isBankrupt){

                            break;

                        } 

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

                    default:
                        
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

