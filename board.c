#include <stdio.h>
#include <string.h>
#include "Types.h"

Square gameboard[40];

void Add_data_Special_Square(short SquareIndex, char nameofProperty[50], SquareTypes Square_N, PropertyGroup GROUP_COLOUR, short HOUSE_PRICE, short MORTGAGE, short baseRent, short houseCost, short hotelCost){

    // Square Index, Name, Type and Colour Initializing
    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name,nameofProperty);
    gameboard[SquareIndex].Types = Square_N;
    gameboard[SquareIndex].PropertyData.group = GROUP_COLOUR;

    // Adding prices and variating propities
    gameboard[SquareIndex].PropertyData.house_price = HOUSE_PRICE;
    gameboard[SquareIndex].PropertyData.mortgage_value = MORTGAGE;
    gameboard[SquareIndex].PropertyData.house_rent = baseRent;
    gameboard[SquareIndex].PropertyData.house_price = houseCost;
    gameboard[SquareIndex].PropertyData.hotel_price = hotelCost;

    // Banking and mortgages
    gameboard[SquareIndex].PropertyData.ownerID = 0;      // 0 means owned by Bank
    gameboard[SquareIndex].PropertyData.mortgage_status = 0;  // 0 for false
    gameboard[SquareIndex].PropertyData.isLoanLocked = 0; // 0 for false
    
    // Buildings & Maintenance Starting State
    gameboard[SquareIndex].PropertyData.houses_count = 0;
    gameboard[SquareIndex].PropertyData.hotel_count = 0;
    gameboard[SquareIndex].PropertyData.conditionofproperty = 100; // Starts at 100%
    
    // Insurance & Age Starting State
    gameboard[SquareIndex].PropertyData.insurance = INSURANCE_NONE;
    gameboard[SquareIndex].PropertyData.insuranceExpireRounds = 0;
    gameboard[SquareIndex].PropertyData.age = 0;
    gameboard[SquareIndex].PropertyData.depreciationPercentage = 0;
    

}

void Add_Data_Normal_Square(short SquareIndex, char nameofSquare[50], SquareTypes Square_N){

    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name,nameofSquare);
    gameboard[SquareIndex].Types = Square_N;

}

void initializing_Board(){
    //Square 0 init...
    Add_Data_Normal_Square(0,"GO",Square_Start);

    //Square 1 init...
    Add_data_Special_Square(1,"Pettah",Square_Property, Group_Brown,2500,1250,250,2000,3000);

    //Square 2 init...
    
    
}