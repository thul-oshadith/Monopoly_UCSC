#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"

// Depreciation Constants (shared with economy.c)
#define DEPRECIATION_GRACE_ROUNDS 50
#define DEPRECIATION_INTERVAL 5
#define MAX_DEPRECIATION_PERCENT 30
#define STRUCTURAL_DAMAGE_PENALTY 15

// Recursive function to handle rolling and ties
void rankPlayers(GameState *game, int candidates[], int count, int *resultPos, int isReroll) {
    // Base cases to stop recursion
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
    // Kick off the recursive ranking
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

void handleInsuranceSquare(GameState *game, int playerIdx);

void handleLanding(GameState *game, int playerIdx, int diceTotal) {
    int pos = game->players[playerIdx].position;
    Square *sq = &game->board[pos];  // pointer to the square they landed on

    printf("  >> %s landed on %s\n", game->players[playerIdx].name, sq->name);

    switch (sq->type) {

        case SQUARE_PROPERTY:{

        Property *prop = &sq->data.property;
        if (prop->owner == -1) {
         // New AI buying logic
         attemptPurchase(game, playerIdx, sq, prop->purchasePrice, prop->group);
            
        } 
        else if (prop->owner != playerIdx) {
        // Owned by someone else — pay rent
            int multiplier = 1;
            if (prop->hotel == 1) multiplier = 10;
            else if (prop->houses == 1) multiplier = 2;
            else if (prop->houses == 2) multiplier = 3;
            else if (prop->houses == 3) multiplier = 5;
            else if (prop->houses == 4) multiplier = 7;
            int effectiveRent = prop->rent * multiplier;
            effectiveRent = applyEventRentModifier(game, sq->index, effectiveRent, prop->hotel);

            // 1. Apply Property Depreciation (Rules 15-16)
            if (prop->propertyAge > DEPRECIATION_GRACE_ROUNDS) {
                int depPercent = (prop->propertyAge - DEPRECIATION_GRACE_ROUNDS) / DEPRECIATION_INTERVAL;
                if (depPercent > MAX_DEPRECIATION_PERCENT) depPercent = MAX_DEPRECIATION_PERCENT; // Max 30%
                int remainingPercent = 100 - depPercent;
                effectiveRent = effectiveRent * remainingPercent / 100;
            }

            // 2. Apply Building Condition (Rule 26)
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

            // 3. Apply Structural Damage Penalty (Rule 28)
            if (prop->hasStructuralDamage) {
                int remainingPercent = 100 - (STRUCTURAL_DAMAGE_PENALTY + 10); // -25% for rent
                effectiveRent = effectiveRent * remainingPercent / 100;
            }

            // 4. Apply Disaster Damage (Rule-LK 11)
            if (prop->isDisasterDamaged) {
                effectiveRent = 0; // Rent is 0 until repaired
            }

            if (effectiveRent > 0) {
                printf("  >> %s must pay LKR %d rent to %s\n", 
                    game->players[playerIdx].name, effectiveRent, game->players[prop->owner].name);
                payAmount(game, playerIdx, prop->owner, effectiveRent);
            } else {
                printf("  >> %s pays no rent because the building is closed due to poor condition!\n", game->players[playerIdx].name);
            }
        }
        else {
            // Player owns it — check if they want to renovate the land (Rule-LK 17)
            printf("  >> %s owns this property.\n", game->players[playerIdx].name);
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
                        printf("  >>> %s renovated the property %s for LKR %d, restoring its value!\n", pname, prop->name, cost);
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
                // New AI buying logic
                attemptPurchase(game, playerIdx, sq, sq->data.railway.purchasePrice, NO_GROUP);
            }
            else if (owner == playerIdx)
            {
                printf(" >> %s owns this railway.\n", game->players[playerIdx].name);
            }
            else if (sq->data.railway.mortgaged)
            {
                printf(" >>%s is mortgaged. No rent paid.\n",sq->name);
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
                
                rent = applyEventRentModifier(game, sq->index, rent, 0);

                printf("  >> %s must pay LKR %d rent to %s (owns %d stations)\n", 
                    game->players[playerIdx].name, rent, game->players[owner].name, stationCount);
                payAmount(game, playerIdx, owner, rent);
            }
            break;
        }

        case SQUARE_UTILITY:{
            int owner = sq->data.utility.owner;
            if (owner == -1)
            {
                // New AI buying logic
                attemptPurchase(game, playerIdx, sq, sq->data.utility.purchasePrice, NO_GROUP);
            }
            else if (owner == playerIdx)
            {
                printf(" >> %s owns this utility.\n", game->players[playerIdx].name);
            }
            else if (sq->data.utility.mortgaged)
            {
                printf(" >> %s is mortgaged. No rent paid.\n", sq->name);
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
                rent = applyEventRentModifier(game, sq->index, rent, 0);
                printf("  >> %s must pay LKR %d rent to %s (owns %d utilities. Rolled %d)\n", 
                    game->players[playerIdx].name, rent, game->players[owner].name, utilityCount, diceTotal);
                payAmount(game, playerIdx, owner, rent);
                
            }
            break;
        }

        case SQUARE_TAX:{
            int tax = sq->data.taxAmount;
            printf("  >> %s must pay LKR %d in tax\n", game->players[playerIdx].name, tax);
            payAmount(game, playerIdx, -1, tax); // -1 means they pay the bank!
            break;
        }

        case SQUARE_SPECIAL:{
            
            if (pos == 30){
              game->players[playerIdx].position = 10;
              game->players[playerIdx].inJail = 1;
              game->players[playerIdx].jailTurns = 0;
              printf(" >> %s was sent to JAIL! \n", game->players[playerIdx].name);

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
            printf("  >> Just visiting jail.\n");
            break;

        case SQUARE_GO:
            // Already handled in main.c (pass GO check)
            break;
    }
}
