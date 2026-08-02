#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

void gameState(){

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

}

typedef struct{

    int dice1;
    int dice2;
    int sum_of_dice;

}DiceOperations;

DiceOperations get_random_dice_values(){

    DiceOperations dice;

    dice.dice1 = (rand()%6) + 1;

    dice.dice2 = (rand()%6) + 1;

    dice.sum_of_dice = dice.dice1 + dice.dice2;

    return dice;


}

void movePlayer(player *p, int dice)
{
    p->current_position += dice;

    if(p->current_position >= 40)
    {
        p->current_position -= 40;
        p->cash_balance += 2000;
    }
}

void playerTurn(){

    DiceOperations dice_value_A = get_random_dice_values();
    DiceOperations dice_value_C = get_random_dice_values();
    DiceOperations dice_value_R = get_random_dice_values();
    DiceOperations dice_value_O = get_random_dice_values();

    if

}

int main(){
    
    srand(time(NULL));
    
    playerTurn();

}