#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include "types.h"

/* ---------- Player identity ---------- */

const char *getPlayerName(int PlayerID);

/* ---------- Dice and turn order ---------- */

DiceOperations get_random_dice_values(void);

void sortTurnResults(TurnResult results[], int start, int end);

void rankPlayerGroup(TurnResult results[], int start, int end);

void determineTurnOrder(int order[NUM_PLAYERS]);

bool player_move(Player *theplayer, int *diceTotal);

bool allPlayersPassedGo(const Player players[NUM_PLAYERS], const int passesAtRoundStart[NUM_PLAYERS]);

/* ---------- Simple cash movement ---------- */

bool payPlayer(Player *payer, Player *receiver, int amount);

bool payBank(Player *payer, int amount);

/* ---------- Properties ---------- */

int calculatePropertyRent(const Property *property);

bool shouldBuyProperty(const Player *player, const Square board[NUM_SQUARES], int propertyIndex);

bool buyProperty(Player *player, Square board[NUM_SQUARES], int propertyIndex);

/* ---------- Jail ---------- */

void handleGotoJail(Player *theplayer);

/* ---------- Monopolies and construction ---------- */

bool hasMonopoly(Player *theplayer, Square board[NUM_SQUARES], PropertyGroup group);

int getDevelopmentLevel(Property *property);

int getLowestGroupDevelopmentLevel(Square gameboard[NUM_SQUARES], PropertyGroup group);

bool isColourGroupBuildable(Player *theplayer, Square gameboard[NUM_SQUARES], PropertyGroup group);

bool canBuildHouse(Player *theplayer, Square gameboard[], int propertyIndex);

bool buildHouse(Player *player, Square gameboard[], int propertyIndex);

bool canBuildHotel(Player *theplayer, Square gameboard[], int propertyIndex);

bool buildHotel(Player *theplayer, Square gameboard[NUM_SQUARES], int propertyIndex);

int findNextDevelopmentIndex(Player *player, Square gameboard[NUM_SQUARES], PropertyGroup group);

bool shouldBuild(Player *theplayer, Square board[NUM_SQUARES], int propertyIndex);

void handleBuildingTurn(Player *player, Square board[NUM_SQUARES]);

/* ---------- Railways ---------- */

bool buyRailway(Player *buyer, Square *railwaySquare);

int calculateRailwayRent(const Player *owner, const Railway *railway);

void handleRailwaySquare(Player *theplayer, int landedSquare, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState);

/* ---------- Utilities ---------- */

bool buyUtility(Player *buyer, Square *utilitySquare);

int calculateUtilityRent(Player *owner, int diceSum);

void handleUtilitySquare(Player *theplayer, int landedSquare, int diceSum, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState);

/* ---------- Taxation ---------- */

bool handleTaxSquare(Player *currentPlayer, Player allPlayers[NUM_PLAYERS], Square gameboard[NUM_SQUARES], GameState *gameState);

/* ---------- Simulation entry point ---------- */

void startgame(void);

#endif
