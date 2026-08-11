#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"

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


// Helper: checks if buying this property would complete a color set for the player
int checkIfCompletesSet(GameState *game, int playerIdx, PropertyGroup group) {
    if (group == NO_GROUP) return 0;
    
    int totalInGroup = 0;
    int ownedByPlayer = 0;
    
    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *p = &game->board[i].data.property;
            if (p->group == group) {
                totalInGroup++;
                if (p->owner == playerIdx) {
                    ownedByPlayer++;
                }
            }
        }
    }
    
    // If buying THIS property makes owned == total, set is complete
    return (ownedByPlayer + 1 == totalInGroup);
}

// AI Purchase Decision — each player has their own brain
void attemptPurchase(GameState *game, int playerIdx, Square *sq, int purchasePrice, PropertyGroup group) {
    Player *player = &game->players[playerIdx];
    int buyDecision = 0;

    // --- AGGRESSIVE INVESTOR ---
    // Rule: "Always purchases if sufficient funds remain to pay at least one future rent"
    // We use 100 (the cheapest rent on the board) as the minimum rent threshold
    if (strcmp(player->name, "Aggressive Investor") == 0) {
        if (player->cash - purchasePrice >= 100) {
            buyDecision = 1;
        }
    }

    // --- CONSERVATIVE BANKER ---
    // Rule: "Purchases properties only if at least 50% of current cash remains after purchase"
    // So: purchasePrice must be <= half of their current cash
    else if (strcmp(player->name, "Conservative Banker") == 0) {
        if (purchasePrice <= player->cash / 2) {
            buyDecision = 1;
        }
    }

    // --- RISK TAKER ---
    // Rule: "Purchases every available property whenever legally possible"
    // Simply: buy if they have the cash
    else if (strcmp(player->name, "Risk Taker") == 0) {
        if (player->cash >= purchasePrice) {
            buyDecision = 1;
        }
    }

    // --- OPPORTUNISTIC TRADER ---
    // Rule: "Purchases only when projected appreciation exceeds construction costs"
    // Placeholder: Buy if it completes a color set, OR if 30% of cash remains after purchase
    // (We will refine this once the economic event system is built)
    else if (strcmp(player->name, "Opportunistic Trader") == 0) {
        int completesSet = 0;
        if (sq->type == SQUARE_PROPERTY) {
            completesSet = checkIfCompletesSet(game, playerIdx, group);
        }
        
        if (completesSet) {
            if (player->cash >= purchasePrice) {
                buyDecision = 1;
            }
        } else if (player->cash - purchasePrice > player->cash * 3 / 10) {
            buyDecision = 1;
        }
    }

    // EXECUTE THE DECISION
    if (buyDecision) {
        player->cash -= purchasePrice;

        if (sq->type == SQUARE_PROPERTY)  sq->data.property.owner = playerIdx;
        else if (sq->type == SQUARE_RAILWAY) sq->data.railway.owner = playerIdx;
        else if (sq->type == SQUARE_UTILITY) sq->data.utility.owner = playerIdx;

        printf("  >> %s bought %s for LKR %d\n", player->name, sq->name, purchasePrice);
    } else {
        printf("  >> %s declined to buy %s. (Going to Auction)\n", player->name, sq->name);
        // TODO: implement auction logic here later
    }
}


// Helper: Processes payments and handles bankruptcy if a player cannot pay
void payAmount(GameState *game, int payerIdx, int payeeIdx, int amount) {
    Player *payer = &game->players[payerIdx];
    
    // Does the player have enough cash?
    if (payer->cash >= amount) {
        payer->cash -= amount;
        
        // If they are paying another player (not the bank), give the payee the cash
        if (payeeIdx != -1) {
            game->players[payeeIdx].cash += amount;
        }
    } else {
        // Player cannot afford the payment! (We will add Mortgage logic here later)
        // For now, they go Bankrupt immediately.
        
        payer->bankrupt = 1;
        payer->cash = 0; // Wipe their remaining cash
        printf("\n  🚨🚨 %s is BANKRUPT! 🚨🚨\n", payer->name);
        
        // Return all their properties to the Bank
        for (int i = 0; i < SQUARE_COUNT; i++) {
            if (game->board[i].type == SQUARE_PROPERTY && game->board[i].data.property.owner == payerIdx) {
                game->board[i].data.property.owner = -1;
                game->board[i].data.property.mortgaged = 0; // Reset mortgage status
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            } 
            else if (game->board[i].type == SQUARE_RAILWAY && game->board[i].data.railway.owner == payerIdx) {
                game->board[i].data.railway.owner = -1;
                game->board[i].data.railway.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            } 
            else if (game->board[i].type == SQUARE_UTILITY && game->board[i].data.utility.owner == payerIdx) {
                game->board[i].data.utility.owner = -1;
                game->board[i].data.utility.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            }
        }
    }
}




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
            int rent = prop->rent;
            printf("  >> %s must pay LKR %d rent to %s\n", 
                game->players[playerIdx].name, rent, game->players[prop->owner].name);
            payAmount(game, playerIdx, prop->owner, rent);
        }
        else {
            // Player owns it — nothing happens
            printf("  >> %s owns this property.\n", game->players[playerIdx].name);
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
            // TODO
            break;

        case SQUARE_INSURANCE:
            // TODO
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
