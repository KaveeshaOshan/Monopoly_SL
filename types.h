#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>
#define NUM_PLAYERS 4
#define NUM_SQUARES 40
#define NUM_EVENTS 20
#define NUM_RAILWAYS 4
#define NUM_UTILITIES 2
#define NUM_PROPERTIES 22
#define MAX_ROUNDS 500

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

typedef enum
{
    EVENT_EFFECT_CASH_CHANGE,
    EVENT_EFFECT_RENT_CHANGE,
    EVENT_EFFECT_ASSET_VALUE_CHANGE,
    EVENT_EFFECT_CONSTRUCTION_COST_CHANGE,
    EVENT_EFFECT_INTEREST_RATE_CHANGE,
    EVENT_EFFECT_INSURANCE_PREMIUM_CHANGE,
    EVENT_EFFECT_PROPERTY_DAMAGE,
    EVENT_EFFECT_PROPERTY_CLOSURE,
    EVENT_EFFECT_CONSTRUCTION_SUSPENSION

} EventEffectType;

typedef enum{

    TARGET_DRAWING_PLAYER_HOTELS,
    TARGET_DRAWING_PLAYER_RAILWAYS,
    TARGET_RANDOM_COASTAL_PROPERTY,
    TARGET_RANDOM_PROPERTY,
    TARGET_ALL_PROPERTIES,
    TARGET_HOUSE_CONSTRUCTION,
    TARGET_LOAN_INTEREST,
    TARGET_ALL_PLAYERS,
    TARGET_DRAWING_PLAYER_UTILITIES,
    TARGET_COMMERCIAL_PROPERTIES,
    TARGET_ALL_RAILWAYS,
    TARGET_ALL_HOTELS,
    TARGET_CONSTRUCTION_SYSTEM,
    TARGET_INSURANCE_PREMIUMS,
    TARGET_RANDOM_PROPERTY_GROUP,
    TARGET_RANDOM_PLAYER,
    TARGET_RANDOM_DEVELOPED_PROPERTY

} EventTargetType;

typedef enum{

    EVENT_VALUE_NONE,
    EVENT_VALUE_LKR,
    EVENT_VALUE_PERCENTAGE,
    EVENT_VALUE_INTEREST_POINTS

} EventValueType;

typedef enum{

    EVENT_IMMEDIATE,
    EVENT_TEMPORARY

} EventTiming;

typedef struct{

    EventCardType id;

    char name[50];
    char description[160];

    EventEffectType effectType;
    EventTargetType targetType;
    EventValueType valueType;

    int value;
    int duration;

    EventTiming timing;

} EventCard;

typedef struct{

    EventCardType order[CARD_COUNT];

    int topIndex;

} EventDeck;

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

typedef struct {

    int current_round;
    int current_turn;
    int max_rounds;
    int game_state;
    int winner;
    int number_of_players;
    int number_of_solvent_players;
    int current_player;
    int current_inflation;
    int current_interest;
    int current_market_boom;
    int current_echonomic_event;
    int current_reginol_event;
    int current_government_regulation;

} GameState;

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
    int passed_go;

    //players property ownerships
    short railwayOwned;
    short UtilitiesOwned;
    short totalPropertiesOwned;

    bool ownedAssets[40];

    //player loans and stuff    
    Loan loan;

    GameState gameState;

} Player;

typedef enum{

    CASH_EFFECT,
    DAMAGE_EFFECT,
    VALUE_EFFECT,
    COST_EFFECT,
    INTEREST_EFFECT,
    RESTRICTION_EFFECT,
    RENT_EFFECT,

} EffectType;


#endif