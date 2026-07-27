#include <stdio.h>
#include <string.h>
#include "types.h"

Square gameboard[40];

void InitializePropertySquare(short SquareIndex, const char* nameofProperty, SquareTypes Square_N, PropertyGroup GROUP_COLOUR, int PURCHASE_PRICE, int MORTGAGE, int baseRent, int HOUSE_COST, int HOTEL_COST){

    // Square Index, Name, Type and Colour Initializing
    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name,nameofProperty);
    gameboard[SquareIndex].Types = Square_N;
    gameboard[SquareIndex].PropertyData.group = GROUP_COLOUR;

    // Adding prices and variating propities
    gameboard[SquareIndex].PropertyData.property_purchase_price = PURCHASE_PRICE;
    gameboard[SquareIndex].PropertyData.mortgage_value = MORTGAGE;
    gameboard[SquareIndex].PropertyData.base_rent = baseRent;
    gameboard[SquareIndex].PropertyData.house_price = HOUSE_COST;
    gameboard[SquareIndex].PropertyData.hotel_price = HOTEL_COST;

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

void Add_Data_Normal_Square(short SquareIndex, const char* nameofSquare, SquareTypes Square_N){

    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name,nameofSquare);
    gameboard[SquareIndex].Types = Square_N;

}

void initialize_board() {
    
    Add_Data_Normal_Square(  0, "GO",                           Square_Start);
    InitializePropertySquare(1, "Pettah",                       Square_Property, Group_Brown,      2500, 1250,  250, 2000, 3000);
    Add_Data_Normal_Square(  2, "Community Development Fund",   Square_Event);
    InitializePropertySquare(3, "Maradana",                     Square_Property, Group_Brown,      2500, 1250,  250, 2000, 3000);
    Add_Data_Normal_Square(  4, "Income Tax",                   Square_Tax);
    InitializePropertySquare(5, "Colombo Fort Railway Station", Square_Railway,  Group_None,       8000, 4000,    0,    0,    0);
    InitializePropertySquare(6, "Bambalapitiya",                Square_Property, Group_Light_Blue, 3500, 1750,  350, 2250, 4000);
    Add_Data_Normal_Square(  7, "National Event Card",          Square_Event);
    InitializePropertySquare(8, "Wellawatte",                   Square_Property, Group_Light_Blue, 3500, 1750,  350, 2250, 4000);
    InitializePropertySquare(9, "Mount Lavinia",                Square_Property, Group_Light_Blue, 3500, 1750,  350, 2250, 4000);

    Add_Data_Normal_Square( 10, "Jail / Just Visiting",         Square_Special);
    InitializePropertySquare(11, "Nugegoda",                    Square_Property, Group_Pink,       4500, 2250,  450, 2500, 5000);
    InitializePropertySquare(12, "Ceylon Electricity Board",    Square_Utility,  Group_None,       3000, 1500,    0,    0,    0);
    InitializePropertySquare(13, "Maharagama",                  Square_Property, Group_Pink,       4500, 2250,  450, 2500, 5000);
    InitializePropertySquare(14, "Kottawa",                     Square_Property, Group_Pink,       4500, 2250,  450, 2500, 5000);
    InitializePropertySquare(15, "Kandy Railway Station",       Square_Railway,  Group_None,       8000, 4000,    0,    0,    0);
    InitializePropertySquare(16, "Negombo",                     Square_Property, Group_Orange,     5500, 2750,  550, 2750, 6000);
    Add_Data_Normal_Square( 17, "Sri Lanka Insurance",          Square_Insurance);
    InitializePropertySquare(18, "Katunayake",                  Square_Property, Group_Orange,     5500, 2750,  550, 2750, 6000);
    InitializePropertySquare(19, "Ja-Ela",                      Square_Property, Group_Orange,     5500, 2750,  550, 2750, 6000);

    Add_Data_Normal_Square( 20, "Free Parking",                 Square_Special);
    InitializePropertySquare(21, "Kandy City",                  Square_Property, Group_Red,        6500, 3250,  650, 3000, 8000);
    Add_Data_Normal_Square( 22, "National Event Card",          Square_Event);
    InitializePropertySquare(23, "Peradeniya",                  Square_Property, Group_Red,        6500, 3250,  650, 3000, 8000);
    InitializePropertySquare(24, "Katugastota",                 Square_Property, Group_Red,        6500, 3250,  650, 3000, 8000);
    InitializePropertySquare(25, "Galle Railway Station",       Square_Railway,  Group_None,       8000, 4000,    0,    0,    0);
    InitializePropertySquare(26, "Galle Fort",                  Square_Property, Group_Yellow,     8000, 4000,  800, 4000,10000);
    InitializePropertySquare(27, "Unawatuna",                   Square_Property, Group_Yellow,     8000, 4000,  800, 4000,10000);
    InitializePropertySquare(28, "Water Board",                 Square_Utility,  Group_None,       3000, 1500,    0,    0,    0);
    InitializePropertySquare(29, "Hikkaduwa",                   Square_Property, Group_Yellow,     8000, 4000,  800, 4000,10000);

    Add_Data_Normal_Square( 30, "Go To Jail",                   Square_Special);
    InitializePropertySquare(31, "Jaffna Town",                 Square_Property, Group_Green,     10000, 5000, 1000, 5000,12000);
    InitializePropertySquare(32, "Nallur",                      Square_Property, Group_Green,     10000, 5000, 1000, 5000,12000);
    Add_Data_Normal_Square( 33, "Ceylinco Insurance",           Square_Insurance);
    InitializePropertySquare(34, "Trincomalee",                 Square_Property, Group_Green,     10000, 5000, 1000, 5000,12000);
    InitializePropertySquare(35, "Jaffna Railway Station",      Square_Railway,  Group_None,       8000, 4000,    0,    0,    0);
    Add_Data_Normal_Square( 36, "National Event Card",          Square_Event);
    InitializePropertySquare(37, "Nuwara Eliya",                Square_Property, Group_Dark_Blue, 12000, 6000, 1200, 6000,15000);
    Add_Data_Normal_Square( 38, "Bank of Ceylon",               Square_Bank);
    InitializePropertySquare(39, "Galle Face",                  Square_Property, Group_Dark_Blue, 12000, 6000, 1200, 6000,15000);
}