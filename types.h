#ifndef TYPES_H_
#define TYPES_H_

typedef enum{

    Square_Start,
    Square_Property,
    Square_Event,
    Square_Tax,
    Square_Railway,
    Square_Special,
    Square_Utility,
    Square_Insurance,
    Square_Bank

} SquareTypes;

typedef enum {

    Group_None,
    Group_Brown,
    Group_Light_Blue,
    Group_Pink,
    Group_Orange,
    Group_Red,
    Group_Yellow,
    Group_Green,
    Group_Dark_Blue

} PropertyGroup;

typedef enum{

    INSURANCE_NONE,
    INSURANCE_BASIC,
    INSURANCE_COMPREHENSIVE,
    INSURANCE_BUSINESS

} InsuranceType;

typedef struct{

    PropertyGroup group;
    char name[50]; // Name of property "Pettah, Kandy City, Galle Fort etc."

    int property_purchase_price;
    int mortgage_value;
    int house_price;
    int hotel_price;
    int base_rent;

    short ownerID;
    short mortgage_status;
    short isLoanLocked;

    short houses_count;
    short hotel_count;
    short conditionofproperty;

    InsuranceType insurance;
    int insuranceExpireRounds;

    short age;
    int depreciationPercentage;

} Property;

typedef struct{

    short index;
    char name[50];
    SquareTypes Types;

    Property PropertyData;

} Square;

typedef struct{

    //player identification
    char name[50];
    char strategy_type[50];
    short ID;

    //player money and net worth
    short cash_balance;
    short isBankrupt;
    int networth;

    //player position and locations
    short current_position;
    short jail_status;
    short turns_remaining_Injail;

    //players property ownerships
    short railwayOwned;
    short UtilitiesOwned;
    short totalPropertiesOwned;

    //player loans and stuff
    short hasLoan;
    short LoanPrincipal;
    short accruedInterest;
    short LoanInterestRate;
    short loanRoundRemaining;

} player;

#endif