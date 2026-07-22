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

#endif