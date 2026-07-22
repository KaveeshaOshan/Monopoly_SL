#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct dice {

    int dice1;
    int dice2;
    int sum_of_dice;

};

struct cell {

    char cell_type[40];
     column;
     index;


};

struct cell_type {

    for(int i=0; i<11; i++){

        for(int j=0; j<11; j++){

            arr[i][j]=0;

        }

    }

};

struct dice get_random_value(){

    struct dice D1;

    D1.dice1 = (rand()%6) + 1;

    D1.dice2 = (rand()%6) + 1;

    D1.sum_of_dice = D1.dice1 + D1.dice2;

    return D1;


}

int board_func(){




}