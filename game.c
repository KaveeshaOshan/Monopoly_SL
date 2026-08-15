#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "players.h"
#include "types.h"
#include "game.h"
#include "events.h"
#include "board.h"
#include "finance.h"

#include <limits.h>

enum{

    OPPORTUNISTIC_CASH_RESERVE = 6000,
    OPPORTUNISTIC_BUILD_PAYBACKS = 5,
    OPPORTUNISTIC_BUY_PAYBACKS = 10,
    INCOME_TAX_AMOUNT = 2000
};

static bool isValidPlayerID(int playerID){

    return playerID >= 0 && playerID < NUM_PLAYERS;
}

/**static bool isValidPropertyGroup(PropertyGroup group){

    return group > Group_None &&
           group <= Group_Dark_Blue;
}**/

/*
 * DiceOperations and TurnResult moved to types.h so that
 * game.h can declare the functions that use them.
 */

const char *getPlayerName(int PlayerID){

    switch (PlayerID)
    {
        case OWNER_AGGRESSIVE_INVESTOR:
            return "Aggressive Investor";

        case OWNER_CONSERVATIVE_BANKER:

            return "Conservative Banker";

        case OWNER_RISK_TAKER:
            return "Risk Taker";

        case OWNER_OPPORTUNISTIC_TRADER:
            return "Opportunistic Trader";

        default:
            return "Unknown Player";
    }
}

DiceOperations get_random_dice_values(){

    DiceOperations dice;

    dice.dice1 = (rand()%6) + 1;

    dice.dice2 = (rand()%6) + 1;

    dice.sum_of_dice = dice.dice1 + dice.dice2;

    return dice;


}

void sortTurnResults(TurnResult results[], int start, int end){

    for (int i = start; i < end - 1; i++)
    {
        for (int j = start; j < end - 1; j++)
        {
            if (results[j].diceTotal < results[j + 1].diceTotal)
            {
                TurnResult temporary = results[j];
                results[j] = results[j + 1];
                results[j + 1] = temporary;
            }
        }
    }
}

void rankPlayerGroup(TurnResult results[], int start, int end){

    /* Every player in this group rolls */
    for (int i = start; i < end; i++)
    {
        DiceOperations dice = get_random_dice_values();

        results[i].diceTotal = dice.sum_of_dice;

        printf(
            "%s rolled %d and %d. Total = %d\n",
            getPlayerName(results[i].playerID),
            dice.dice1,
            dice.dice2,
            dice.sum_of_dice
        );
    }

    /* Arrange this group from highest to lowest */
    sortTurnResults(results, start, end);

    /* Search for equal totals */
    int groupStart = start;

    while (groupStart < end)
    {
        int groupEnd = groupStart + 1;

        while (groupEnd < end && results[groupEnd].diceTotal == results[groupStart].diceTotal)
        {
            groupEnd++;
        }

        int tiedPlayers = groupEnd - groupStart;

        if (tiedPlayers > 1)
        {
            printf("\n%d players are tied with %d.\n", tiedPlayers, results[groupStart].diceTotal);

            printf("Only these tied players reroll.\n\n");

            rankPlayerGroup(results, groupStart, groupEnd);
        }

        groupStart = groupEnd;
    }
}

void determineTurnOrder(int order[NUM_PLAYERS]){

    TurnResult results[NUM_PLAYERS];

    /* Connect each position to one player */
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        results[i].playerID = i;
        results[i].diceTotal = 0;
    }

    printf("Determining Player Turn Order\n");
    printf("=============================\n");

    /*
     * All four players roll first.
     * Ties are handled inside this function.
     */
    rankPlayerGroup(results, 0, 4);

    /* Save the final player IDs */
    for (int i = 0; i < 4; i++)
    {
        order[i] = results[i].playerID;
    }

    printf("\nFinal Turn Order\n");
    printf("================\n");

    for (int i = 0; i < 4; i++)
    {
        printf("%d. %s\n", i + 1, getPlayerName(order[i]));

    }

}

bool player_move(Player *theplayer, int *diceTotal){

    if (theplayer == NULL || diceTotal == NULL)
    {
        return false;
    }

    *diceTotal = 0;

    /*
     * A jailed player does not move while jail turns remain.
     */
    if (theplayer->in_jail)
    {
        if (theplayer->turns_remaining_Injail > 0)
        {
            printf(
                "%s is in jail and cannot move this turn.\n",
                getPlayerName(theplayer->ID)
            );

            theplayer->turns_remaining_Injail--;

            return false;
        }

        theplayer->in_jail = false;

        printf(
            "%s has been released from jail.\n",
            getPlayerName(theplayer->ID)
        );
    }

    DiceOperations dice = get_random_dice_values();

    *diceTotal = dice.sum_of_dice;

    printf(
        "%s rolled %d and %d. Total = %d\n",
        getPlayerName(theplayer->ID),
        dice.dice1,
        dice.dice2,
        dice.sum_of_dice
    );

    int oldPosition = theplayer->current_position;

    int newPosition =
        (oldPosition + dice.sum_of_dice) % NUM_SQUARES;

    printf(
        "%s moved from Square %d to Square %d.\n",
        getPlayerName(theplayer->ID),
        oldPosition,
        newPosition
    );

    /*
     * The player passed or landed on GO.
     */
    if (oldPosition + dice.sum_of_dice >= NUM_SQUARES)
    {
        theplayer->passed_go++;
        theplayer->cash_balance += 2000;
        theplayer->networth += 2000;

        printf(
            "%s passed GO and collected LKR 2000.\n",
            getPlayerName(theplayer->ID)
        );
    }

    theplayer->current_position = (short)newPosition;

    return true;
}

bool allPlayersPassedGo(const Player players[NUM_PLAYERS], const int passesAtRoundStart[NUM_PLAYERS]){
    
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (players[i].isBankrupt)
        {
            continue;
        }

        if (players[i].passed_go <= passesAtRoundStart[i])
        {
            return false;
        }
    }

    return true;
}

bool payPlayer(Player *payer, Player *receiver, int amount){

    if (payer == NULL ||
        receiver == NULL ||
        payer == receiver ||
        payer->isBankrupt ||
        receiver->isBankrupt ||
        amount <= 0 ||
        payer->cash_balance < amount ||
        receiver->cash_balance > INT_MAX - amount)
    {
        return false;
    }

    payer->cash_balance -= amount;
    receiver->cash_balance += amount;

    payer->networth -= amount;
    receiver->networth += amount;

    return true;
}

bool payBank(Player *payer, int amount){

    if (payer == NULL ||
        payer->isBankrupt ||
        amount <= 0 ||
        payer->cash_balance < amount)
    {
        return false;
    }

    payer->cash_balance -= amount;
    payer->networth -= amount;

    return true;
}

int calculatePropertyRent(const Property *property){

    if (property == NULL)
    {
        return 0;
    }

    //  cannot currently produce rent.
    if (property->isMortgaged || property->isClosed || property->isDamaged)
    {
        return 0;
    }

    //Index 5 stores the hotel rent.
    if (property->hasHotel)
    {
        return property->rent_with_buildings[5];
    }

    //Counting the rent if it is a normal house
    if (property->houses_count >= 0 && property->houses_count <= 4)
    {
        return property->rent_with_buildings[property->houses_count];
    }

    /* Invalid building information. */
    return 0;
}

static bool handleRent(Player *tenant, Player *owner, int propertyIndex, Player allPlayers[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){

    if (tenant == NULL ||
        owner == NULL ||
        allPlayers == NULL ||
        board == NULL ||
        tenant == owner ||
        tenant->isBankrupt ||
        owner->isBankrupt ||
        !isValidPlayerID(tenant->ID) ||
        !isValidPlayerID(owner->ID) ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property ||
        board[propertyIndex].Data.property.ownerID != owner->ID)
    {
        return false;
    }

    int rent =
        calculatePropertyRent(
            &board[propertyIndex].Data.property
        );

    if (rent <= 0)
    {
        printf(
            "No rent is collected on %s.\n",
            board[propertyIndex].name
        );

        return true;
    }

    if (payPlayer(tenant, owner, rent))
    {
        printf(
            "%s paid LKR %d rent to %s.\n",
            getPlayerName(tenant->ID),
            rent,
            getPlayerName(owner->ID)
        );

        return true;
    }

    /*
     * The tenant cannot pay from cash alone, so the debt
     * recovery process decides whether assets can cover it
     * or whether the player is finished.
     */
    PaymentResult result =
        resolveDebt(
            tenant,
            owner,
            rent,
            allPlayers,
            board,
            gameState
        );

    return result == PAYMENT_SUCCESS;
}

static bool wouldCompleteColourGroup(const Player *player, const Square board[NUM_SQUARES], int propertyIndex){

    if (player == NULL ||
        board == NULL ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    PropertyGroup targetGroup = board[propertyIndex].Data.property.group;

    bool foundAnotherProperty = false;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (i == propertyIndex)
        {
            continue;
        }

        if (board[i].Types != Square_Property)
        {
            continue;
        }

        if (board[i].Data.property.group != targetGroup)
        {
            continue;
        }

        foundAnotherProperty = true;

        if (board[i].Data.property.ownerID != player->ID)
        {
            return false;
        }
    }

    return foundAnotherProperty;
}

bool shouldBuyProperty(const Player *player, const Square board[NUM_SQUARES], int propertyIndex){

    if (player == NULL ||
        board == NULL ||
        player->isBankrupt ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    const Property *property = &board[propertyIndex].Data.property;

    /*
     * Use current_market_value because economic
     * events may change the property price.
     */
    int currentPrice = property->current_market_value;

    /*
     * The property cannot be purchased if:
     * - the bank does not own it,
     * - it is locked,
     * - the price is invalid, or
     * - the player cannot afford it.
     */
    if (property->ownerID != OWNER_BANK ||
        property->isLoanLocked ||
        currentPrice <= 0 ||
        player->cash_balance < currentPrice)
    {
        return false;
    }

    int remainingCash = player->cash_balance - currentPrice;

    switch (player->strategy)
    {
        case STRATEGY_AGGRESSIVE:

            /*
             * Aggressive player buys whenever possible,
             * but keeps a small amount equal to the base rent.
             */
            return remainingCash >= property->base_rent;

        case STRATEGY_CONSERVATIVE:

            /*
             * Conservative player only buys if at least
             * half of the current cash remains.
             */
            return remainingCash >= player->cash_balance / 2;

        case STRATEGY_RISK_TAKER:

            /*
             * Risk taker buys whenever the player
             * has enough money.
             */
            return true;

        case STRATEGY_OPPORTUNISTIC:
        {
            bool completesGroup =
                wouldCompleteColourGroup(player, board, propertyIndex);

            /*
             * The current price is lower than the
             * property's original purchase price.
             */
            bool discounted = currentPrice < property->property_purchase_price;

            /*
             * Check whether the property provides
             * reasonably good rent compared to its price.
             */
            bool goodRentReturn = (long long)property->base_rent * OPPORTUNISTIC_BUY_PAYBACKS >= currentPrice;

            /*
             * Opportunistic player must keep some cash
             * and must find a special advantage.
             */
            return remainingCash >= OPPORTUNISTIC_CASH_RESERVE && (completesGroup ||
                                                                     discounted ||
                                                                     goodRentReturn);
        }

        default:
            return false;
    }
}

bool buyProperty(Player *player, Square board[NUM_SQUARES], int propertyIndex){

    /*
     * Check pointers, player condition,
     * property index and square type.
     */
    if (player == NULL ||
        board == NULL ||
        player->isBankrupt ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    Property *property = &board[propertyIndex].Data.property;

    /*
     * Use the current market value because economic
     * events may change the property price.
     */
    int price = property->current_market_value;

    /*
     * Check whether the purchase is legal.
     */
    if (property->ownerID != OWNER_BANK ||
        property->isLoanLocked ||
        property->isMortgaged ||
        price <= 0 ||
        player->cash_balance < price ||
        player->ownedAssets[propertyIndex])
    {
        return false;
    }

    /*
     * Perform the complete transaction.
     * These changes happen only after every check passes.
     */

    payBank(player, price);

    property->ownerID = (OwnerID)player->ID;

    player->ownedAssets[propertyIndex] = true;

    player->totalPropertiesOwned++;

    printf(
        "%s bought %s for LKR %d.\n",
        getPlayerName(player->ID),
        board[propertyIndex].name,
        price
    );

    printf(
        "Remaining cash balance: LKR %d.\n",
        player->cash_balance
    );

    return true;
}

void handleGotoJail(Player *theplayer){

    theplayer->current_position = 10;
    theplayer->in_jail = true;
    theplayer->turns_remaining_Injail = 3;

    printf("%s was sent to Jail.\n", getPlayerName(theplayer->ID));
 
}

bool hasMonopoly(Player *theplayer, Square board[NUM_SQUARES], PropertyGroup group){

    if (theplayer == NULL || board == NULL){
        return false;

    }

    bool foundProperty = false;

    for (int i = 0; i < NUM_SQUARES; i++){

        if (board[i].Types != Square_Property){

            continue;

        }

        Property *property = &board[i].Data.property;

        if (property->group != group){

            continue;

        }

        foundProperty = true;

        if (property->ownerID != theplayer->ID){

            return false;

        }
    }

    return foundProperty;
}

int getDevelopmentLevel(Property *property){

    if (property == NULL)
    {
        return -1;
    }

    /*
     * A hotel is level 5.
     * Because the hotel replaces four houses,
     * houses_count must be zero.
     */
    if (property->hasHotel)
    {
        return property->houses_count == 0 ? 5 : -1;
    }

    if (property->houses_count < 0 ||
        property->houses_count > 4)
    {
        return -1;
    }

    return property->houses_count;
}
    
int getLowestGroupDevelopmentLevel(Square gameboard[NUM_SQUARES], PropertyGroup group){

    if (gameboard == NULL){
        return -1;
    }

    bool foundProperty = false;
    int minDevLevel = 6;

    for(int i=0; i<NUM_SQUARES; i++){

        if (gameboard[i].Types != Square_Property || gameboard[i].Data.property.group != group){
            
            continue;

        }

        int level = getDevelopmentLevel(&gameboard[i].Data.property);

        if (level < 0){

            return -1;

        }

        if (level < minDevLevel){

            minDevLevel = level;

        }

        foundProperty = true;
        
    }

    return foundProperty ? minDevLevel : -1;

}

bool isColourGroupBuildable(Player *theplayer, Square gameboard[NUM_SQUARES], PropertyGroup group){

    bool has_monopoly = hasMonopoly(theplayer, gameboard, group);

    if (theplayer == NULL || gameboard == NULL ||theplayer->isBankrupt || !has_monopoly){
        return false;
    }

    for(int i = 0; i<NUM_SQUARES; i++){

        Property *property = &gameboard[i].Data.property;

        if (gameboard[i].Types != Square_Property || property->group != group){
            continue;
        }
        if (getDevelopmentLevel(property) < 0 || property->isMortgaged || property->isDamaged || property->isClosed) {    
            return false;  // must own the full set
        }

    } // false if any property has a hotel

    return true;
}

bool canBuildHouse(Player *theplayer, Square gameboard[], int propertyIndex){

    if (theplayer == NULL ||
        gameboard == NULL ||
        theplayer->isBankrupt ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        gameboard[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    Property *property = &gameboard[propertyIndex].Data.property;

    if (property->ownerID != theplayer->ID ||
        property->hasHotel ||
        property->houses_count < 0 ||
        property->houses_count >= 4 ||
        property->house_price <= 0 ||
        theplayer->cash_balance < property->house_price)
    {
        return false;
    }

    if (!isColourGroupBuildable(theplayer,gameboard, property->group)){

        return false;

    }

    return property->houses_count == getLowestGroupDevelopmentLevel(gameboard, property->group);


}

bool buildHouse(Player *player, Square gameboard[], int propertyIndex){
    
    if(!canBuildHouse(player, gameboard, propertyIndex)){
        
        printf("%s cannot build a house on %s.\n", getPlayerName(player->ID), gameboard[propertyIndex].name);
        
        return false;
        
    }
    
    Property *property = &gameboard[propertyIndex].Data.property;
    
    player->cash_balance -= property->house_price;
    
    property->houses_count++;

    property->conditionofproperty = 100;
    
    printf("%s built a house on %s.\n", getPlayerName(player->ID), gameboard[propertyIndex].name);
    
    return true;

}

bool canBuildHotel(Player *theplayer, Square gameboard[], int propertyIndex){

    if (theplayer == NULL ||
        gameboard == NULL ||
        theplayer->isBankrupt ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        gameboard[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    Property *property = &gameboard[propertyIndex].Data.property;

    if (property->ownerID != theplayer->ID ||
        property->hasHotel ||
        property->houses_count != 4 ||
        property->hotel_price <= 0 ||
        theplayer->cash_balance < property->hotel_price)
    {
        return false;
    }

    if (!isColourGroupBuildable(theplayer, gameboard, property->group)){
        return false;
    }

    return getLowestGroupDevelopmentLevel(gameboard, property->group) == 4;

}

bool buildHotel(Player *theplayer, Square gameboard[NUM_SQUARES], int propertyIndex){

    if (!canBuildHotel(theplayer, gameboard, propertyIndex)){
        return false;
    }

    Property *property = &gameboard[propertyIndex].Data.property;

    theplayer->cash_balance -= property->hotel_price;

    /*
     * The hotel replaces the property's four houses.
     */
    property->houses_count = 0;
    property->hasHotel = true;
    property->conditionofproperty = 100;

    printf("%s built a hotel on %s for LKR %d.\n", getPlayerName(theplayer->ID), gameboard[propertyIndex].name, property->hotel_price);

    return true;
}

int findNextDevelopmentIndex(Player *player, Square gameboard[NUM_SQUARES], PropertyGroup group) {
    
    if (player == NULL || gameboard == NULL || !isColourGroupBuildable(player, gameboard, group)){
        return -1;
    }

    int minimumLevel = getLowestGroupDevelopmentLevel(gameboard, group);

    /*
     * Level 5 means every property already has a hotel.
     */
    if (minimumLevel < 0 || minimumLevel > 4)
    {
        return -1;
    }

    for (int i = 0; i < NUM_SQUARES; i++){
        if (gameboard[i].Types != Square_Property ||
            gameboard[i].Data.property.group != group ||
            getDevelopmentLevel(&gameboard[i].Data.property) != minimumLevel){

            continue;
        }

        if (minimumLevel < 4 && canBuildHouse(player, gameboard, i)){
            return i;
        }

        if (minimumLevel == 4 && canBuildHotel(player, gameboard, i)){
            return i;
        }
    }

    return -1;
}           

static int getNextDevelopmentRent(Property *property){

    int level = getDevelopmentLevel(property);

    if (level >= 0 && level < 4){

        return property->rent_with_buildings[level + 1];
    }

    if (level == 4){

        return property->rent_with_buildings[5];
    }

    return 0;
}

bool shouldBuild(Player *theplayer, Square board[NUM_SQUARES], int propertyIndex){
    
    if (theplayer == NULL ||
        board == NULL ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property)
    {
        return false;
    }

    bool buildingHotel = canBuildHotel(theplayer, board, propertyIndex);

    bool buildingHouse = canBuildHouse(theplayer, board, propertyIndex);

    if (!buildingHotel && !buildingHouse){

        return false;
    }

    Property *property = &board[propertyIndex].Data.property;

    int cost = buildingHotel ? property->hotel_price : property->house_price;

    int cashAfterBuilding = theplayer->cash_balance - cost;

    int currentRent = calculatePropertyRent(property);

    int nextRent = getNextDevelopmentRent(property);

    int rentIncrease = nextRent - currentRent;

    switch (theplayer->strategy)
    {
        case STRATEGY_AGGRESSIVE:

            if(cost < theplayer->cash_balance){
                return true;
            }
            else{
                return false;
            }

        case STRATEGY_CONSERVATIVE:

            /*
             * A hotel is delayed while a loan is active.
             */
            if (buildingHotel &&(theplayer->loan.status == LOAN_HAVE || theplayer->loan.loanOutstanding > 0)){

                return false;
            }

            /*
             * At least half the current cash remains.
             */
            return cashAfterBuilding >= theplayer->cash_balance / 2;

        case STRATEGY_RISK_TAKER:

            /*
             * Build whenever legally affordable,
             * even when no cash remains afterward.
             */
            return cashAfterBuilding >= 0;

        case STRATEGY_OPPORTUNISTIC:

            /*
             * Keep LKR 6000 and require the extra rent
             * to recover the cost within five payments.
             */
            return cashAfterBuilding >= OPPORTUNISTIC_CASH_RESERVE &&
                   rentIncrease > 0 &&
                   (long long)rentIncrease * OPPORTUNISTIC_BUILD_PAYBACKS >= cost;

        default:
            return false;
    }
}

void handleBuildingTurn(Player *player, Square board[NUM_SQUARES]){
    
    if (player == NULL || board == NULL || player->isBankrupt){
        return;
    }

    /*
     * Search the complete board for a property
     * where this player wants to construct.
     */
    for (int propertyIndex = 0; propertyIndex < NUM_SQUARES; propertyIndex++){
        /*
         * Ignore squares that are not properties.
         */
        if (board[propertyIndex].Types != Square_Property)
        {
            continue;
        }

        /*
         * Check whether the player's strategy
         * wants to construct here.
         */
        if (!shouldBuild(player, board, propertyIndex)){
            continue;
        }

        /*
         * Construct a hotel if possible.
         */
        if (canBuildHotel(player, board, propertyIndex)){

            buildHotel(player, board, propertyIndex);

            return;
        }

        /*
         * Otherwise, construct a house if possible.
         */
        if (canBuildHouse(player, board, propertyIndex)){

            buildHouse(player, board, propertyIndex);

            return;
        }
    }
}

static void handlePropertySquare(Player *theplayer, int propertyIndex, Player allPlayers[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){

    if (theplayer == NULL ||
        allPlayers == NULL ||
        board == NULL ||
        theplayer->isBankrupt ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property)
    {
        return;
    }

    Property *property =
        &board[propertyIndex].Data.property;

    /*
     * Case 1: The bank owns the property.
     */
    if (property->ownerID == OWNER_BANK)
    {
        if (shouldBuyProperty(theplayer, board, propertyIndex)){

            if (buyProperty(theplayer, board, propertyIndex)){

                    printf("Purchase Successful.\n");  
            }
        }
        else{

            printf("%s decided not to buy %s.\n", getPlayerName(theplayer->ID), board[propertyIndex].name);

            /* Rule 5 : a declined property goes straight to auction. */
            (void)runAuction(board, propertyIndex, allPlayers, gameState);

        }

        /*
         * The square has been fully dealt with. Without this
         * return the code below would treat a property that
         * is still owned by the bank as an invalid owner.
         */
        return;
    }

    /*
     * Case 2: The current player owns it.
     */
    if (property->ownerID == theplayer->ID)
    {
        printf("%s already owns %s. No rent is collected.\n", getPlayerName(theplayer->ID), board[propertyIndex].name);

        return;
    }

    /*
     * Case 3: Another player owns it.
     */
    int ownerIndex = (int)property->ownerID;

    if (!isValidPlayerID(ownerIndex) || allPlayers[ownerIndex].isBankrupt)
    {
        printf("Invalid owner state on %s.\n", board[propertyIndex].name);

        return;
    }

    (void)handleRent(theplayer, &allPlayers[ownerIndex], propertyIndex, allPlayers, board, gameState);

}

static bool handleAssetRent( Player *tenant, Player *owner, int rent, const char *assetName, Player allPlayers[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){

    if (tenant == NULL ||
        owner == NULL ||
        assetName == NULL ||
        allPlayers == NULL ||
        board == NULL ||
        gameState == NULL ||
        tenant == owner ||
        tenant->isBankrupt ||
        owner->isBankrupt ||
        rent <= 0)
    {
        return false;
    }

    if (payPlayer(tenant, owner, rent))
    {
        printf(
            "%s paid LKR %d rent to %s for %s.\n",
            getPlayerName(tenant->ID),
            rent,
            getPlayerName(owner->ID),
            assetName
        );

        return true;
    }

    /*
     * Railway and utility rent follows exactly the same
     * recovery path as property rent.
     */
    PaymentResult result =
        resolveDebt(
            tenant,
            owner,
            rent,
            allPlayers,
            board,
            gameState
        );

    return result == PAYMENT_SUCCESS;
}

bool buyRailway(Player *buyer, Square *railwaySquare){

    /*
     * Validate the player and square.
     */
    if (buyer == NULL ||
        railwaySquare == NULL ||
        buyer->isBankrupt ||
        railwaySquare->Types != Square_Railway)
    {
        return false;
    }

    Railway *railway =
        &railwaySquare->Data.railway;

    int railwayIndex = railwaySquare->index;
    int cost = railway->current_market_value;

    /*
     * Check whether the purchase is legal.
     */
    if (railwayIndex < 0 ||
        railwayIndex >= NUM_SQUARES ||
        railway->ownerID != OWNER_BANK ||
        railway->isLoanLocked ||
        railway->isMortgaged ||
        cost <= 0 ||
        buyer->cash_balance < cost ||
        buyer->railwayOwned >= NUM_RAILWAYS ||
        buyer->ownedAssets[railwayIndex])
    {
        return false;
    }

    /*
     * Perform the purchase.
     */
    buyer->cash_balance -= cost;

    railway->ownerID =
        (OwnerID)buyer->ID;

    buyer->ownedAssets[railwayIndex] = true;

    buyer->railwayOwned++;

    printf(
        "%s bought %s for LKR %d.\n",
        getPlayerName(buyer->ID),
        railwaySquare->name,
        cost
    );

    printf(
        "Remaining cash balance: LKR %d.\n",
        buyer->cash_balance
    );

    return true;
}

int calculateRailwayRent(const Player *owner, const Railway *railway){

    if (owner == NULL ||
        owner->isBankrupt ||
        owner->railwayOwned < 1 ||
        owner->railwayOwned > NUM_RAILWAYS)
    {
        return 0;
    }

    int railway_count = owner->railwayOwned;
    int rent = railway_count * railway->current_market_rent;

    return rent;

}

void handleRailwaySquare(Player *theplayer, int landedSquare, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState){

    if (theplayer == NULL ||
        allPlayers == NULL ||
        gameboard == NULL ||
        gameState == NULL ||
        theplayer->isBankrupt ||
        landedSquare < 0 ||
        landedSquare >= NUM_SQUARES ||
        gameboard[landedSquare].Types != Square_Railway)
    {
        return;
    }

    Railway *railway =
        &gameboard[landedSquare].Data.railway;

    if (railway->ownerID == OWNER_BANK)
    {
        /* Rule 5 : an unbought railway is auctioned. */
        if (!buyRailway(theplayer, &gameboard[landedSquare]))
        {
            (void)runAuction(gameboard, landedSquare, allPlayers, gameState);
        }

        return;
    }

    if (railway->ownerID == theplayer->ID)
    {
        printf(
            "%s already owns %s. No rent is collected.\n",
            getPlayerName(theplayer->ID),
            gameboard[landedSquare].name
        );

        return;
    }

    int ownerIndex = (int)railway->ownerID;

    if (!isValidPlayerID(ownerIndex))
    {
        printf(
            "Invalid railway owner on %s.\n",
            gameboard[landedSquare].name
        );

        return;
    }

    Player *owner = &allPlayers[ownerIndex];

    if (owner->isBankrupt ||
        railway->isMortgaged)
    {
        printf(
            "No rent is collected on %s.\n",
            gameboard[landedSquare].name
        );

        return;
    }

    int rent =
        calculateRailwayRent(owner, railway);

    (void)handleAssetRent(
        theplayer,
        owner,
        rent,
        gameboard[landedSquare].name,
        allPlayers,
        gameboard,
        gameState
    );
}

bool buyUtility(Player *buyer, Square *utilitySquare){

    /*
     * Validate the player and square.
     */
    if (buyer == NULL ||
        utilitySquare == NULL ||
        buyer->isBankrupt ||
        utilitySquare->Types != Square_Utility)
    {
        return false;
    }

    Utility *utility =
        &utilitySquare->Data.utility;

    int utilityIndex = utilitySquare->index;
    int cost = utility->current_market_value;

    /*
     * Check whether the purchase is legal.
     */
    if (utilityIndex < 0 ||
        utilityIndex >= NUM_SQUARES ||
        utility->ownerID != OWNER_BANK ||
        utility->isLoanLocked ||
        utility->isMortgaged ||
        cost <= 0 ||
        buyer->cash_balance < cost ||
        buyer->UtilitiesOwned >= NUM_UTILITIES ||
        buyer->ownedAssets[utilityIndex])
    {
        return false;
    }

    /*
     * Perform the purchase.
     */
    buyer->cash_balance -= cost;

    utility->ownerID =
        (OwnerID)buyer->ID;

    buyer->ownedAssets[utilityIndex] = true;

    buyer->UtilitiesOwned++;

    printf(
        "%s bought %s for LKR %d.\n",
        getPlayerName(buyer->ID),
        utilitySquare->name,
        cost
    );

    printf(
        "Remaining cash balance: LKR %d.\n",
        buyer->cash_balance
    );

    return true;
}

int calculateUtilityRent(Player *owner, int diceSum){

    if (owner == NULL ||
        owner->isBankrupt ||
        diceSum < 2 ||
        diceSum > 12)
    {
        return 0;
    }

    if (owner->UtilitiesOwned == 1)
    {
        return diceSum * 4;
    }

    if (owner->UtilitiesOwned == 2)
    {
        return diceSum * 10;
    }

    return 0;
}

void handleUtilitySquare(Player *theplayer, int landedSquare, int diceSum, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState){
    
    if (theplayer == NULL ||
        allPlayers == NULL ||
        gameboard == NULL ||
        gameState == NULL ||
        theplayer->isBankrupt ||
        landedSquare < 0 ||
        landedSquare >= NUM_SQUARES ||
        gameboard[landedSquare].Types != Square_Utility)
    {
        return;
    }

    Utility *utility =
        &gameboard[landedSquare].Data.utility;

    if (utility->ownerID == OWNER_BANK)
    {
        /* Rule 5 : an unbought utility is auctioned. */
        if (!buyUtility(theplayer, &gameboard[landedSquare]))
        {
            (void)runAuction(gameboard, landedSquare, allPlayers, gameState);
        }

        return;
    }

    if (utility->ownerID == theplayer->ID)
    {
        printf(
            "%s already owns %s. No rent is collected.\n",
            getPlayerName(theplayer->ID),
            gameboard[landedSquare].name
        );

        return;
    }

    int ownerIndex = (int)utility->ownerID;

    if (!isValidPlayerID(ownerIndex))
    {
        printf(
            "Invalid utility owner on %s.\n",
            gameboard[landedSquare].name
        );

        return;
    }

    Player *owner = &allPlayers[ownerIndex];

    if (owner->isBankrupt ||
        utility->isMortgaged)
    {
        printf(
            "No rent is collected on %s.\n",
            gameboard[landedSquare].name
        );

        return;
    }

    int rent =
        calculateUtilityRent(owner, diceSum);

    (void)handleAssetRent(
        theplayer,
        owner,
        rent,
        gameboard[landedSquare].name,
        allPlayers,
        gameboard,
        gameState
    );
}

bool handleTaxSquare(Player *currentPlayer, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState){

    if (currentPlayer == NULL ||
        allPlayers == NULL ||
        gameboard == NULL ||
        gameState == NULL ||
        currentPlayer->isBankrupt ||
        currentPlayer->current_position < 0 ||
        currentPlayer->current_position >= NUM_SQUARES ||
        gameboard[currentPlayer->current_position].Types != Square_Tax)
    {
        return false;
    }

    /*
     * The creditor is NULL because tax is paid to the bank
     * and not to another player.
     */
    PaymentResult result =
        resolveDebt(
            currentPlayer,
            NULL,
            INCOME_TAX_AMOUNT,
            allPlayers,
            gameboard,
            gameState
        );

    if (result != PAYMENT_SUCCESS)
    {
        return false;
    }

    printf(
        "%s paid LKR %d tax.\n",
        getPlayerName(currentPlayer->ID),
        INCOME_TAX_AMOUNT
    );

    return true;
}

/*
 * ============================================================
 * OUTPUT FORMATTING
 * ============================================================
 */

/*
 * Formats a whole number the way Section 5 prints money,
 * for example 82500 becomes 82,500.
 *
 * Four buffers are rotated so that several calls can appear
 * inside one printf without overwriting each other.
 */
static const char *formatLKR(int amount){

    static char buffers[4][24];
    static int nextBuffer = 0;

    char digits[16];

    char *out = buffers[nextBuffer];

    nextBuffer = (nextBuffer + 1) % 4;

    int value = amount < 0 ? -amount : amount;

    int digitCount = 0;

    do
    {
        digits[digitCount] = (char)('0' + (value % 10));

        digitCount++;

        value /= 10;

    } while (value > 0 && digitCount < 15);

    int position = 0;

    if (amount < 0)
    {
        out[position] = '-';

        position++;
    }

    for (int i = digitCount - 1; i >= 0; i--)
    {
        out[position] = digits[i];

        position++;

        /* A comma is placed after every third digit. */
        if (i > 0 && (i % 3) == 0)
        {
            out[position] = ',';

            position++;
        }
    }

    out[position] = '\0';

    return out;
}

/*
 * Counts the hotels a player currently owns.
 * The end of round summary reports this separately
 * from the property count.
 */
static int countPlayerHotels(const Player *player, const Square board[NUM_SQUARES]){

    if (player == NULL || board == NULL)
    {
        return 0;
    }

    int hotels = 0;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || board[i].Types != Square_Property)
        {
            continue;
        }

        if (board[i].Data.property.hasHotel)
        {
            hotels++;
        }
    }

    return hotels;
}

/*
 * Adds up the market value of every asset a player owns,
 * including the buildings standing on them.
 *
 * The finance accessors hide the union, so railways,
 * utilities and properties are all counted in one loop.
 */
static int calculatePlayerAssetValue(const Player *player, const Square board[NUM_SQUARES]){

    if (player == NULL || board == NULL)
    {
        return 0;
    }

    int total = 0;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }

        total += getAssetMarketValue(&board[i]);

        if (board[i].Types == Square_Property)
        {
            total += calculateBuildingValue(&board[i].Data.property);
        }
    }

    return total;
}

/*
 * Prints the summary that Section 5 requires at the end of
 * every round.
 */
static void printRoundSummary(const Player allPlayers[NUM_PLAYERS], const Square board[NUM_SQUARES], const GameState *gameState){

    if (allPlayers == NULL || board == NULL || gameState == NULL)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("Round %d Summary\n", gameState->current_round);
    printf("=============================================\n");

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        const Player *player = &allPlayers[i];

        printf("\n%s\n", getPlayerName(player->ID));

        if (player->isBankrupt)
        {
            printf("Bankrupt\n");
        }
        else
        {
            printf("Cash : LKR %s\n", formatLKR(player->cash_balance));

            printf("Net Worth : LKR %s\n", formatLKR(calculatePlayerNetWorth(player, board)));

            printf("Properties : %d\n", player->totalPropertiesOwned);

            printf("Hotels : %d\n", countPlayerHotels(player, board));

            if (player->loan.loanOutstanding > 0)
            {
                printf("Outstanding Loan : LKR %s\n", formatLKR(player->loan.loanOutstanding));
            }
            else
            {
                printf("Outstanding Loan : None\n");
            }
        }

        printf("---------------------------------------------\n");
    }
}

/*
 * Finds the solvent player holding the highest net worth.
 * Returns -1 when nobody is left.
 */
static int findWinner(const Player allPlayers[NUM_PLAYERS], const Square board[NUM_SQUARES]){

    if (allPlayers == NULL || board == NULL)
    {
        return -1;
    }

    int winnerIndex = -1;
    int bestNetWorth = 0;

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (allPlayers[i].isBankrupt)
        {
            continue;
        }

        int netWorth = calculatePlayerNetWorth(&allPlayers[i], board);

        if (winnerIndex < 0 || netWorth > bestNetWorth)
        {
            winnerIndex = i;

            bestNetWorth = netWorth;
        }
    }

    return winnerIndex;
}

/*
 * Prints the closing report required by Section 5.
 */
static void printFinalResult(const Player allPlayers[NUM_PLAYERS], const Square board[NUM_SQUARES], const GameState *gameState){

    if (allPlayers == NULL || board == NULL || gameState == NULL)
    {
        return;
    }

    int winnerIndex = findWinner(allPlayers, board);

    printf("\n=============================================\n");
    printf("GAME OVER\n");
    printf("=============================================\n");

    /*
     * The loop leaves current_round one past the last round
     * that was actually played.
     */
    int roundsPlayed = gameState->current_round - 1;

    if (roundsPlayed > gameState->max_rounds)
    {
        roundsPlayed = gameState->max_rounds;
    }

    printf("Rounds Played : %d\n", roundsPlayed);

    if (winnerIndex < 0)
    {
        printf("Every player is bankrupt. There is no winner.\n");
        printf("=============================================\n");

        return;
    }

    const Player *winner = &allPlayers[winnerIndex];

    printf("Winner : %s\n", getPlayerName(winner->ID));

    printf("Total Cash : LKR %s\n", formatLKR(winner->cash_balance));

    printf("Total Property Value : LKR %s\n", formatLKR(calculatePlayerAssetValue(winner, board)));

    if (winner->loan.loanOutstanding > 0)
    {
        printf("Outstanding Loans : LKR %s\n", formatLKR(winner->loan.loanOutstanding));
    }
    else
    {
        printf("Outstanding Loans : None\n");
    }

    printf("Net Worth : LKR %s\n", formatLKR(calculatePlayerNetWorth(winner, board)));

    printf("=============================================\n");
}

/*
 * ============================================================
 * SIMULATION ENGINE
 * ============================================================
 */

/*
 * Prepares everything the simulation needs before the first
 * round begins.
 */
static void setupSimulation(Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], int turnOrder[NUM_PLAYERS], EventCard cards[CARD_COUNT], EventDeck *deck, GameState *gameState){

    gameState->current_round = 1;
    gameState->current_turn = 1;
    gameState->max_rounds = MAX_ROUNDS;

    /*
     * Bankruptcy decreases the solvent count, and Rule 15
     * uses it to decide when the game ends.
     */
    gameState->number_of_players = NUM_PLAYERS;
    gameState->number_of_solvent_players = NUM_PLAYERS;

    printf("MONOPOLY-LK Simulation\n\n");

    initializePlayers(allPlayers);

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        printf("Player %d : %s\n", i + 1, getPlayerName(allPlayers[i].ID));
    }

    printf("\nEach player begins with LKR %s.\n\n", formatLKR(allPlayers[0].cash_balance));

    initialize_board(gameboard);

    determineTurnOrder(turnOrder);

    initializeEventCards(cards);

    initializeEventDeck(deck);

    shuffleEventDeck(deck);
}

/*
 * Rule 15 : the game finishes when the round limit is
 * reached or only one solvent player remains.
 */
static bool isGameOver(const GameState *gameState){

    if (gameState == NULL)
    {
        return true;
    }

    return gameState->number_of_solvent_players <= 1 ||
           gameState->current_round > gameState->max_rounds;
}

/*
 * Carries out the action belonging to the square a player
 * has landed on.
 *
 * Every branch is one line, so adding a new square type
 * means adding one case and one handler.
 */
static void handleLandedSquare(Player *currentPlayer, int diceTotal, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState, EventDeck *deck, const EventCard cards[CARD_COUNT]){

    if (currentPlayer == NULL ||
        allPlayers == NULL ||
        gameboard == NULL ||
        gameState == NULL ||
        deck == NULL ||
        cards == NULL ||
        currentPlayer->current_position < 0 ||
        currentPlayer->current_position >= NUM_SQUARES)
    {
        return;
    }

    int landedSquare = currentPlayer->current_position;

    switch (gameboard[landedSquare].Types)
    {
        case Square_Start:

            break;

        case Square_Property:

            handlePropertySquare(currentPlayer, landedSquare, allPlayers, gameboard, gameState);

            break;

        case Square_Event:
        {
            const EventCard *drawnCard = drawEventCard(deck, cards);

            printEventCard(drawnCard);

            /*
             * Applying the card's actual effect
             * will be implemented later.
             */

            break;
        }

        case Square_Tax:

            (void)handleTaxSquare(currentPlayer, allPlayers, gameboard, gameState);

            break;

        case Square_Railway:

            handleRailwaySquare(currentPlayer, landedSquare, allPlayers, gameboard, gameState);

            break;

        case Square_Utility:

            handleUtilitySquare(currentPlayer, landedSquare, diceTotal, allPlayers, gameboard, gameState);

            break;

        case Square_Jail:

            printf("%s is just visiting Jail.\n", getPlayerName(currentPlayer->ID));

            break;

        case Square_Goto_Jail:

            handleGotoJail(currentPlayer);

            break;

        case Square_Free_Parking:

            printf("%s has landed on Free Parking.\n", getPlayerName(currentPlayer->ID));

            break;

        case Square_Bank:

            (void)handleBankSquare(currentPlayer, allPlayers, gameboard, gameState);

            break;

        case Square_Insurance:

            /* Insurance will be handled here later. */

            break;

        default:

            break;
    }
}

/*
 * Plays one complete turn for a single player.
 *
 * Rule 3 lists the order: resolve penalties, roll, move,
 * resolve the landing action, then build.
 */
static void playSingleTurn(Player *currentPlayer, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState, EventDeck *deck, const EventCard cards[CARD_COUNT]){

    if (currentPlayer == NULL ||
        allPlayers == NULL ||
        gameboard == NULL ||
        gameState == NULL)
    {
        return;
    }

    if (currentPlayer->isBankrupt)
    {
        printf("%s is bankrupt and skips this turn.\n", getPlayerName(currentPlayer->ID));

        return;
    }

    printf("\n--- %s's movement ---\n\n", getPlayerName(currentPlayer->ID));

    int diceTotal = 0;

    /*
     * A jailed player does not move, so the turn ends here.
     */
    if (!player_move(currentPlayer, &diceTotal))
    {
        return;
    }

    printf("%s is now on %s.\n\n", getPlayerName(currentPlayer->ID), gameboard[currentPlayer->current_position].name);

    handleLandedSquare(currentPlayer, diceTotal, allPlayers, gameboard, gameState, deck, cards);

    /*
     * The landing action may have bankrupted the player,
     * so construction is checked again here.
     */
    if (!currentPlayer->isBankrupt)
    {
        handleBuildingTurn(currentPlayer, gameboard);
    }
}

/*
 * Gives every player one turn, following the order that was
 * decided before the game started.
 */
static void playTurnCycle(const int turnOrder[NUM_PLAYERS], Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState, EventDeck *deck, const EventCard cards[CARD_COUNT]){

    if (turnOrder == NULL || allPlayers == NULL || gameState == NULL)
    {
        return;
    }

    printf("\n---------- Turn %d ----------\n", gameState->current_turn);

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        gameState->current_player = turnOrder[i];

        playSingleTurn(&allPlayers[gameState->current_player], allPlayers, gameboard, gameState, deck, cards);

        if (isGameOver(gameState))
        {
            return;
        }
    }

    printf("\nTurn %d completed.\n", gameState->current_turn);

    gameState->current_turn++;
}

/*
 * Runs every turn cycle until all solvent players have
 * passed GO once, which is what completes a round.
 */
static void playRound(const int turnOrder[NUM_PLAYERS], Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState, EventDeck *deck, const EventCard cards[CARD_COUNT]){

    if (turnOrder == NULL || allPlayers == NULL || gameState == NULL)
    {
        return;
    }

    int passesAtRoundStart[NUM_PLAYERS];

    /*
     * Remember how many times each player had passed GO
     * at the beginning of this round.
     */
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        passesAtRoundStart[i] = allPlayers[i].passed_go;
    }

    gameState->current_turn = 1;

    printf("\n====================================\n""ROUND %d STARTED\n""====================================\n", gameState->current_round);

    while (!allPlayersPassedGo(allPlayers, passesAtRoundStart))
    {
        playTurnCycle(turnOrder, allPlayers, gameboard, gameState, deck, cards);

        if (isGameOver(gameState))
        {
            return;
        }
    }
}

/*
 * Everything that happens once, after a complete round.
 *
 * This is the single place where the remaining periodic
 * rules belong:
 *
 *   Rule-LK 4  : loan interest is compounded
 *   Rule-LK 12 : inflation is generated every ten rounds
 *   Rule-LK 15 : every property grows one round older
 *   Rule-LK 25 : building condition drops by 2%
 *   Rule-LK 30 : the property market is reviewed every ten rounds
 *   Rule-LK 36 : active market conditions are displayed
 *   Section 2.5 : a national event every fifteen rounds
 *   Section 2.7 : a government regulation every twenty rounds
 */
static void endOfRoundUpdate(Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState){

    if (allPlayers == NULL || gameboard == NULL || gameState == NULL)
    {
        return;
    }

    processLoansEndOfRound(allPlayers, gameboard, gameState);

    printRoundSummary(allPlayers, gameboard, gameState);

    printf(
        "\n====================================\n"
        "ROUND %d COMPLETED\n"
        "====================================\n\n"
        "*******************************************************\n",
        gameState->current_round
    );
}

/*
 * The simulation entry point.
 *
 * Setup, the round loop, and the closing report. Every
 * detail lives inside the functions above.
 */
void startgame(){

    GameState gameState = {0};

    Player allPlayers[NUM_PLAYERS];
    Square gameboard[NUM_SQUARES];
    int turnOrder[NUM_PLAYERS];
    EventCard cards[CARD_COUNT];
    EventDeck deck;

    setupSimulation(allPlayers, gameboard, turnOrder, cards, &deck, &gameState);

    while (!isGameOver(&gameState))
    {
        playRound(turnOrder, allPlayers, gameboard, &gameState, &deck, cards);

        endOfRoundUpdate(allPlayers, gameboard, &gameState);

        gameState.current_round++;
    }

    printFinalResult(allPlayers, gameboard, &gameState);
}