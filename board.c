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
    Add_Data_Normal_Square(2, "Community Development Fund", Square_Event);

    //Square 3 init...
    Add_Property_Square(3, "Maradana", Square_Property, Group_Brown, 2500, 1250, 250, 2000, 3000);

    //Square 4 init...
    Add_Data_Normal_Square(4, "Income Tax", Square_Tax);

    //Square 5 init... (Railway - Dynamic rent, no buildings)
    Add_Property_Square(5, "Colombo Fort Railway Station", Square_Railway, Group_None, 8000, 4000, 0, 0, 0);

    //Square 6 init...
    Add_Property_Square(6, "Bambalapitiya", Square_Property, Group_Light_Blue, 3500, 1750, 350, 2250, 4000);

    //Square 7 init...
    Add_Data_Normal_Square(7, "National Event Card", Square_Event);

    //Square 8 init...
    Add_Property_Square(8, "Wellawatte", Square_Property, Group_Light_Blue, 3500, 1750, 350, 2250, 4000);

    //Square 9 init...
    Add_Property_Square(9, "Mount Lavinia", Square_Property, Group_Light_Blue, 3500, 1750, 350, 2250, 4000);

    //Square 10 init...
    Add_Data_Normal_Square(10, "Jail/Just Visiting", Square_Special);

    //Square 11 init...
    Add_Property_Square(11, "Nugegoda", Square_Property, Group_Pink, 4500, 2250, 450, 2500, 5000);

    //Square 12 init... (Utility - Dynamic rent, no buildings)
    Add_Property_Square(12, "Ceylon Electricity Board", Square_Utility, Group_None, 3000, 1500, 0, 0, 0);

    //Square 13 init...
    Add_Property_Square(13, "Maharagama", Square_Property, Group_Pink, 4500, 2250, 450, 2500, 5000);

    //Square 14 init...
    Add_Property_Square(14, "Kottawa", Square_Property, Group_Pink, 4500, 2250, 450, 2500, 5000);

    //Square 15 init...
    Add_Property_Square(15, "Kandy Railway Station", Square_Railway, Group_None, 8000, 4000, 0, 0, 0);

    //Square 16 init...
    Add_Property_Square(16, "Negombo", Square_Property, Group_Orange, 5500, 2750, 550, 2750, 6000);

    //Square 17 init...
    Add_Data_Normal_Square(17, "Sri Lanka Insurance", Square_Insurance);

    //Square 18 init...
    Add_Property_Square(18, "Katunayake", Square_Property, Group_Orange, 5500, 2750, 550, 2750, 6000);

    //Square 19 init...
    Add_Property_Square(19, "Ja-Ela", Square_Property, Group_Orange, 5500, 2750, 550, 2750, 6000);

    //Square 20 init...
    Add_Data_Normal_Square(20, "Free Parking", Square_Special);

    //Square 21 init...
    Add_Property_Square(21, "Kandy City", Square_Property, Group_Red, 6500, 3250, 650, 3000, 8000);

    //Square 22 init...
    Add_Data_Normal_Square(22, "National Event Card", Square_Event);

    //Square 23 init...
    Add_Property_Square(23, "Peradeniya", Square_Property, Group_Red, 6500, 3250, 650, 3000, 8000);

    //Square 24 init...
    Add_Property_Square(24, "Katugastota", Square_Property, Group_Red, 6500, 3250, 650, 3000, 8000);

    //Square 25 init...
    Add_Property_Square(25, "Galle Railway Station", Square_Railway, Group_None, 8000, 4000, 0, 0, 0);

    //Square 26 init...
    Add_Property_Square(26, "Galle Fort", Square_Property, Group_Yellow, 8000, 4000, 800, 4000, 10000);

    //Square 27 init...
    Add_Property_Square(27, "Unawatuna", Square_Property, Group_Yellow, 8000, 4000, 800, 4000, 10000);

    //Square 28 init...
    Add_Property_Square(28, "Water Supply and Drainage Board", Square_Utility, Group_None, 3000, 1500, 0, 0, 0);

    //Square 29 init...
    Add_Property_Square(29, "Hikkaduwa", Square_Property, Group_Yellow, 8000, 4000, 800, 4000, 10000);

    //Square 30 init...
    Add_Data_Normal_Square(30, "Go To Jail", Square_Special);

    //Square 31 init...
    Add_Property_Square(31, "Jaffna Town", Square_Property, Group_Green, 10000, 5000, 1000, 5000, 12000);

    //Square 32 init...
    Add_Property_Square(32, "Nallur", Square_Property, Group_Green, 10000, 5000, 1000, 5000, 12000);

    //Square 33 init...
    Add_Data_Normal_Square(33, "Ceylinco Insurance", Square_Insurance);

    //Square 34 init...
    Add_Property_Square(34, "Trincomalee", Square_Property, Group_Green, 10000, 5000, 1000, 5000, 12000);

    //Square 35 init...
    Add_Property_Square(35, "Jaffna Railway Station", Square_Railway, Group_None, 8000, 4000, 0, 0, 0);

    //Square 36 init...
    Add_Data_Normal_Square(36, "National Event Card", Square_Event);

    //Square 37 init...
    Add_Property_Square(37, "Nuwara Eliya", Square_Property, Group_Dark_Blue, 12000, 6000, 1200, 6000, 15000);

    //Square 38 init...
    Add_Data_Normal_Square(38, "Bank of Ceylon", Square_Bank);

    //Square 39 init...
    Add_Property_Square(39, "Galle Face", Square_Property, Group_Dark_Blue, 12000, 6000, 1200, 6000, 15000);
    
    
}