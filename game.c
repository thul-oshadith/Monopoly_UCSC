#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"

// -------------------------------------------------------------
// FORWARD DECLARATIONS FOR runSimulation
// -------------------------------------------------------------
void init_Board(GameState *game);
void init_Players(GameState *game);
void decideTurnOrder(GameState *game);
void handleLanding(GameState *game, int playerIdx, int diceTotal);
void developProperties(GameState *game, int playerIdx);
void performMaintenance(GameState *game, int playerIdx);
void attemptPurchase(GameState *game, int playerIdx, Square *sq, int purchasePrice, PropertyGroup group);
void processEndRoundLoans(GameState *game);
void processDepreciation(GameState *game);
void processEndRoundInsurance(GameState *game);
void triggerRandomDisaster(GameState *game);
void processDynamicPropertyMarket(GameState *game);
void processInflation(GameState *game);
void processGovernmentRegulations(GameState *game);
void triggerEconomicEvent(GameState *game);
void attemptToUnmortgage(GameState *game, int playerIdx);
int  handleJailTurn(GameState *game, int p, int die1, int die2);
void displayRoundSummary(GameState *game);
int  calculateNetWorth(GameState *game, int playerIdx);
int  getDynamicPurchasePrice(GameState *game, int sqIdx);
int  applyEventValueModifier(GameState *game, int sqIdx, int value);
int  applyDynamicMarketValue(GameState *game, int sqIdx, int value);
int  applyDynamicMarketHouseCost(GameState *game, PropertyGroup group, int cost);
int  applyEventHouseCostModifier(GameState *game, int cost);

// -------------------------------------------------------------
// CURRENCY HELPER
// -------------------------------------------------------------
static void printCurrency(int amount) {
    if (amount < 0) {
        printf("-");
        amount = -amount;
    }
    if (amount < 1000) {
        printf("LKR %d", amount);
    } else if (amount < 1000000) {
        printf("LKR %d,%03d", amount / 1000, amount % 1000);
    } else {
        printf("LKR %d,%03d,%03d", amount / 1000000, (amount / 1000) % 1000, amount % 1000);
    }
}

// -------------------------------------------------------------
// MAIN GAME SIMULATION
// -------------------------------------------------------------
void runSimulation() {
    GameState game;
    game.currentEvent = EVENT_NONE;
    game.currentRound = 0;

    game.currentBoomGroup = NO_GROUP;
    game.currentDeclineGroup = NO_GROUP;
    for (int i = 0; i < 8; i++) {
        game.boomCooldowns[i] = 0;
        game.declineCooldowns[i] = 0;
    }

    game.currentInflationRate = 0.0f;
    game.currentLoanInterestRate = 0.08f;

    init_Board(&game);
    init_Players(&game);

    printf("\nMONOPOLY-LK Simulation\n\n");
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("Player %d : %s\n", i+1, game.players[i].name);
    }
    printf("\nEach player begins with LKR 30,000.\n\n");

    decideTurnOrder(&game);

    int goCount[PLAYER_COUNT] = {0};
    int gameOver = 0;

    while (!gameOver) {

        for (int t = 0; t < PLAYER_COUNT; t++) {
            int p = game.turnOrder[t];

            if (game.players[p].bankrupt) {
                continue;
            }

            attemptToUnmortgage(&game, p);
            performMaintenance(&game, p);

            int die1 = rand() % 6 + 1;
            int die2 = rand() % 6 + 1;
            int diceTotal = die1 + die2;

            if (handleJailTurn(&game, p, die1, die2)) {
                continue;
            }

            int oldPosition = game.players[p].position;
            int newPosition = (oldPosition + diceTotal) % SQUARE_COUNT;

            game.players[p].position = newPosition;

            printf("%s rolled %d.\n", game.players[p].name, diceTotal);
            printf("%s moves from Square %d to Square %d.\n", game.players[p].name, oldPosition, newPosition);

            if (newPosition < oldPosition) {
                game.players[p].cash += 2000;
                goCount[p]++;
                printf("%s passed Go! Collected LKR 2000\n", game.players[p].name);
            }

            handleLanding(&game, p, diceTotal);
            developProperties(&game, p);

            // Check if 3 players are bankrupt — game over!
            int bankruptCount = 0;
            for (int i = 0; i < PLAYER_COUNT; i++) {
                if (game.players[i].bankrupt) bankruptCount++;
            }
            if (bankruptCount >= 3) {
                gameOver = 1;
                break;
            }
        }

        // Calculate minGO across all active players
        int minGO = 99999;
        for (int i = 0; i < PLAYER_COUNT; i++) {
            int idx = game.turnOrder[i];
            if (!game.players[idx].bankrupt && goCount[idx] < minGO) {
                minGO = goCount[idx];
            }
        }

        if (minGO >= 500) {
            gameOver = 1;
            break;
        }

        // If minGO has increased, all active players completed another full lap — new ROUND
        if (minGO > game.currentRound) {
            int roundsPassed = minGO - game.currentRound;

            for (int r = 0; r < roundsPassed; r++) {
                processEndRoundLoans(&game);
                processDepreciation(&game);
                processEndRoundInsurance(&game);

                game.currentRound++;

                if (game.currentRound > 0 && game.currentRound % 10 == 0) {
                    triggerRandomDisaster(&game);
                    processDynamicPropertyMarket(&game);
                    processInflation(&game);
                }

                if (game.currentRound > 0 && game.currentRound % 15 == 0) {
                    triggerEconomicEvent(&game);
                }

                if (game.currentRound > 0 && game.currentRound % 20 == 0) {
                    processGovernmentRegulations(&game);
                }

                displayRoundSummary(&game);
            }
        }
    }

    // Calculate net worth for each player and find the winner
    int netWorths[PLAYER_COUNT] = {0};
    int winnerIdx = -1;
    int highestNetWorth = -1;

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (game.players[i].bankrupt) continue;
        netWorths[i] = calculateNetWorth(&game, i);
        if (netWorths[i] > highestNetWorth) {
            highestNetWorth = netWorths[i];
            winnerIdx = i;
        }
    }

    if (winnerIdx != -1) {
        Player *winner = &game.players[winnerIdx];
        printf("\nGAME OVER\n\n");
        printf("Winner\n\n");
        printf("%s\n\n", winner->name);

        printf("Total Cash\n\n");
        printCurrency(winner->cash);
        printf("\n\n");

        int outstandingLoans = winner->activeLoan.active ? winner->activeLoan.amount : 0;
        int accruedInterest  = winner->activeLoan.active ? (int)(winner->activeLoan.amount * winner->activeLoan.interestRate) : 0;
        int totalDebt = outstandingLoans + accruedInterest;
        int totalPropertyValue = netWorths[winnerIdx] - winner->cash + totalDebt;

        printf("Total Property Value\n\n");
        printCurrency(totalPropertyValue);
        printf("\n\n");

        printf("Outstanding Loans\n\n");
        if (outstandingLoans > 0) {
            printCurrency(outstandingLoans);
        } else {
            printf("None");
        }
        printf("\n\n");

        printf("Net Worth\n\n");
        printCurrency(netWorths[winnerIdx]);
        printf("\n\n");
    }
}

// -------------------------------------------------------------
// DECIDE TURN ORDER
// -------------------------------------------------------------


void rankPlayers(GameState *game, int candidates[], int count, int *resultPos, int isReroll) {

    if (count == 1) {
        game->turnOrder[*resultPos] = candidates[0]; // lock in this player
        (*resultPos)++;                              // move to next chair
        return;
    }
    if (count == 0) return;

    int rolls[PLAYER_COUNT] = {0};

    // Step 1: Roll dice for all candidates
    for (int i = 0; i < count; i++) {
        int playerIdx = candidates[i];
        rolls[i] = (rand() % 6 + 1) + (rand() % 6 + 1);

        if (isReroll) {
            printf("  %s rerolls: %d\n", game->players[playerIdx].name, rolls[i]);
        } else {
            printf("  %s rolls: %d\n", game->players[playerIdx].name, rolls[i]);
        }
    }

    // Step 2: Find unique rolls
    int uniqueRolls[12];
    int uniqueCount = 0;

    // Collect unique rolls
    for (int i = 0; i < count; i++) {
        int found = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (uniqueRolls[j] == rolls[i]) {
                found = 1; break;
            }
        }
        if (!found) {
            uniqueRolls[uniqueCount++] = rolls[i];
        }
    }

    // Sort them from highest to lowest (Bubble Sort)
    for (int i = 0; i < uniqueCount - 1; i++) {
        for (int j = i + 1; j < uniqueCount; j++) {
            if (uniqueRolls[j] > uniqueRolls[i]) {
                int temp = uniqueRolls[i];
                uniqueRolls[i] = uniqueRolls[j];
                uniqueRolls[j] = temp;
            }
        }
    }

    // Step 3: Process groups highest to lowest
    for (int g = 0; g < uniqueCount; g++) {
        int group[PLAYER_COUNT];
        int groupSize = 0;
        
        // Find everyone who rolled this exact number
        for (int i = 0; i < count; i++) {
            if (rolls[i] == uniqueRolls[g]) {
                group[groupSize++] = candidates[i];
            }
        }

        if (groupSize == 1) {
            // No tie! Place them.
            game->turnOrder[*resultPos] = group[0];
            (*resultPos)++;
        } else {
            // Tie! Call this exact function again with just the tied group
            printf("\n  Tie detected! Rerolling...\n");
            rankPlayers(game, group, groupSize, resultPos, 1);
        }
    }
}

void decideTurnOrder(GameState *game) {
    printf("=== Determining Turn Order ===\n");
    
    int allPlayers[PLAYER_COUNT];
    for (int i = 0; i < PLAYER_COUNT; i++) {
        allPlayers[i] = i;
    }

    int resultPos = 0;
    
    rankPlayers(game, allPlayers, PLAYER_COUNT, &resultPos, 0);

    // Save their turn order inside the Player struct for easy access later
    for (int i = 0; i < PLAYER_COUNT; i++) {
        game->players[game->turnOrder[i]].turnOrder = i + 1;
    }

    // Print the final result
    printf("\n=== Final Turn Order ===\n");
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("  %d. %s\n", i + 1, game->players[game->turnOrder[i]].name);
    }
    printf("\n");
}


// Forward declarations for functions in finance.c and helper.c
void attemptPurchase(GameState *game, int playerIdx, Square *sq, int purchasePrice, PropertyGroup group);
void handleBankSquare(GameState *game, int playerIdx);
void payAmount(GameState *game, int payerIdx, int payeeIdx, int amount);
int applyEventRentModifier(GameState *game, int sqIdx, int rent, int isHotel);
int applyDynamicMarketRent(GameState *game, int sqIdx, int rent);
int getDynamicPurchasePrice(GameState *game, int sqIdx);
int applyEventValueModifier(GameState *game, int sqIdx, int value);

// -------------------------------------------------------------
// Handle Landing
// -------------------------------------------------------------

// Depreciation Constants (shared with economy.c)
#define DEPRECIATION_GRACE_ROUNDS 50
#define DEPRECIATION_INTERVAL 5
#define MAX_DEPRECIATION_PERCENT 30
#define STRUCTURAL_DAMAGE_PENALTY 15
void handleInsuranceSquare(GameState *game, int playerIdx);

void handleLanding(GameState *game, int playerIdx, int diceTotal) {
    int pos = game->players[playerIdx].position;
    Square *sq = &game->board[pos]; 

    printf("  >> %s landed on %s\n", game->players[playerIdx].name, sq->name);

    switch (sq->type) {

        case SQUARE_PROPERTY:{

        Property *prop = &sq->data.property;
        if (prop->owner == -1) {

         int price = getDynamicPurchasePrice(game, pos);
         price = applyEventValueModifier(game, pos, price);
         attemptPurchase(game, playerIdx, sq, price, prop->group);
            
        } 
        else if (prop->owner != playerIdx) {
         // pay rent
            int multiplier = 1;
            if (prop->hotel == 1) multiplier = 10;
            else if (prop->houses == 1) multiplier = 2;
            else if (prop->houses == 2) multiplier = 3;
            else if (prop->houses == 3) multiplier = 5;
            else if (prop->houses == 4) multiplier = 7;
            int effectiveRent = prop->rent * multiplier;
            effectiveRent = applyEventRentModifier(game, sq->index, effectiveRent, prop->hotel);
            effectiveRent = applyDynamicMarketRent(game, sq->index, effectiveRent);

            // 1. Apply Property Depreciation
            if (prop->propertyAge > DEPRECIATION_GRACE_ROUNDS) {
                int depPercent = (prop->propertyAge - DEPRECIATION_GRACE_ROUNDS) / DEPRECIATION_INTERVAL;
                if (depPercent > MAX_DEPRECIATION_PERCENT) depPercent = MAX_DEPRECIATION_PERCENT; // Max 30%
                int remainingPercent = 100 - depPercent;
                effectiveRent = effectiveRent * remainingPercent / 100;
            }

            // 2. Apply Building Condition
            if (prop->houses > 0 || prop->hotel > 0) {
                if (prop->buildingCondition >= 75 && prop->buildingCondition <= 89) {
                    effectiveRent = effectiveRent * 90 / 100;
                } else if (prop->buildingCondition >= 50 && prop->buildingCondition <= 74) {
                    effectiveRent = effectiveRent * 75 / 100;
                } else if (prop->buildingCondition >= 25 && prop->buildingCondition <= 49) {
                    effectiveRent = effectiveRent * 50 / 100;
                } else if (prop->buildingCondition < 25) {
                    effectiveRent = 0; // Building closed
                }
            }

            // 3. Apply Structural Damage Penalty
            if (prop->hasStructuralDamage) {
                int remainingPercent = 100 - (STRUCTURAL_DAMAGE_PENALTY + 10); // -25% for rent
                effectiveRent = effectiveRent * remainingPercent / 100;
            }

            // 4. Apply Disaster Damage
            if (prop->isDisasterDamaged) {
                effectiveRent = 0; // Rent is 0 until repaired
            }

            if (effectiveRent > 0) {
                printf("  >> %s pay LKR %d rent to %s\n\n", 
                    game->players[playerIdx].name, effectiveRent, game->players[prop->owner].name);
                payAmount(game, playerIdx, prop->owner, effectiveRent);
            } else {
                printf("  >> %s pays no rent because the building is closed due to poor condition!\n\n", game->players[playerIdx].name);
            }
        }
        else {
            // Player owns it — check if they want to renovate the land
            printf("  >> %s owns this property.\n\n", game->players[playerIdx].name);
            if (prop->propertyAge > DEPRECIATION_GRACE_ROUNDS) {
                int depPercent = (prop->propertyAge - DEPRECIATION_GRACE_ROUNDS) / DEPRECIATION_INTERVAL;
                if (depPercent > MAX_DEPRECIATION_PERCENT) depPercent = MAX_DEPRECIATION_PERCENT;

                int renovateDecision = 0;
                char *pname = game->players[playerIdx].name;

                if (strcmp(pname, "Opportunistic Trader") == 0 && depPercent > 15) renovateDecision = 1;
                else if (strcmp(pname, "Conservative Banker") == 0 && depPercent > 10) renovateDecision = 1;
                else if (depPercent >= MAX_DEPRECIATION_PERCENT) renovateDecision = 1; // Risk Taker / Aggressive Investor wait till max

                if (renovateDecision) {
                    int remainingPercent = 100 - depPercent;
                    int marketValue = prop->purchasePrice * remainingPercent / 100;
                    if (prop->hasStructuralDamage) marketValue = marketValue * (100 - STRUCTURAL_DAMAGE_PENALTY) / 100;
                    
                    int cost = marketValue * 10 / 100; // 10% of current market value
                    if (game->players[playerIdx].cash >= cost) {
                        game->players[playerIdx].cash -= cost;
                        prop->propertyAge = 0;
                        printf("  >>> %s renovated the property %s for LKR %d, restoring its value!\n\n", pname, prop->name, cost);
                    }
                }
            }
        }

        break;
        }
    
        case SQUARE_RAILWAY:{
            int owner = sq->data.railway.owner;
            if (owner == -1)
            {
            
                int price = getDynamicPurchasePrice(game, pos);
                price = applyEventValueModifier(game, pos, price);
                attemptPurchase(game, playerIdx, sq, price, NO_GROUP);
            }
            else if (owner == playerIdx)
            {
                printf(" >> %s owns this railway.\n\n", game->players[playerIdx].name);
            }
            else if (sq->data.railway.mortgaged)
            {
                printf(" >>%s is mortgaged. No rent paid.\n\n",sq->name);
            }
            else
            {
                // count how many railways the owner has
                int stationCount = 0;
                for (int i = 0; i < SQUARE_COUNT; i++) 
                {
                    if (game->board[i].type == SQUARE_RAILWAY && game->board[i].data.railway.owner == owner) 
                    {
                        stationCount++;
                    }
                }

                int rent = 0;
                if (stationCount == 1) rent = 250;
                if (stationCount == 2) rent = 500;
                if (stationCount == 3) rent = 1000;
                if (stationCount == 4) rent = 2000;
                
                if (game->currentRegulation == REG_RAILWAY_MODERNIZATION) {
                    rent = (int)(rent * 1.25f);
                }
                
                rent = applyEventRentModifier(game, sq->index, rent, 0);

                printf("  >> %s pay LKR %d rent to %s (owns %d stations)\n\n", 
                    game->players[playerIdx].name, rent, game->players[owner].name, stationCount);
                payAmount(game, playerIdx, owner, rent);
            }
            break;
        }

        case SQUARE_UTILITY:{
            int owner = sq->data.utility.owner;
            if (owner == -1)
            {
                
                int price = getDynamicPurchasePrice(game, pos);
                price = applyEventValueModifier(game, pos, price);
                attemptPurchase(game, playerIdx, sq, price, NO_GROUP);
            }
            else if (owner == playerIdx)
            {
                printf(" >> %s owns this utility.\n\n", game->players[playerIdx].name);
            }
            else if (sq->data.utility.mortgaged)
            {
                printf(" >> %s is mortgaged. No rent paid.\n\n", sq->name);
            }
            else
            {
                int utilityCount = 0;
                for(int i = 0; i < SQUARE_COUNT; i++)
                {
                    if (game->board[i].type == SQUARE_UTILITY && game->board[i].data.utility.owner ==owner)
                    {
                        utilityCount++;
                    }
                }

                int rent = 0;
                if(utilityCount == 1)
                {
                    rent = diceTotal *4;
                }
                else if(utilityCount == 2)
                {
                    rent = diceTotal *10;
                }
                
                if (game->currentRegulation == REG_ELECTRICITY_TARIFF_REVISION) {
                    rent = (int)(rent * 1.20f);
                }
                
                rent = applyEventRentModifier(game, sq->index, rent, 0);
                printf("  >> %s pay LKR %d rent to %s (owns %d utilities. Rolled %d)\n\n", 
                    game->players[playerIdx].name, rent, game->players[owner].name, utilityCount, diceTotal);
                payAmount(game, playerIdx, owner, rent);
                
            }
            break;
        }

        case SQUARE_TAX:{
            // Charge 15% of the player's current cash in hand
            int tax = (int)(game->players[playerIdx].cash * 0.15f);
            
            if (game->currentRegulation == REG_INCREASE_PROPERTY_TAX) {
                tax = (int)(tax * 1.50f);
            }
            
            printf("  >> %s pays LKR %d in Income Tax\n\n", game->players[playerIdx].name, tax);
            payAmount(game, playerIdx, -1, tax); // -1 means they pay the bank!
            break;
        }

        case SQUARE_SPECIAL:{
            
            if (pos == 30){
              game->players[playerIdx].position = 10;
              game->players[playerIdx].inJail = 1;
              game->players[playerIdx].jailTurns = 0;
              printf(" >> %s was sent to JAIL! \n\n", game->players[playerIdx].name);

            }
        
            break;
        }

        case SQUARE_EVENT:
            // TODO
            break;

        case SQUARE_BANK:
            printf("  >> %s visited the Bank.\n", game->players[playerIdx].name);
            handleBankSquare(game, playerIdx);
            break;

        case SQUARE_INSURANCE:
            handleInsuranceSquare(game, playerIdx);
            break;

        case SQUARE_JAIL:
            // Just visiting — nothing happens
            printf("  >> Just visiting jail.\n\n");
            break;

        case SQUARE_GO:
            // Already handled in main.c (pass GO check)
            break;
    }
}

// Handles a player's turn if they are in jail.
int handleJailTurn(GameState *game, int p, int die1, int die2) {
    if (!game->players[p].inJail) return 0;
    
    printf("%s is in the JAIL (Turn %d). They rolled %d and %d.\n", game->players[p].name,
    game->players[p].jailTurns + 1, die1, die2);

    if(die1 == die2){
        printf(" >> Doubles! %s escapes Jail \n", game->players[p].name);
        game->players[p].inJail = 0;
        game->players[p].jailTurns = 0;
        return 0; // Escaped, proceed with turn
    }
    else if (game->players[p].cash >= 300)
    {
        printf(" >> No Doubles, but %s pays LKR 300 bail to escape.\n", game->players[p].name);
        game->players[p].cash -= 300;
        game->players[p].inJail = 0;
        game->players[p].jailTurns = 0;
        return 0; // Escaped, proceed with turn
    }
    else
    {
        game->players[p].jailTurns++;
        if (game->players[p].jailTurns >= 3)
        {
            printf(" >> %s has served 3 turns. Forced release!\n", game->players[p].name);
            game->players[p].inJail = 0;
            game->players[p].jailTurns = 0;
            return 0; // Escaped, proceed with turn
        }
        else
        {
            printf(" >> %s stays in Jail.\n", game->players[p].name);
            return 1; // Did not escape, skip rest of turn
        }
    }

}

int calculateNetWorth(GameState *game, int playerIdx);

void displayRoundSummary(GameState *game) {
    printf("\n------------------------------------------------------\n");
    printf("Round %d Summary\n", game->currentRound);
    printf("------------------------------------------------------\n");

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (game->players[i].bankrupt) continue;

        int properties = 0;
        int hotels = 0;

        for (int j = 0; j < SQUARE_COUNT; j++) {
            if (game->board[j].type == SQUARE_PROPERTY && game->board[j].data.property.owner == i) {
                properties++;
                if (game->board[j].data.property.hotel > 0) {
                    hotels += game->board[j].data.property.hotel;
                }
            } else if (game->board[j].type == SQUARE_RAILWAY && game->board[j].data.railway.owner == i) {
                properties++;
            } else if (game->board[j].type == SQUARE_UTILITY && game->board[j].data.utility.owner == i) {
                properties++;
            }
        }

        printf("\n%s\n", game->players[i].name);
        printf("\nCash : LKR %d\n", game->players[i].cash);
        printf("\nNet Worth : LKR %d\n", calculateNetWorth(game, i));
        printf("\nProperties : %d\n", properties);
        if (hotels > 0) {
            printf("\nHotels : %d\n", hotels);
        }
        
        if (game->players[i].activeLoan.active) {
            printf("\nOutstanding Loan : LKR %d\n", game->players[i].activeLoan.amount);
        } else {
            printf("\nOutstanding Loan : None\n");
        }
        
        printf("\n------------------------------------------------------\n");
    }
}
