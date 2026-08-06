#ifndef BOARD_H_
#define BOARD_H_

#include "types.h"

void InitializePropertySquare(Square gameboard[40], short SquareIndex, const char* nameofProperty, SquareTypes Square_N, PropertyGroup GROUP_COLOUR, int PURCHASE_PRICE, int MORTGAGE, int baseRent, int HOUSE_COST, int HOTEL_COST);

void Add_Data_Normal_Square(Square gameboard[40], short SquareIndex, const char* nameofSquare, SquareTypes Square_N);

void initialize_board(Square gameboard[40]);

#endif