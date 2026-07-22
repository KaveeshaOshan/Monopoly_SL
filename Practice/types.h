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

} Square_Types;

typedef enum {

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
    char name[50];

    short purchase_price;
    short mortgage_value;
    short house_price;
    short hotel_price;
    short house_rent;

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

#endif