#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>

typedef enum{

    Square_Start,
    Square_Property,
    Square_Event,
    Square_Tax,
    Square_Railway,
    Square_Utility,
    Square_Jail,
    Square_Goto_Jail,
    Square_Free_Parking,
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

typedef enum{

    OWNER_BANK = -1,
    OWNER_AGGRESSIVE_INVESTOR,
    OWNER_CONSERVATIVE_BANKER,
    OWNER_RISK_TAKER,
    OWNER_OPPORTUNISTIC_TRADER

} OwnerID;

typedef enum{

    ASSET_PROPERTY,
    ASSET_UTILITY,
    ASSET_RAILWAY

} AssetType;

typedef enum{
    CARD_TOURISM_HYPE,
    CARD_FUEL_SHORTAGE,
    CARD_HEAVY_FLOODS,
    CARD_POLITICAL_RALLY,
    CARD_STOCK_MARKET_RISE,
    CARD_ECONOMIC_DOWNTURN,
    CARD_HOUSING_SUBSIDY,
    CARD_INTEREST_RATE_CUT,
    CARD_INTEREST_RATE_INCREASE,
    CARD_TAX_AMNESTY,
    CARD_POWER_FAILURE,
    CARD_FOREIGN_FUNDING,
    CARD_PORT_EXPANSION,
    CARD_FESTIVAL_SEASON,
    CARD_LABOUR_STRIKE,
    CARD_INSURANCE_DISCOUNT,
    CARD_PROPERTY_REVALUATION,
    CARD_CURRENCY_DEPRECIATION,
    CARD_GOVERNMENT_GRANT,
    CARD_NATIONAL_DISASTER,

    CARD_COUNT

} EventCardType;

typedef enum{

    LOAN_NONE,
    LOAN_HAVE,
    LOAN_PAID,
    LOAN_DEFAULTED,
    LOAN_FORECLOSED

} LoanStatus;

typedef struct{

    PropertyGroup group;

    int property_purchase_price;
    int current_market_value;
    int mortgage_value;
    int house_price;
    int hotel_price;
    int base_rent;
    int rent_with_buildings[6];

    OwnerID ownerID;    
    bool isMortgaged;
    bool isLoanLocked;

    short houses_count;
    bool hasHotel;
    short conditionofproperty;

    InsuranceType insurance;
    int insuranceExpireRounds;

    short propertyAge;
    int depreciationPercentage;

    bool isDamaged;
    bool isClosed;

} Property;

typedef struct{
    SquareTypes Types;
    int purchase_price;
    int mortgage_value;
    int current_market_value;

    OwnerID ownerID;

    int isMortgaged;
    int isLoanLocked;

} Railway;

typedef struct{
    SquareTypes Types;
    int purchase_price;
    int mortgage_value;
    int current_market_value;

    OwnerID ownerID;

    bool isMortgaged;
    bool isLoanLocked;

} Utility;

typedef union{

    Property property;
    Railway railway;
    Utility utility;

} SquareData;

typedef struct{

    short index;
    char name[50];
    SquareTypes Types;

    SquareData Data;

} Square;

typedef enum{
    STRATEGY_AGGRESSIVE,
    STRATEGY_CONSERVATIVE,
    STRATEGY_RISK_TAKER,
    STRATEGY_OPPORTUNISTIC
} StrategyType;

typedef struct{
    int loanAmount;
    int loanOutstanding;
    int startround;
    int interestRate;
    int roundsRemaining;
    
    LoanStatus status;

} Loan;

typedef struct{

    //player identification
    char name[50];
    StrategyType strategy;
    short ID;

    //player money and net worth
    int cash_balance;
    bool isBankrupt;
    int networth;

    //player position and locations
    short current_position;
    bool in_jail;
    short turns_remaining_Injail;

    //players property ownerships
    short railwayOwned;
    short UtilitiesOwned;
    short totalPropertiesOwned;

    bool ownedAssets[40];

    //player loans and stuff    
    Loan loan;

} Player;

#endif