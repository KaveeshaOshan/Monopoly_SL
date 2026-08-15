#ifndef FINANCE_H_
#define FINANCE_H_

#include <stdbool.h>
#include "types.h"

/*
 * ============================================================
 * ASSET ACCESS LAYER
 * ------------------------------------------------------------
 * A Square stores either a Property, a Railway or a Utility
 * inside one union. These three structures keep the same
 * information under different names.
 *
 * Every function below hides that union so the rest of the
 * program can treat all three as one single "asset".
 * ============================================================
 */

bool isOwnableSquare(const Square *square);

OwnerID getAssetOwner(const Square *square);

int getAssetMarketValue(const Square *square);

int getAssetMortgageValue(const Square *square);

bool isAssetMortgaged(const Square *square);

bool isAssetLoanLocked(const Square *square);

bool assetHasBuildings(const Square *square);

void setAssetOwner(Square *square, OwnerID newOwner);

void setAssetMortgaged(Square *square, bool mortgaged);

void registerAssetOwnership(Player *player, const Square *square, int squareIndex);

void releaseAssetOwnership(Player *player, const Square *square, int squareIndex);

void setAssetLoanLocked(Square *square, bool locked);

/*
 * ============================================================
 * VALUATION
 * ============================================================
 */

int calculateBuildingValue(const Property *property);

int calculateAssetSaleValue(const Square *square);

int calculatePlayerNetWorth(const Player *player, const Square board[NUM_SQUARES]);

int calculateRealizableValue(const Player *player, const Square board[NUM_SQUARES]);

/*
 * ============================================================
 * LIQUIDATION
 * ============================================================
 */

int findBuildingToSell(const Player *player, const Square board[NUM_SQUARES]);

int findAssetToMortgage(const Player *player, const Square board[NUM_SQUARES], int shortfall);

int findAssetToSell(const Player *player, const Square board[NUM_SQUARES], int shortfall);

bool sellOneBuilding(Player *player, Square board[NUM_SQUARES], int propertyIndex);

bool mortgageAsset(Player *player, Square board[NUM_SQUARES], int squareIndex);

bool sellAssetToBank(Player *player, Square board[NUM_SQUARES], int squareIndex);

/*
 * ============================================================
 * DEBT RECOVERY AND BANKRUPTCY
 * ============================================================
 */

PaymentResult resolveDebt(Player *debtor, Player *creditor, int debtAmount, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState);

void declareBankrupt(Player *bankruptPlayer, Player *creditor, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState);

/*
 * ============================================================
 * AUCTIONS
 * ============================================================
 */

int getAuctionBidLimit(const Player *bidder, const Square board[NUM_SQUARES], int squareIndex);

bool runAuction(Square board[NUM_SQUARES], int squareIndex, Player players[NUM_PLAYERS], GameState *gameState);

/*
 * ============================================================
 * Loans and Banking
 * ============================================================
 */

int getCurrentInterestRate(const GameState *gameState);
 
int calculateMaxLoanAmount(const Player *player, const Square board[NUM_SQUARES]);
 
bool takeLoan(Player *player, Square board[NUM_SQUARES], GameState *gameState, int requestedAmount);
 
bool repayLoan(Player *player, Square board[NUM_SQUARES], int repaymentAmount);
 
bool extendLoanPeriod(Player *player);
 
bool increaseLoanAmount(Player *player, Square board[NUM_SQUARES], int additionalAmount);
 
bool handleLoanDefault(Player *borrower, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState);
 
void processLoansEndOfRound(Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState);
 
BankAction decideBankAction(const Player *player, const Square board[NUM_SQUARES]);
 
bool handleBankSquare(Player *player, Player players[NUM_PLAYERS], Square board[NUM_SQUARES], GameState *gameState);





#endif
