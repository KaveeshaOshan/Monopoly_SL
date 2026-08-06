#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "events.h"

//Function with the values of each card
static void setEventCard(EventCard *card, EventCardType id, const char *name, const char *description, EventEffectType effectType, EventTargetType targetType, EventValueType valueType, int value, int duration, EventTiming timing){
    card->id = id;

    snprintf(card->name, sizeof(card->name), "%s", name);

    snprintf(card->description, sizeof(card->description), "%s", description);

    card->effectType = effectType;
    card->targetType = targetType;
    card->valueType = valueType;

    card->value = value;
    card->duration = duration;
    card->timing = timing;
}

//Initializes all event cards
void initializeEventCards(EventCard cards[CARD_COUNT]){

    setEventCard(
        &cards[CARD_TOURISM_HYPE],
        CARD_TOURISM_HYPE,
        "Tourism Hype",
        "Hotels earn double rent for 5 rounds.",
        EVENT_EFFECT_RENT_CHANGE,
        TARGET_DRAWING_PLAYER_HOTELS,
        EVENT_VALUE_PERCENTAGE,
        100,
        5,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_FUEL_SHORTAGE],
        CARD_FUEL_SHORTAGE,
        "Fuel Shortage",
        "Railway rent doubles for 5 rounds.",
        EVENT_EFFECT_RENT_CHANGE,
        TARGET_DRAWING_PLAYER_RAILWAYS,
        EVENT_VALUE_PERCENTAGE,
        100,
        5,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_HEAVY_FLOODS],
        CARD_HEAVY_FLOODS,
        "Heavy Floods",
        "A random coastal property is damaged.",
        EVENT_EFFECT_PROPERTY_DAMAGE,
        TARGET_RANDOM_COASTAL_PROPERTY,
        EVENT_VALUE_NONE,
        0,
        0,
        EVENT_IMMEDIATE
    );

    setEventCard(
        &cards[CARD_POLITICAL_RALLY],
        CARD_POLITICAL_RALLY,
        "Political Rally",
        "One random property is closed for 2 rounds.",
        EVENT_EFFECT_PROPERTY_CLOSURE,
        TARGET_RANDOM_PROPERTY,
        EVENT_VALUE_NONE,
        0,
        2,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_STOCK_MARKET_RISE],
        CARD_STOCK_MARKET_RISE,
        "Stock Market Rise",
        "All property values increase by 10%.",
        EVENT_EFFECT_ASSET_VALUE_CHANGE,
        TARGET_ALL_PROPERTIES,
        EVENT_VALUE_PERCENTAGE,
        10,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_ECONOMIC_DOWNTURN],
        CARD_ECONOMIC_DOWNTURN,
        "Economic Downturn",
        "Property values decrease by 15%.",
        EVENT_EFFECT_ASSET_VALUE_CHANGE,
        TARGET_ALL_PROPERTIES,
        EVENT_VALUE_PERCENTAGE,
        -15,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_HOUSING_SUBSIDY],
        CARD_HOUSING_SUBSIDY,
        "Housing Subsidy",
        "House construction costs are reduced by 30%.",
        EVENT_EFFECT_CONSTRUCTION_COST_CHANGE,
        TARGET_HOUSE_CONSTRUCTION,
        EVENT_VALUE_PERCENTAGE,
        -30,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_INTEREST_RATE_CUT],
        CARD_INTEREST_RATE_CUT,
        "Interest Rate Cut",
        "Loan interest is reduced by 2 percentage points.",
        EVENT_EFFECT_INTEREST_RATE_CHANGE,
        TARGET_LOAN_INTEREST,
        EVENT_VALUE_INTEREST_POINTS,
        -2,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_INTEREST_RATE_INCREASE],
        CARD_INTEREST_RATE_INCREASE,
        "Interest Rate Increase",
        "Loan interest is increased by 2 percentage points.",
        EVENT_EFFECT_INTEREST_RATE_CHANGE,
        TARGET_LOAN_INTEREST,
        EVENT_VALUE_INTEREST_POINTS,
        2,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_TAX_AMNESTY],
        CARD_TAX_AMNESTY,
        "Tax Amnesty",
        "Each player receives LKR 2,000.",
        EVENT_EFFECT_CASH_CHANGE,
        TARGET_ALL_PLAYERS,
        EVENT_VALUE_LKR,
        2000,
        0,
        EVENT_IMMEDIATE
    );

    setEventCard(
        &cards[CARD_POWER_FAILURE],
        CARD_POWER_FAILURE,
        "Power Failure",
        "Utility income is halved for 3 rounds.",
        EVENT_EFFECT_RENT_CHANGE,
        TARGET_DRAWING_PLAYER_UTILITIES,
        EVENT_VALUE_PERCENTAGE,
        -50,
        3,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_FOREIGN_FUNDING],
        CARD_FOREIGN_FUNDING,
        "Foreign Funding",
        "Commercial property values increase by 15%.",
        EVENT_EFFECT_ASSET_VALUE_CHANGE,
        TARGET_COMMERCIAL_PROPERTIES,
        EVENT_VALUE_PERCENTAGE,
        15,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_PORT_EXPANSION],
        CARD_PORT_EXPANSION,
        "Port Expansion",
        "Railway station values increase by 20%.",
        EVENT_EFFECT_ASSET_VALUE_CHANGE,
        TARGET_ALL_RAILWAYS,
        EVENT_VALUE_PERCENTAGE,
        20,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_FESTIVAL_SEASON],
        CARD_FESTIVAL_SEASON,
        "Festival Season",
        "Hotels receive 50% additional rent.",
        EVENT_EFFECT_RENT_CHANGE,
        TARGET_ALL_HOTELS,
        EVENT_VALUE_PERCENTAGE,
        50,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_LABOUR_STRIKE],
        CARD_LABOUR_STRIKE,
        "Labour Strike",
        "Construction is suspended for 2 rounds.",
        EVENT_EFFECT_CONSTRUCTION_SUSPENSION,
        TARGET_CONSTRUCTION_SYSTEM,
        EVENT_VALUE_NONE,
        0,
        2,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_INSURANCE_DISCOUNT],
        CARD_INSURANCE_DISCOUNT,
        "Insurance Discount",
        "Insurance premiums are reduced by 20%.",
        EVENT_EFFECT_INSURANCE_PREMIUM_CHANGE,
        TARGET_INSURANCE_PREMIUMS,
        EVENT_VALUE_PERCENTAGE,
        -20,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_PROPERTY_REVALUATION],
        CARD_PROPERTY_REVALUATION,
        "Property Revaluation",
        "A random property group appreciates by 15%.",
        EVENT_EFFECT_ASSET_VALUE_CHANGE,
        TARGET_RANDOM_PROPERTY_GROUP,
        EVENT_VALUE_PERCENTAGE,
        15,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_CURRENCY_DEPRECIATION],
        CARD_CURRENCY_DEPRECIATION,
        "Currency Depreciation",
        "Construction costs increase by 10%.",
        EVENT_EFFECT_CONSTRUCTION_COST_CHANGE,
        TARGET_CONSTRUCTION_SYSTEM,
        EVENT_VALUE_PERCENTAGE,
        10,
        15,
        EVENT_TEMPORARY
    );

    setEventCard(
        &cards[CARD_GOVERNMENT_GRANT],
        CARD_GOVERNMENT_GRANT,
        "Government Grant",
        "A random player receives LKR 5,000.",
        EVENT_EFFECT_CASH_CHANGE,
        TARGET_RANDOM_PLAYER,
        EVENT_VALUE_LKR,
        5000,
        0,
        EVENT_IMMEDIATE
    );

    setEventCard(
        &cards[CARD_NATIONAL_DISASTER],
        CARD_NATIONAL_DISASTER,
        "National Disaster",
        "A random developed property is damaged.",
        EVENT_EFFECT_PROPERTY_DAMAGE,
        TARGET_RANDOM_DEVELOPED_PROPERTY,
        EVENT_VALUE_NONE,
        0,
        0,
        EVENT_IMMEDIATE
    );
}

//Initializes the event deck
void initializeEventDeck(EventDeck *deck){

    for (int i = 0; i < CARD_COUNT; i++)
    {
        deck->order[i] = (EventCardType)i;
    }

    deck->topIndex = 0;
}

//Shuffles the event deck
void shuffleEventDeck(EventDeck *deck){

    for (int i = CARD_COUNT - 1; i > 0; i--)
    {
        int randomIndex = rand() % (i + 1);

        EventCardType temporary = deck->order[i];

        deck->order[i] = deck->order[randomIndex];

        deck->order[randomIndex] = temporary;
    }

    deck->topIndex = 0;
}

//draws an event card from the deck
const EventCard *drawEventCard(EventDeck *deck, const EventCard cards[CARD_COUNT]){

    if (deck == NULL || cards == NULL)
    {
        return NULL;
    }

    EventCardType drawnCardID =
        deck->order[deck->topIndex];

    deck->topIndex =
        (deck->topIndex + 1) % CARD_COUNT;

    return &cards[drawnCardID];
}

//prints the event card effects and duration if it has one
void printEventCard(const EventCard *card){

    if (card == NULL)
    {
        printf("No event card was drawn.\n");
        return;
    }

    printf("\n====================================\n");
    printf("National Event Card\n");
    printf("====================================\n");

    printf("Card: %s\n", card->name);
    printf("Effect: %s\n", card->description);

    if (card->timing == EVENT_TEMPORARY)
    {
        printf("Duration: %d rounds\n", card->duration);
    }

    printf("====================================\n");
}

