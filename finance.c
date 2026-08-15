#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "types.h"
#include "game.h"
#include "finance.h"

enum{

    /* Rule-LK 19 : bidding begins at 50% of market value. */
    AUCTION_OPENING_PERCENTAGE = 50,

    /* Rule-LK 20 : minimum bid increment LKR 250. */
    AUCTION_BID_INCREMENT = 250,

    /* The bank returns half the money spent on a building. */
    BUILDING_SALE_PERCENTAGE = 50,

    /* The bank buys an asset back at half its market value. */
    ASSET_SALE_PERCENTAGE = 50
};

enum{

    /* Section 3.1 : bids until 120% of estimated market value. */
    AGGRESSIVE_BID_PERCENTAGE = 120,

    /* Section 3.2 : participates only when bidding below market value. */
    CONSERVATIVE_BID_PERCENTAGE = 90,

    /* Section 3.4 : prefers discounted auction purchases. */
    OPPORTUNISTIC_BID_PERCENTAGE = 80,

    /* The opportunistic trader keeps this much cash untouched. */
    OPPORTUNISTIC_AUCTION_RESERVE = 6000
};

static bool isValidPlayerID(int playerID){

    return playerID >= 0 && playerID < NUM_PLAYERS;
}

enum{
 
    /* Rule-LK 2 : 75% of the total mortgage value. */
    LOAN_COLLATERAL_PERCENTAGE = 75,
 
    /* Rule-LK 4 : loans start with a 20 round duration. */
    LOAN_DURATION_ROUNDS = 20,
 
    /* Table 9 : the rate used before the economy is modelled. */
    STABLE_ECONOMY_RATE = 8
};

enum{
 
    /* Cash levels at which each strategy decides to borrow. */
    AGGRESSIVE_LOAN_TRIGGER = 10000,
    CONSERVATIVE_LOAN_TRIGGER = 2000,
    OPPORTUNISTIC_LOAN_TRIGGER = 8000,
 
    /* The opportunistic trader ignores loans below this size. */
    OPPORTUNISTIC_MIN_LOAN = 3000,
 
    /* Cash kept spare before it clears a loan. */
    OPPORTUNISTIC_LOAN_RESERVE = 5000
};

/*
 * ============================================================
 * ASSET ACCESS LAYER
 * ============================================================
 */

/*
 * Reports whether this square can be owned by a player.
 * Only properties, railways and utilities can be owned.
 */
bool isOwnableSquare(const Square *square){

    if (square == NULL)
    {
        return false;
    }

    return square->Types == Square_Property ||
           square->Types == Square_Railway ||
           square->Types == Square_Utility;
}

/*
 * Returns the owner of any ownable square.
 * OWNER_BANK is returned when the square cannot be owned,
 * so the caller never has to look inside the union.
 */
OwnerID getAssetOwner(const Square *square){

    if (!isOwnableSquare(square))
    {
        return OWNER_BANK;
    }

    switch (square->Types)
    {
        case Square_Property:
            return square->Data.property.ownerID;

        case Square_Railway:
            return square->Data.railway.ownerID;

        case Square_Utility:
            return square->Data.utility.ownerID;

        default:
            return OWNER_BANK;
    }
}

/*
 * Returns the current market value of any ownable square.
 * Market value is used instead of purchase price because
 * inflation, booms and declines keep changing it.
 */
int getAssetMarketValue(const Square *square){

    if (!isOwnableSquare(square))
    {
        return 0;
    }

    switch (square->Types)
    {
        case Square_Property:
            return square->Data.property.current_market_value;

        case Square_Railway:
            return square->Data.railway.current_market_value;

        case Square_Utility:
            return square->Data.utility.current_market_value;

        default:
            return 0;
    }
}

/*
 * Returns the mortgage value of any ownable square.
 * This is the amount the bank pays when the asset
 * is handed over as security.
 */
int getAssetMortgageValue(const Square *square){

    if (!isOwnableSquare(square))
    {
        return 0;
    }

    switch (square->Types)
    {
        case Square_Property:
            return square->Data.property.mortgage_value;

        case Square_Railway:
            return square->Data.railway.mortgage_value;

        case Square_Utility:
            return square->Data.utility.mortgage_value;

        default:
            return 0;
    }
}

/*
 * Reports whether the asset is already mortgaged.
 * A mortgaged asset earns no rent and cannot be
 * mortgaged a second time.
 */
bool isAssetMortgaged(const Square *square){

    if (!isOwnableSquare(square))
    {
        return false;
    }

    switch (square->Types)
    {
        case Square_Property:
            return square->Data.property.isMortgaged;

        case Square_Railway:
            return square->Data.railway.isMortgaged != 0;

        case Square_Utility:
            return square->Data.utility.isMortgaged;

        default:
            return false;
    }
}

/*
 * Reports whether the asset is pledged against a loan.
 * Rule-LK 3 states that a loan locked asset cannot be
 * sold, traded, auctioned or mortgaged again.
 */
bool isAssetLoanLocked(const Square *square){

    if (!isOwnableSquare(square))
    {
        return false;
    }

    switch (square->Types)
    {
        case Square_Property:
            return square->Data.property.isLoanLocked;

        case Square_Railway:
            return square->Data.railway.isLoanLocked != 0;

        case Square_Utility:
            return square->Data.utility.isLoanLocked;

        default:
            return false;
    }
}

/*
 * Reports whether the asset carries any building.
 * Railways and utilities can never be developed,
 * so they always report false.
 */
bool assetHasBuildings(const Square *square){

    if (square == NULL || square->Types != Square_Property)
    {
        return false;
    }

    return square->Data.property.hasHotel ||
           square->Data.property.houses_count > 0;
}

/*
 * Changes the owner of any ownable square.
 * This only writes the ownership field. The player
 * counters are updated by the two functions below.
 */
void setAssetOwner(Square *square, OwnerID newOwner){

    if (!isOwnableSquare(square))
    {
        return;
    }

    switch (square->Types)
    {
        case Square_Property:
            square->Data.property.ownerID = newOwner;
            break;

        case Square_Railway:
            square->Data.railway.ownerID = newOwner;
            break;

        case Square_Utility:
            square->Data.utility.ownerID = newOwner;
            break;

        default:
            break;
    }
}

/*
 * Sets or clears the mortgage flag of any ownable square.
 */
void setAssetMortgaged(Square *square, bool mortgaged){

    if (!isOwnableSquare(square))
    {
        return;
    }

    switch (square->Types)
    {
        case Square_Property:
            square->Data.property.isMortgaged = mortgaged;
            break;

        case Square_Railway:
            square->Data.railway.isMortgaged = mortgaged ? 1 : 0;
            break;

        case Square_Utility:
            square->Data.utility.isMortgaged = mortgaged;
            break;

        default:
            break;
    }
}

/*
 * Records that a player has gained an asset.
 * The ownedAssets table and the three ownership
 * counters are always updated together so they
 * can never disagree with each other.
 */
void registerAssetOwnership(Player *player, const Square *square, int squareIndex){

    if (player == NULL ||
        !isOwnableSquare(square) ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        player->ownedAssets[squareIndex])
    {
        return;
    }

    player->ownedAssets[squareIndex] = true;

    switch (square->Types)
    {
        case Square_Property:
            player->totalPropertiesOwned++;
            break;

        case Square_Railway:
            player->railwayOwned++;
            break;

        case Square_Utility:
            player->UtilitiesOwned++;
            break;

        default:
            break;
    }
}

/*
 * Records that a player has lost an asset.
 * This is the exact opposite of the function above.
 */
void releaseAssetOwnership(Player *player, const Square *square, int squareIndex){

    if (player == NULL ||
        !isOwnableSquare(square) ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        !player->ownedAssets[squareIndex])
    {
        return;
    }

    player->ownedAssets[squareIndex] = false;

    switch (square->Types)
    {
        case Square_Property:

            if (player->totalPropertiesOwned > 0)
            {
                player->totalPropertiesOwned--;
            }

            break;

        case Square_Railway:

            if (player->railwayOwned > 0)
            {
                player->railwayOwned--;
            }

            break;

        case Square_Utility:

            if (player->UtilitiesOwned > 0)
            {
                player->UtilitiesOwned--;
            }

            break;

        default:
            break;
    }
}

/*
 * ============================================================
 * VALUATION
 * ============================================================
 */

/*
 * Returns the total money invested in the buildings
 * standing on one property.
 *
 * A hotel replaced four houses, so the hotel price
 * already represents the whole development.
 */
int calculateBuildingValue(const Property *property){

    if (property == NULL)
    {
        return 0;
    }

    if (property->hasHotel)
    {
        return property->hotel_price;
    }

    if (property->houses_count > 0 && property->houses_count <= 4)
    {
        return property->houses_count * property->house_price;
    }

    return 0;
}

/*
 * Returns the cash the bank pays when a player is forced
 * to sell an asset back.
 *
 * The bank pays half the market value. When the asset is
 * already mortgaged the mortgage money was collected
 * earlier, so that amount is deducted here.
 */
int calculateAssetSaleValue(const Square *square){

    if (!isOwnableSquare(square))
    {
        return 0;
    }

    int saleValue = (getAssetMarketValue(square) * ASSET_SALE_PERCENTAGE) / 100;

    if (isAssetMortgaged(square))
    {
        saleValue -= getAssetMortgageValue(square);
    }

    return saleValue > 0 ? saleValue : 0;
}

/*
 * Calculates the true net worth of a player as defined
 * in Rule 15.
 *
 * Cash + Property Value + Building Value + Railway Value
 * + Utility Value - Outstanding Loans.
 *
 * A mortgaged asset counts only for the part that is not
 * owed back to the bank, because the mortgage money is
 * already sitting inside the cash balance.
 *
 * This function recalculates everything from the board,
 * so the answer can never drift away from reality.
 */
int calculatePlayerNetWorth(const Player *player, const Square board[NUM_SQUARES]){

    if (player == NULL || board == NULL)
    {
        return 0;
    }

    int netWorth = player->cash_balance;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }

        if (getAssetOwner(&board[i]) != player->ID)
        {
            continue;
        }

        int assetValue = getAssetMarketValue(&board[i]);

        if (isAssetMortgaged(&board[i]))
        {
            assetValue -= getAssetMortgageValue(&board[i]);
        }

        if (assetValue > 0)
        {
            netWorth += assetValue;
        }

        if (board[i].Types == Square_Property)
        {
            netWorth += calculateBuildingValue(&board[i].Data.property);
        }
    }

    /*
     * Rule-LK 4 adds the accumulated interest into the
     * outstanding loan, so subtracting the outstanding
     * amount already removes the interest as well.
     */
    netWorth -= player->loan.loanOutstanding;

    return netWorth;
}

/*
 * Calculates how much cash the player could raise if every
 * legal recovery action were taken.
 *
 * Cash
 * + half the value of every building
 * + the best result obtainable from every free asset
 *
 * An unmortgaged asset can first be mortgaged and then sold,
 * so the combined result is simply the larger of the two
 * amounts. Writing it this way makes double counting
 * impossible.
 *
 * Loan locked assets are excluded because Rule-LK 3 forbids
 * selling or mortgaging them.
 */
int calculateRealizableValue(const Player *player, const Square board[NUM_SQUARES]){

    if (player == NULL || board == NULL)
    {
        return 0;
    }

    int total = player->cash_balance;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }

        if (getAssetOwner(&board[i]) != player->ID)
        {
            continue;
        }

        if (board[i].Types == Square_Property)
        {
            total += (calculateBuildingValue(&board[i].Data.property) * BUILDING_SALE_PERCENTAGE) / 100;
        }

        if (isAssetLoanLocked(&board[i]))
        {
            continue;
        }

        int saleValue = calculateAssetSaleValue(&board[i]);

        if (isAssetMortgaged(&board[i]))
        {
            total += saleValue;
        }
        else
        {
            int mortgageValue = getAssetMortgageValue(&board[i]);

            total += mortgageValue > saleValue ? mortgageValue : saleValue;
        }
    }

    return total;
}

/*
 * ============================================================
 * LIQUIDATION
 * ============================================================
 */

/*
 * Chooses which building should be sold next.
 *
 * Rule 9 forces buildings to stay even inside a colour group,
 * so the building must always come off the property that is
 * currently the most developed one in its own group.
 *
 * Among equally developed properties the cheapest building is
 * removed first, so the expensive developments survive longer.
 *
 * Returns the board index, or -1 when nothing can be sold.
 */
int findBuildingToSell(const Player *player, const Square board[NUM_SQUARES]){

    if (player == NULL || board == NULL)
    {
        return -1;
    }

    int chosenIndex = -1;
    int chosenLevel = -1;
    int chosenPrice = INT_MAX;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] ||
            board[i].Types != Square_Property ||
            board[i].Data.property.ownerID != player->ID)
        {
            continue;
        }

        const Property *property = &board[i].Data.property;

        if (!property->hasHotel && property->houses_count <= 0)
        {
            continue;
        }

        int level = property->hasHotel ? 5 : property->houses_count;

        int price = property->hasHotel ? property->hotel_price : property->house_price;

        /*
         * Confirm that no other property of the same group is
         * developed further. Selling here first keeps the
         * group even.
         */
        bool isHighestInGroup = true;

        for (int j = 0; j < NUM_SQUARES; j++)
        {
            if (j == i || board[j].Types != Square_Property)
            {
                continue;
            }

            if (board[j].Data.property.group != property->group)
            {
                continue;
            }

            int otherLevel = board[j].Data.property.hasHotel ? 5 : board[j].Data.property.houses_count;

            if (otherLevel > level)
            {
                isHighestInGroup = false;

                break;
            }
        }

        if (!isHighestInGroup)
        {
            continue;
        }

        if (level > chosenLevel || (level == chosenLevel && price < chosenPrice))
        {
            chosenIndex = i;
            chosenLevel = level;
            chosenPrice = price;
        }
    }

    return chosenIndex;
}

/*
 * Shared selection rule used by mortgaging and by selling.
 *
 * The smallest asset that still covers the whole shortfall is
 * preferred, because that clears the debt in one action and
 * keeps every other asset in the player's hands.
 *
 * When no single asset is large enough, the largest available
 * asset is taken instead, so the shortfall closes in as few
 * steps as possible.
 *
 * forMortgage selects between the two recovery methods.
 */
static int findAssetForLiquidation(const Player *player, const Square board[NUM_SQUARES], int shortfall, bool forMortgage){

    if (player == NULL || board == NULL || shortfall <= 0)
    {
        return -1;
    }

    int bestCoveringIndex = -1;
    int bestCoveringValue = INT_MAX;

    int bestPartialIndex = -1;
    int bestPartialValue = 0;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }

        if (getAssetOwner(&board[i]) != player->ID)
        {
            continue;
        }

        /* Rule-LK 3 : pledged assets cannot be released. */
        if (isAssetLoanLocked(&board[i]))
        {
            continue;
        }

        /*
         * Buildings must always be cleared before the land
         * underneath them leaves the player's control.
         */
        if (assetHasBuildings(&board[i]))
        {
            continue;
        }

        int value;

        if (forMortgage)
        {
            if (isAssetMortgaged(&board[i]))
            {
                continue;
            }

            value = getAssetMortgageValue(&board[i]);
        }
        else
        {
            value = calculateAssetSaleValue(&board[i]);
        }

        if (value <= 0)
        {
            continue;
        }

        if (value >= shortfall)
        {
            if (value < bestCoveringValue)
            {
                bestCoveringIndex = i;
                bestCoveringValue = value;
            }
        }
        else
        {
            if (value > bestPartialValue)
            {
                bestPartialIndex = i;
                bestPartialValue = value;
            }
        }
    }

    return bestCoveringIndex >= 0 ? bestCoveringIndex : bestPartialIndex;
}

/*
 * Chooses the next asset to mortgage.
 */
int findAssetToMortgage(const Player *player, const Square board[NUM_SQUARES], int shortfall){

    return findAssetForLiquidation(player, board, shortfall, true);
}

void setAssetLoanLocked(Square *square, bool locked){
 
    if (!isOwnableSquare(square))
    {
        return;
    }
 
    switch (square->Types)
    {
        case Square_Property:
            square->Data.property.isLoanLocked = locked;
            break;
 
        case Square_Railway:
            square->Data.railway.isLoanLocked = locked ? 1 : 0;
            break;
 
        case Square_Utility:
            square->Data.utility.isLoanLocked = locked;
            break;
 
        default:
            break;
    }
}

/*
 * Chooses the next asset to sell back to the bank.
 */
int findAssetToSell(const Player *player, const Square board[NUM_SQUARES], int shortfall){

    return findAssetForLiquidation(player, board, shortfall, false);
}

/*
 * Removes exactly one building from a property and pays the
 * player half of what that building cost.
 *
 * A hotel is not destroyed outright. It becomes the four
 * houses it originally replaced, which matches Rule 10 and
 * lets the player raise money in smaller steps.
 *
 * Returns true when a building was actually removed.
 */
bool sellOneBuilding(Player *player, Square board[NUM_SQUARES], int propertyIndex){

    if (player == NULL ||
        board == NULL ||
        propertyIndex < 0 ||
        propertyIndex >= NUM_SQUARES ||
        board[propertyIndex].Types != Square_Property ||
        board[propertyIndex].Data.property.ownerID != player->ID)
    {
        return false;
    }

    Property *property = &board[propertyIndex].Data.property;

    int refund;

    if (property->hasHotel)
    {
        refund = (property->hotel_price * BUILDING_SALE_PERCENTAGE) / 100;

        property->hasHotel = false;

        property->houses_count = 4;

        printf(
            "%s sold the hotel on %s for LKR %d.\n",
            getPlayerName(player->ID),
            board[propertyIndex].name,
            refund
        );
    }
    else if (property->houses_count > 0)
    {
        refund = (property->house_price * BUILDING_SALE_PERCENTAGE) / 100;

        property->houses_count--;

        printf(
            "%s sold one house on %s for LKR %d.\n",
            getPlayerName(player->ID),
            board[propertyIndex].name,
            refund
        );
    }
    else
    {
        return false;
    }

    player->cash_balance += refund;

    return true;
}

/*
 * Mortgages one asset and pays the mortgage value in cash.
 *
 * A mortgaged asset stays with the player but stops earning
 * rent, which is exactly what Rule 7 requires.
 */
bool mortgageAsset(Player *player, Square board[NUM_SQUARES], int squareIndex){

    if (player == NULL ||
        board == NULL ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        !isOwnableSquare(&board[squareIndex]) ||
        getAssetOwner(&board[squareIndex]) != player->ID ||
        isAssetMortgaged(&board[squareIndex]) ||
        isAssetLoanLocked(&board[squareIndex]) ||
        assetHasBuildings(&board[squareIndex]))
    {
        return false;
    }

    int mortgageValue = getAssetMortgageValue(&board[squareIndex]);

    if (mortgageValue <= 0)
    {
        return false;
    }

    setAssetMortgaged(&board[squareIndex], true);

    player->cash_balance += mortgageValue;

    printf(
        "%s mortgaged %s and received LKR %d.\n",
        getPlayerName(player->ID),
        board[squareIndex].name,
        mortgageValue
    );

    return true;
}

/*
 * Sells one asset back to the bank.
 *
 * Ownership returns to the bank, every mortgage and insurance
 * record on it is cleared, and the asset becomes available for
 * purchase or auction again.
 */
bool sellAssetToBank(Player *player, Square board[NUM_SQUARES], int squareIndex){

    if (player == NULL ||
        board == NULL ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        !isOwnableSquare(&board[squareIndex]) ||
        getAssetOwner(&board[squareIndex]) != player->ID ||
        isAssetLoanLocked(&board[squareIndex]) ||
        assetHasBuildings(&board[squareIndex]))
    {
        return false;
    }

    int saleValue = calculateAssetSaleValue(&board[squareIndex]);

    setAssetOwner(&board[squareIndex], OWNER_BANK);

    setAssetMortgaged(&board[squareIndex], false);

    if (board[squareIndex].Types == Square_Property)
    {
        board[squareIndex].Data.property.insurance = INSURANCE_NONE;

        board[squareIndex].Data.property.insuranceExpireRounds = 0;
    }

    releaseAssetOwnership(player, &board[squareIndex], squareIndex);

    player->cash_balance += saleValue;

    printf(
        "%s sold %s to the Bank for LKR %d.\n",
        getPlayerName(player->ID),
        board[squareIndex].name,
        saleValue
    );

    return true;
}

/*
 * ============================================================
 * DEBT RECOVERY AND BANKRUPTCY
 * ============================================================
 */

/*
 * Handles the moment a player owes money that the cash
 * balance cannot cover.
 *
 * The recovery order is always the same:
 *
 *   1. pay directly when the cash is already there
 *   2. give up immediately when even total liquidation fails
 *   3. sell buildings
 *   4. mortgage assets
 *   5. sell assets back to the bank
 *
 * Buildings are sacrificed first because they are the least
 * permanent asset. Land is only released at the very end.
 *
 * creditor may be NULL. That means the money is owed to the
 * bank, for example a tax payment.
 *
 * Returns PAYMENT_SUCCESS, PLAYER_BANKRUPT or PAYMENT_INVALID.
 */
PaymentResult resolveDebt(Player *debtor, Player *creditor, int debtAmount, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){

    /* Step 1 : validate every input. */
    if (debtor == NULL ||
        players == NULL ||
        board == NULL ||
        gameState == NULL ||
        debtor->isBankrupt ||
        debtAmount <= 0 ||
        !isValidPlayerID(debtor->ID) ||
        debtor == creditor)
    {
        return PAYMENT_INVALID;
    }

    if (creditor != NULL && (creditor->isBankrupt || !isValidPlayerID(creditor->ID)))
    {
        return PAYMENT_INVALID;
    }

    /* Step 2 : the cash is already available. */
    if (debtor->cash_balance >= debtAmount)
    {
        debtor->cash_balance -= debtAmount;

        if (creditor != NULL)
        {
            creditor->cash_balance += debtAmount;
        }

        return PAYMENT_SUCCESS;
    }

    printf(
        "%s owes LKR %d but holds only LKR %d.\n",
        getPlayerName(debtor->ID),
        debtAmount,
        debtor->cash_balance
    );

    /*
     * Steps 3 and 4 : if everything the player owns is still
     * not enough, there is no reason to break the portfolio
     * apart first. Bankruptcy is declared straight away.
     */
    int realizableValue = calculateRealizableValue(debtor, board);

    if (realizableValue < debtAmount)
    {
        printf(
            "%s can raise only LKR %d in total.\n",
            getPlayerName(debtor->ID),
            realizableValue
        );

        declareBankrupt(debtor, creditor, players, board, gameState);

        return PLAYER_BANKRUPT;
    }

    printf(
        "%s begins raising funds.\n",
        getPlayerName(debtor->ID)
    );

    /* Step 5 : sell buildings until the debt is covered. */
    while (debtor->cash_balance < debtAmount)
    {
        int buildingIndex = findBuildingToSell(debtor, board);

        if (buildingIndex < 0)
        {
            break;
        }

        if (!sellOneBuilding(debtor, board, buildingIndex))
        {
            break;
        }
    }

    /* Step 6 : mortgage assets until the debt is covered. */
    while (debtor->cash_balance < debtAmount)
    {
        int shortfall = debtAmount - debtor->cash_balance;

        int assetIndex = findAssetToMortgage(debtor, board, shortfall);

        if (assetIndex < 0)
        {
            break;
        }

        if (!mortgageAsset(debtor, board, assetIndex))
        {
            break;
        }
    }

    /* Step 7 : sell assets back to the bank. */
    while (debtor->cash_balance < debtAmount)
    {
        int shortfall = debtAmount - debtor->cash_balance;

        int assetIndex = findAssetToSell(debtor, board, shortfall);

        if (assetIndex < 0)
        {
            break;
        }

        if (!sellAssetToBank(debtor, board, assetIndex))
        {
            break;
        }
    }

    /* Step 8 : the money was raised successfully. */
    if (debtor->cash_balance >= debtAmount)
    {
        debtor->cash_balance -= debtAmount;

        if (creditor != NULL)
        {
            creditor->cash_balance += debtAmount;

            printf(
                "%s paid LKR %d to %s.\n",
                getPlayerName(debtor->ID),
                debtAmount,
                getPlayerName(creditor->ID)
            );
        }
        else
        {
            printf(
                "%s paid LKR %d to the Bank.\n",
                getPlayerName(debtor->ID),
                debtAmount
            );
        }

        return PAYMENT_SUCCESS;
    }

    /* Step 9 : liquidation stalled before the debt was met. */
    declareBankrupt(debtor, creditor, players, board, gameState);

    return PLAYER_BANKRUPT;
}

/*
 * Removes a player from the game under Rule 14 and Rule-LK 7.
 *
 * Whatever cash is left goes to the creditor, since that
 * player was owed money first. Every asset then returns to
 * the bank with its buildings demolished and its insurance
 * cancelled, and each one is auctioned as Section 2.6
 * requires.
 *
 * creditor may be NULL when the debt was owed to the bank.
 */
void declareBankrupt(Player *bankruptPlayer, Player *creditor, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){

    if (bankruptPlayer == NULL ||
        players == NULL ||
        board == NULL ||
        gameState == NULL ||
        bankruptPlayer->isBankrupt)
    {
        return;
    }

    printf(
        "\n%s has been declared bankrupt.\n",
        getPlayerName(bankruptPlayer->ID)
    );

    /*
     * The flag is raised before anything else so that the
     * bankrupt player can never join the auctions that
     * follow, and so that a second bankruptcy can never be
     * declared for the same player.
     */
    bankruptPlayer->isBankrupt = true;

    /* Remaining cash settles as much of the debt as it can. */
    if (bankruptPlayer->cash_balance > 0 && creditor != NULL && !creditor->isBankrupt)
    {
        creditor->cash_balance += bankruptPlayer->cash_balance;

        printf(
            "LKR %d in remaining cash transferred to %s.\n",
            bankruptPlayer->cash_balance,
            getPlayerName(creditor->ID)
        );
    }

    bankruptPlayer->cash_balance = 0;

    /* Rule 14 : the loan is cleared with the player. */
    bankruptPlayer->loan.loanAmount = 0;
    bankruptPlayer->loan.loanOutstanding = 0;
    bankruptPlayer->loan.roundsRemaining = 0;
    bankruptPlayer->loan.interestRate = 0;
    bankruptPlayer->loan.status = LOAN_NONE;

    /*
     * Collect the assets before auctioning them, because the
     * ownership table is cleared during the transfer.
     */
    int recoveredAssets[NUM_SQUARES];
    int recoveredCount = 0;

    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!bankruptPlayer->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }

        if (board[i].Types == Square_Property)
        {
            Property *property = &board[i].Data.property;

            /* Rule 14 : all buildings are removed. */
            property->houses_count = 0;
            property->hasHotel = false;

            /* Rule 14 : insurance policies expire. */
            property->insurance = INSURANCE_NONE;
            property->insuranceExpireRounds = 0;

            property->isDamaged = false;
            property->isClosed = false;
            property->conditionofproperty = 100;
            property->isLoanLocked = false;
        }
        else if (board[i].Types == Square_Railway)
        {
            board[i].Data.railway.isLoanLocked = 0;
        }
        else if (board[i].Types == Square_Utility)
        {
            board[i].Data.utility.isLoanLocked = false;
        }

        setAssetOwner(&board[i], OWNER_BANK);

        setAssetMortgaged(&board[i], false);

        releaseAssetOwnership(bankruptPlayer, &board[i], i);

        recoveredAssets[recoveredCount] = i;

        recoveredCount++;
    }

    bankruptPlayer->networth = 0;

    printf("Remaining assets transferred to the Bank.\n");

    if (gameState->number_of_solvent_players > 0)
    {
        gameState->number_of_solvent_players--;
    }

    /*
     * Section 2.6 : the liquidated assets are auctioned.
     * This happens after the player has fully left the game
     * so that the auction sees a clean board.
     */
    for (int i = 0; i < recoveredCount; i++)
    {
        (void)runAuction(board, recoveredAssets[i], players, gameState);
    }
}

/*
 * ============================================================
 * AUCTIONS
 * ============================================================
 */

/*
 * Returns the highest amount a player is willing to bid for
 * one asset.
 *
 * Every strategy has its own ceiling taken from Section 3,
 * and Rule-LK 22 caps all of them at the cash actually held
 * because loans cannot be taken during an auction.
 */
int getAuctionBidLimit(const Player *bidder, const Square board[NUM_SQUARES], int squareIndex){

    if (bidder == NULL ||
        board == NULL ||
        bidder->isBankrupt ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        !isOwnableSquare(&board[squareIndex]))
    {
        return 0;
    }

    int marketValue = getAssetMarketValue(&board[squareIndex]);

    if (marketValue <= 0)
    {
        return 0;
    }

    int limit;

    switch (bidder->strategy)
    {
        case STRATEGY_AGGRESSIVE:

            /*
             * Bids up to 120% of market value.
             */
            limit = (marketValue * AGGRESSIVE_BID_PERCENTAGE) / 100;

            break;

        case STRATEGY_CONSERVATIVE:

            /*
             * Only bids below market value and never spends
             * more than half of the cash in hand.
             */
            limit = (marketValue * CONSERVATIVE_BID_PERCENTAGE) / 100;

            if (limit > bidder->cash_balance / 2)
            {
                limit = bidder->cash_balance / 2;
            }

            break;

        case STRATEGY_RISK_TAKER:

            /*
             * Bids until the available cash is exhausted.
             */
            limit = bidder->cash_balance;

            break;

        case STRATEGY_OPPORTUNISTIC:

            /*
             * Buys only at a clear discount and always keeps
             * an emergency reserve untouched.
             */
            limit = (marketValue * OPPORTUNISTIC_BID_PERCENTAGE) / 100;

            if (limit > bidder->cash_balance - OPPORTUNISTIC_AUCTION_RESERVE)
            {
                limit = bidder->cash_balance - OPPORTUNISTIC_AUCTION_RESERVE;
            }

            break;

        default:
            limit = 0;

            break;
    }

    /* Rule-LK 22 : nobody may bid more cash than they hold. */
    if (limit > bidder->cash_balance)
    {
        limit = bidder->cash_balance;
    }

    return limit > 0 ? limit : 0;
}

/*
 * Runs a complete auction for one bank owned asset.
 *
 * Rule-LK 19 : every solvent player takes part and bidding
 *              opens at 50% of market value.
 * Rule-LK 20 : each new bid rises by at least LKR 250.
 * Rule-LK 21 : a player who declines is out permanently and
 *              the last remaining bidder wins.
 * Rule-LK 22 : nobody bids more cash than they hold.
 * Rule-LK 23 : if nobody bids the asset stays with the bank.
 *
 * The loop always ends, because every pass either raises the
 * price by a fixed step or removes at least one bidder.
 *
 * Returns true when the asset found a new owner.
 */
bool runAuction(Square board[NUM_SQUARES], int squareIndex, Player players[NUM_PLAYERS], GameState *gameState){

    if (board == NULL ||
        players == NULL ||
        gameState == NULL ||
        squareIndex < 0 ||
        squareIndex >= NUM_SQUARES ||
        !isOwnableSquare(&board[squareIndex]) ||
        getAssetOwner(&board[squareIndex]) != OWNER_BANK ||
        isAssetLoanLocked(&board[squareIndex]))
    {
        return false;
    }

    int marketValue = getAssetMarketValue(&board[squareIndex]);

    if (marketValue <= 0)
    {
        return false;
    }

    int openingBid = (marketValue * AUCTION_OPENING_PERCENTAGE) / 100;

    printf("\nAuction Started.\n");

    printf("Property : %s\n", board[squareIndex].name);

    printf("Opening Bid : LKR %d.\n", openingBid);

    bool isActive[NUM_PLAYERS];

    int activeCount = 0;

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        isActive[i] = !players[i].isBankrupt;

        if (isActive[i])
        {
            activeCount++;
        }
    }

    int highestBid = 0;
    int highestBidder = -1;

    while (activeCount > 1 || (activeCount == 1 && highestBidder < 0))
    {
        bool bidPlacedThisPass = false;

        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            if (!isActive[i] || i == highestBidder)
            {
                continue;
            }

            int requiredBid = highestBidder < 0 ? openingBid : highestBid + AUCTION_BID_INCREMENT;

            int bidLimit = getAuctionBidLimit(&players[i], board, squareIndex);

            if (bidLimit >= requiredBid && players[i].cash_balance >= requiredBid)
            {
                highestBid = requiredBid;

                highestBidder = i;

                bidPlacedThisPass = true;

                printf(
                    "%s bids LKR %d.\n",
                    getPlayerName(players[i].ID),
                    requiredBid
                );
            }
            else
            {
                isActive[i] = false;

                activeCount--;

                printf(
                    "%s withdraws.\n",
                    getPlayerName(players[i].ID)
                );
            }
        }

        /*
         * Nobody moved during a complete pass, so the price
         * can never rise again.
         */
        if (!bidPlacedThisPass)
        {
            break;
        }
    }

    /* Rule-LK 23 : an auction with no bidder changes nothing. */
    if (highestBidder < 0)
    {
        printf(
            "No player placed a bid. %s remains with the Bank.\n",
            board[squareIndex].name
        );

        return false;
    }

    Player *winner = &players[highestBidder];

    winner->cash_balance -= highestBid;

    setAssetOwner(&board[squareIndex], (OwnerID)winner->ID);

    setAssetMortgaged(&board[squareIndex], false);

    registerAssetOwnership(winner, &board[squareIndex], squareIndex);

    printf(
        "%s wins the auction for %s at LKR %d.\n",
        getPlayerName(winner->ID),
        board[squareIndex].name,
        highestBid
    );

    printf(
        "Remaining cash balance: LKR %d.\n",
        winner->cash_balance
    );

    return true;
}

// Returns the interest rate the bank is charging right now.

int getCurrentInterestRate(const GameState *gameState){
 
    if (gameState == NULL || gameState->current_interest <= 0)
    {
        return STABLE_ECONOMY_RATE;
    }
 
    return gameState->current_interest;
}

// Returns the largest loan the bank will approve.

int calculateMaxLoanAmount(const Player *player, const Square board[NUM_SQUARES]){
 
    if (player == NULL || board == NULL || player->isBankrupt)
    {
        return 0;
    }
 
    int collateralValue = 0;
 
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }
 
        if (getAssetOwner(&board[i]) != player->ID)
        {
            continue;
        }
 
        if (isAssetMortgaged(&board[i]) || isAssetLoanLocked(&board[i]))
        {
            continue;
        }
 
        collateralValue += getAssetMortgageValue(&board[i]);
    }
 
    return (collateralValue * LOAN_COLLATERAL_PERCENTAGE) / 100;
}

//  Marks assets as Loan Locked until enough collateral has been pledged to secure the requested amount.
 
static int lockCollateral(Player *player, Square board[NUM_SQUARES], int loanAmount){
 
    if (player == NULL || board == NULL || loanAmount <= 0)
    {
        return 0;
    }
 
    int securedValue = 0;
    int lockedCount = 0;
 
    printf("Collateral :\n");
 
    /*
     * Each pass picks the cheapest asset still available,
     * so the pledged set stays as small as possible.
     */
    while ((securedValue * LOAN_COLLATERAL_PERCENTAGE) / 100 < loanAmount)
    {
        int chosenIndex = -1;
        int chosenValue = INT_MAX;
 
        for (int i = 0; i < NUM_SQUARES; i++)
        {
            if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
            {
                continue;
            }
 
            if (getAssetOwner(&board[i]) != player->ID)
            {
                continue;
            }
 
            if (isAssetMortgaged(&board[i]) || isAssetLoanLocked(&board[i]))
            {
                continue;
            }
 
            int mortgageValue = getAssetMortgageValue(&board[i]);
 
            if (mortgageValue > 0 && mortgageValue < chosenValue)
            {
                chosenIndex = i;
                chosenValue = mortgageValue;
            }
        }
 
        if (chosenIndex < 0)
        {
            break;
        }
 
        setAssetLoanLocked(&board[chosenIndex], true);
 
        securedValue += chosenValue;
 
        lockedCount++;
 
        printf("%s\n", board[chosenIndex].name);
    }
 
    return lockedCount;
}

// Frees every asset this player had pledged. Called when a loan is fully repaid.

static void releaseCollateral(Player *player, Square board[NUM_SQUARES]){
 
    if (player == NULL || board == NULL)
    {
        return;
    }
 
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!player->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }
 
        if (getAssetOwner(&board[i]) != player->ID)
        {
            continue;
        }
 
        setAssetLoanLocked(&board[i], false);
    }
}

// Creates a new secured loan.

bool takeLoan(Player *player, Square board[NUM_SQUARES], GameState *gameState, int requestedAmount){
 
    if (player == NULL ||
        board == NULL ||
        gameState == NULL ||
        player->isBankrupt ||
        player->loan.status == LOAN_HAVE ||
        requestedAmount <= 0)
    {
        return false;
    }
 
    int maximumLoan = calculateMaxLoanAmount(player, board);
 
    if (maximumLoan <= 0)
    {
        printf("%s has no collateral available for a loan.\n", getPlayerName(player->ID));
 
        return false;
    }
 
    /* Rule-LK 2 : the request is capped at the ceiling. */
    if (requestedAmount > maximumLoan)
    {
        requestedAmount = maximumLoan;
    }
 
    printf("%s obtained a secured loan.\n", getPlayerName(player->ID));
 
    printf("Loan Amount : LKR %d\n", requestedAmount);
 
    int lockedCount = lockCollateral(player, board, requestedAmount);
 
    if (lockedCount <= 0)
    {
        printf("No collateral could be pledged. Loan cancelled.\n");
 
        return false;
    }
 
    player->loan.loanAmount = requestedAmount;
    player->loan.loanOutstanding = requestedAmount;
    player->loan.startround = gameState->current_round;
    player->loan.interestRate = getCurrentInterestRate(gameState);
    player->loan.roundsRemaining = LOAN_DURATION_ROUNDS;
    player->loan.status = LOAN_HAVE;
 
    player->cash_balance += requestedAmount;
 
    printf("Interest Rate : %d%%\n", player->loan.interestRate);
 
    printf("Duration : %d Rounds\n", player->loan.roundsRemaining);
 
    return true;
}

// Repays a part of the loan

bool repayLoan(Player *player, Square board[NUM_SQUARES], int repaymentAmount){
 
    if (player == NULL ||
        board == NULL ||
        player->isBankrupt ||
        player->loan.status != LOAN_HAVE ||
        repaymentAmount <= 0)
    {
        return false;
    }
 
    /* Never pay more than is owed. */
    if (repaymentAmount > player->loan.loanOutstanding)
    {
        repaymentAmount = player->loan.loanOutstanding;
    }
 
    if (player->cash_balance < repaymentAmount)
    {
        return false;
    }
 
    player->cash_balance -= repaymentAmount;
 
    player->loan.loanOutstanding -= repaymentAmount;
 
    printf("%s repaid LKR %d.\n", getPlayerName(player->ID), repaymentAmount);
 
    if (player->loan.loanOutstanding <= 0)
    {
        player->loan.loanOutstanding = 0;
        player->loan.loanAmount = 0;
        player->loan.roundsRemaining = 0;
        player->loan.interestRate = 0;
        player->loan.status = LOAN_PAID;
 
        releaseCollateral(player, board);
 
        printf("Loan fully settled. Collateral released.\n");
    }
    else
    {
        printf("Outstanding Balance :\nLKR %d.\n", player->loan.loanOutstanding);
    }
 
    return true;
}

// Extending the loan time period 

bool extendLoanPeriod(Player *player){
 
    if (player == NULL ||
        player->isBankrupt ||
        player->loan.status != LOAN_HAVE)
    {
        return false;
    }
 
    player->loan.roundsRemaining += LOAN_DURATION_ROUNDS;
 
    printf(
        "%s extended the loan period. Rounds remaining : %d\n",
        getPlayerName(player->ID),
        player->loan.roundsRemaining
    );
 
    return true;
}

// Borrows more on top of an existing loan if collateral is still available.

bool increaseLoanAmount(Player *player, Square board[NUM_SQUARES], int additionalAmount){
 
    if (player == NULL ||
        board == NULL ||
        player->isBankrupt ||
        player->loan.status != LOAN_HAVE ||
        additionalAmount <= 0)
    {
        return false;
    }
 
    int availableHeadroom = calculateMaxLoanAmount(player, board);
 
    if (availableHeadroom <= 0)
    {
        return false;
    }
 
    if (additionalAmount > availableHeadroom)
    {
        additionalAmount = availableHeadroom;
    }
 
    printf("%s increased the loan by LKR %d.\n", getPlayerName(player->ID), additionalAmount);
 
    if (lockCollateral(player, board, additionalAmount) <= 0)
    {
        return false;
    }
 
    player->loan.loanAmount += additionalAmount;
    player->loan.loanOutstanding += additionalAmount;
 
    player->cash_balance += additionalAmount;
 
    printf("Outstanding Balance :\nLKR %d.\n", player->loan.loanOutstanding);
 
    return true;
}

// Handles a loan that was not repaid before its duration expired.

bool handleLoanDefault(Player *borrower, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){
 
    if (borrower == NULL ||
        players == NULL ||
        board == NULL ||
        gameState == NULL ||
        borrower->isBankrupt ||
        borrower->loan.status != LOAN_HAVE)
    {
        return false;
    }
 
    printf("\n%s has defaulted.\n", getPlayerName(borrower->ID));
 
    int foreclosedAssets[NUM_SQUARES];
    int foreclosedCount = 0;
 
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!borrower->ownedAssets[i] || !isOwnableSquare(&board[i]))
        {
            continue;
        }
 
        /* Only the pledged assets are taken. */
        if (!isAssetLoanLocked(&board[i]))
        {
            continue;
        }
 
        if (board[i].Types == Square_Property)
        {
            Property *property = &board[i].Data.property;
 
            /* Rule-LK 6 : hotels and houses are demolished. */
            property->houses_count = 0;
            property->hasHotel = false;
 
            /* Rule-LK 6 : insurance on those assets is cancelled. */
            property->insurance = INSURANCE_NONE;
            property->insuranceExpireRounds = 0;
 
            property->isDamaged = false;
            property->isClosed = false;
            property->conditionofproperty = 100;
        }
 
        setAssetLoanLocked(&board[i], false);
 
        setAssetOwner(&board[i], OWNER_BANK);
 
        setAssetMortgaged(&board[i], false);
 
        releaseAssetOwnership(borrower, &board[i], i);
 
        foreclosedAssets[foreclosedCount] = i;
 
        foreclosedCount++;
    }
 
    printf("Collateral has been foreclosed.\n");
 
    /* Rule-LK 6 : the outstanding debt is cleared. */
    borrower->loan.loanAmount = 0;
    borrower->loan.loanOutstanding = 0;
    borrower->loan.roundsRemaining = 0;
    borrower->loan.interestRate = 0;
    borrower->loan.status = LOAN_FORECLOSED;
 
    printf("Outstanding debt cleared.\n");
 
    /*
     * Rule-LK 7 : a player with no assets and no cash left
     * cannot continue.
     */
    if (borrower->totalPropertiesOwned <= 0 &&
        borrower->railwayOwned <= 0 &&
        borrower->UtilitiesOwned <= 0 &&
        borrower->cash_balance <= 0)
    {
        declareBankrupt(borrower, NULL, players, board, gameState);
    }
 
    /* Section 2.6 : foreclosed assets return to the bank and are auctioned. */
    for (int i = 0; i < foreclosedCount; i++)
    {
        (void)runAuction(board, foreclosedAssets[i], players, gameState);
    }
 
    return true;
}

// Updates loan interest and round remaining at the end of each round 

void processLoansEndOfRound(Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){
 
    if (players == NULL || board == NULL || gameState == NULL)
    {
        return;
    }
 
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        Player *player = &players[i];
 
        if (player->isBankrupt || player->loan.status != LOAN_HAVE)
        {
            continue;
        }
 
        /* Interest is compounded on the current balance. */
        int interest = (player->loan.loanOutstanding * player->loan.interestRate) / 100;
 
        player->loan.loanOutstanding += interest;
 
        if (interest > 0)
        {
            printf(
                "%s accrued LKR %d interest. Outstanding : LKR %d\n",
                getPlayerName(player->ID),
                interest,
                player->loan.loanOutstanding
            );
        }
 
        if (player->loan.roundsRemaining > 0)
        {
            player->loan.roundsRemaining--;
        }
 
        /* Rule-LK 6 : time has run out. */
        if (player->loan.roundsRemaining <= 0)
        {
            (void)handleLoanDefault(player, players, board, gameState);
        }
    }
}

// Decides what a players does when landing on the Bank.

BankAction decideBankAction(const Player *player, const Square board[NUM_SQUARES]){
 
    if (player == NULL || board == NULL || player->isBankrupt)
    {
        return BANK_NONE;
    }
 
    bool hasLoan = player->loan.status == LOAN_HAVE;
 
    int maximumLoan = calculateMaxLoanAmount(player, board);
 
    switch (player->strategy)
    {
        case STRATEGY_AGGRESSIVE:
 
            if (hasLoan)
            {
                /* Repays only when cash is twice the debt. */
                if (player->cash_balance > player->loan.loanOutstanding * 2)
                {
                    return BANK_REPAY_FULL;
                }
 
                return BANK_NONE;
            }
 
            if (maximumLoan > 0 && player->cash_balance < AGGRESSIVE_LOAN_TRIGGER)
            {
                return BANK_TAKE_LOAN;
            }
 
            return BANK_NONE;
 
        case STRATEGY_CONSERVATIVE:
 
            if (hasLoan)
            {
                /* Clears the debt as soon as it is affordable. */
                if (player->cash_balance >= player->loan.loanOutstanding)
                {
                    return BANK_REPAY_FULL;
                }
 
                return BANK_REPAY_PART;
            }
 
            /* Borrows only when bankruptcy is imminent. */
            if (maximumLoan > 0 && player->cash_balance < CONSERVATIVE_LOAN_TRIGGER)
            {
                return BANK_TAKE_LOAN;
            }
 
            return BANK_NONE;
 
        case STRATEGY_RISK_TAKER:
 
            if (hasLoan)
            {
                /* Refinances upward whenever collateral allows. */
                if (maximumLoan > 0)
                {
                    return BANK_INCREASE;
                }
 
                return BANK_EXTEND;
            }
 
            if (maximumLoan > 0)
            {
                return BANK_TAKE_LOAN;
            }
 
            return BANK_NONE;
 
        case STRATEGY_OPPORTUNISTIC:
 
            if (hasLoan)
            {
                if (player->cash_balance >= player->loan.loanOutstanding + OPPORTUNISTIC_LOAN_RESERVE)
                {
                    return BANK_REPAY_FULL;
                }
 
                return BANK_NONE;
            }
 
            /* Only borrows when the amount justifies the interest. */
            if (maximumLoan >= OPPORTUNISTIC_MIN_LOAN &&
                player->cash_balance < OPPORTUNISTIC_LOAN_TRIGGER)
            {
                return BANK_TAKE_LOAN;
            }
 
            return BANK_NONE;
 
        default:
 
            return BANK_NONE;
    }
}

// Runs financial transaction when a player lands on Bank.

bool handleBankSquare(Player *player, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState){
 
    if (player == NULL ||
        players == NULL ||
        board == NULL ||
        gameState == NULL ||
        player->isBankrupt)
    {
        return false;
    }
 
    BankAction action = decideBankAction(player, board);
 
    switch (action)
    {
        case BANK_TAKE_LOAN:
 
            return takeLoan(player, board, gameState, calculateMaxLoanAmount(player, board));
 
        case BANK_REPAY_FULL:
 
            return repayLoan(player, board, player->loan.loanOutstanding);
 
        case BANK_REPAY_PART:
 
            /* Pays whatever cash can be spared this visit. */
            return repayLoan(player, board, player->cash_balance / 2);
 
        case BANK_EXTEND:
 
            return extendLoanPeriod(player);
 
        case BANK_INCREASE:
 
            return increaseLoanAmount(player, board, calculateMaxLoanAmount(player, board));
 
        case BANK_NONE:
        default:
 
            printf("%s made no transaction at the Bank.\n", getPlayerName(player->ID));
 
            return false;
    }
}


