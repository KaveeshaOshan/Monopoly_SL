#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

