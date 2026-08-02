#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "types.h"

Square gameboard[40];

void InitializePropertySquare(short SquareIndex, const char* nameofProperty, SquareTypes Square_N, PropertyGroup GROUP_COLOUR, 
                              int PURCHASE_PRICE, int MORTGAGE, int baseRent, 
                              int HOUSE_COST, int HOTEL_COST) {

    // Square Index, Name, Type and Colour Initializing
    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name, nameofProperty);
    gameboard[SquareIndex].Types = Square_N;
    gameboard[SquareIndex].Data.property.group = GROUP_COLOUR;

    // Adding prices and variating properties
    gameboard[SquareIndex].Data.property.current_market_value = PURCHASE_PRICE;
    gameboard[SquareIndex].Data.property.property_purchase_price = PURCHASE_PRICE;
    gameboard[SquareIndex].Data.property.mortgage_value = MORTGAGE;
    gameboard[SquareIndex].Data.property.base_rent = baseRent;
    
    // Rent Tiers based on buildings
    gameboard[SquareIndex].Data.property.rent_with_buildings[0] = baseRent;
    gameboard[SquareIndex].Data.property.rent_with_buildings[1] = baseRent*2;
    gameboard[SquareIndex].Data.property.rent_with_buildings[2] = baseRent*3;
    gameboard[SquareIndex].Data.property.rent_with_buildings[3] = baseRent*5;
    gameboard[SquareIndex].Data.property.rent_with_buildings[4] = baseRent*7;
    gameboard[SquareIndex].Data.property.rent_with_buildings[5] = baseRent*10;

    gameboard[SquareIndex].Data.property.house_price = HOUSE_COST;
    gameboard[SquareIndex].Data.property.hotel_price = HOTEL_COST;

    // Banking and mortgages
    gameboard[SquareIndex].Data.property.ownerID = OWNER_BANK;           
    gameboard[SquareIndex].Data.property.isMortgaged = false;  
    gameboard[SquareIndex].Data.property.isLoanLocked = false; 
    
    // Buildings & Maintenance Starting State
    gameboard[SquareIndex].Data.property.houses_count = 0;
    gameboard[SquareIndex].Data.property.hasHotel = false;
    gameboard[SquareIndex].Data.property.conditionofproperty = 100; 
    
    // Insurance & Age Starting State
    gameboard[SquareIndex].Data.property.insurance = INSURANCE_NONE;
    gameboard[SquareIndex].Data.property.insuranceExpireRounds = 0;
    gameboard[SquareIndex].Data.property.propertyAge = 0;
    gameboard[SquareIndex].Data.property.depreciationPercentage = 0;

    //
    gameboard[SquareIndex].Data.property.isDamaged = false;
    gameboard[SquareIndex].Data.property.isClosed = false; 
}

void Add_Data_Normal_Square(short SquareIndex, const char* nameofSquare, SquareTypes Square_N) {
    gameboard[SquareIndex].index = SquareIndex;
    strcpy(gameboard[SquareIndex].name, nameofSquare);
    gameboard[SquareIndex].Types = Square_N;
}

void initialize_board() {
    
    Add_Data_Normal_Square(  0, "GO",                           Square_Start);
    InitializePropertySquare( 1, "Pettah",                       Square_Property, Group_Brown,       1500,  750,  100,    9000,   2000);
    Add_Data_Normal_Square(  2, "Community Development Fund",   Square_Event);
    InitializePropertySquare( 3, "Maradana",                     Square_Property, Group_Brown,       1800,  900,  120,    10800,   2000);
    Add_Data_Normal_Square(  4, "Income Tax",                   Square_Tax);
    
    gameboard[5].index = 5;
    gameboard[5].Types = Square_Railway;
    strcpy(gameboard[5].name, "Colombo Fort Railway Station");
    gameboard[5].Data.railway.purchase_price = 8000;
    gameboard[5].Data.railway.mortgage_value = 4000;
    gameboard[5].Data.railway.current_market_value = 0;
    gameboard[5].Data.railway.ownerID = OWNER_BANK;
    gameboard[5].Data.railway.isMortgaged = false;
    gameboard[5].Data.railway.isLoanLocked = false;
    
    InitializePropertySquare( 6, "Bambalapitiya",                Square_Property, Group_Light_Blue,  2500, 1250,  180,    16200,   2250);
    Add_Data_Normal_Square(  7, "National Event Card",          Square_Event);
    InitializePropertySquare( 8, "Wellawatte",                   Square_Property, Group_Light_Blue,  2700, 1350,  200,    18000,   2250);
    InitializePropertySquare( 9, "Mount Lavinia",                Square_Property, Group_Light_Blue,  3000, 1500,  220,    19800,   2250);

    Add_Data_Normal_Square( 10, "Jail / Just Visiting",         Square_Jail);
    InitializePropertySquare(11, "Nugegoda",                    Square_Property, Group_Pink,        3500, 1750,  260,     23400,   2500);
    
    gameboard[12].index = 12;
    gameboard[12].Types = Square_Utility;
    strcpy(gameboard[5].name, "Ceylon Electricity Board");
    gameboard[12].Data.utility.purchase_price = 0;
    gameboard[12].Data.utility.mortgage_value = 0;
    gameboard[12].Data.utility.current_market_value = 0;
    gameboard[12].Data.utility.ownerID = OWNER_BANK;
    gameboard[12].Data.utility.isMortgaged = false;
    gameboard[12].Data.utility.isLoanLocked = false;

    InitializePropertySquare(13, "Maharagama",                  Square_Property, Group_Pink,        3800, 1900,  280,     25200,   2500);
    InitializePropertySquare(14, "Kottawa",                     Square_Property, Group_Pink,        4000, 2000,  300,     27000,   2500);
    
    gameboard[15].index = 15;
    gameboard[15].Types = Square_Railway;
    strcpy(gameboard[15].name, "Kandy Railway Station");
    gameboard[15].Data.railway.purchase_price = 8000;
    gameboard[15].Data.railway.mortgage_value = 4000;
    gameboard[15].Data.railway.current_market_value = 0;
    gameboard[15].Data.railway.ownerID = OWNER_BANK;
    gameboard[15].Data.railway.isMortgaged = false;
    gameboard[15].Data.railway.isLoanLocked = false;

    InitializePropertySquare(16, "Negombo",                     Square_Property, Group_Orange,      4500, 2250,  350,     2750, 6000);
    Add_Data_Normal_Square( 17, "Sri Lanka Insurance",          Square_Insurance);
    InitializePropertySquare(18, "Katunayake",                  Square_Property, Group_Orange,      4700, 2250,  370,     1250 , 5000 );
    InitializePropertySquare(19, "Ja-Ela",                      Square_Property, Group_Orange,      5000, 2500,  400,     2750, 6000);

    Add_Data_Normal_Square( 20, "Free Parking",                 Square_Free_Parking);
    InitializePropertySquare(21, "Kandy City",                  Square_Property, Group_Red,         5500, 2750,  450,     40500,   3000);
    Add_Data_Normal_Square( 22, "National Event Card",          Square_Event);
    InitializePropertySquare(23, "Peradeniya",                  Square_Property, Group_Red,         5800, 2900,  480,     43200,   3000);
    InitializePropertySquare(24, "Katugastota",                 Square_Property, Group_Red,         6000, 3000,  500,     45000,   3000);
    
    gameboard[25].index = 25;
    gameboard[25].Types = Square_Railway;
    strcpy(gameboard[25].name, "Galle Railway Station");
    gameboard[25].Data.railway.purchase_price = 8000;
    gameboard[25].Data.railway.mortgage_value = 4000;
    gameboard[25].Data.railway.current_market_value = 0;
    gameboard[25].Data.railway.ownerID = -1;
    gameboard[25].Data.railway.isMortgaged = false;
    gameboard[25].Data.railway.isLoanLocked = false;
    
    InitializePropertySquare(26, "Galle Fort",                  Square_Property, Group_Yellow,      6500, 3250,  600,     54000,   4000);
    InitializePropertySquare(27, "Unawatuna",                   Square_Property, Group_Yellow,      6800, 3400,  620,     55800,   4000);
    
    gameboard[28].index = 28;
    gameboard[28].Types = Square_Utility;
    strcpy(gameboard[28].name, "National Water Supply and Drainage Board");
    gameboard[28].Data.utility.purchase_price = 0;
    gameboard[28].Data.utility.mortgage_value = 0;
    gameboard[28].Data.utility.current_market_value = 0;
    gameboard[28].Data.utility.ownerID = OWNER_BANK;
    gameboard[28].Data.utility.isMortgaged = false;
    gameboard[28].Data.utility.isLoanLocked = false;
    
    InitializePropertySquare(29, "Hikkaduwa",                   Square_Property, Group_Yellow,      7000, 3500,  650,     58500,   4000);
    Add_Data_Normal_Square( 30, "Go To Jail",                   Square_Goto_Jail);
    InitializePropertySquare(31, "Jaffna Town",                 Square_Property, Group_Green,       8000, 4000,  750,     67500,   5000);
    InitializePropertySquare(32, "Nallur",                      Square_Property, Group_Green,       8300, 4150,  780,     70200,   5000);
    Add_Data_Normal_Square( 33, "Ceylinco Insurance",           Square_Insurance);
    InitializePropertySquare(34, "Trincomalee",                 Square_Property, Group_Green,       8500, 4250,  800,     72000,   5000);
    
    gameboard[35].index = 35;
    gameboard[35].Types = Square_Railway;
    strcpy(gameboard[35].name, "Jaffna Railway Station");
    gameboard[35].Data.railway.purchase_price = 8000;
    gameboard[35].Data.railway.mortgage_value = 4000;
    gameboard[35].Data.railway.current_market_value = 0;
    gameboard[35].Data.railway.ownerID = OWNER_BANK;
    gameboard[35].Data.railway.isMortgaged = false;
    gameboard[35].Data.railway.isLoanLocked = false;

    Add_Data_Normal_Square( 36, "National Event Card",          Square_Event);
    InitializePropertySquare(37, "Nuwara Eliya",                Square_Property, Group_Dark_Blue,  10000, 5000, 1000,     90000,   6000);
    Add_Data_Normal_Square( 38, "Bank of Ceylon",               Square_Bank);
    InitializePropertySquare(39, "Galle Face",                  Square_Property, Group_Dark_Blue,  12000, 6000, 1200,     108000,  6000);
}