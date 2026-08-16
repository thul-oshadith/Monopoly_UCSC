#include <stdio.h>
#include <string.h>
#include "types.h"


int hasMonopoly(GameState *game, int playerIdx, PropertyGroup group);
int applyEventHouseCostModifier(GameState *game, int cost);
int applyDynamicMarketHouseCost(GameState *game, PropertyGroup group, int cost);
int getDynamicMortgageValue(GameState *game, int sqIdx);

//unmortgage its properties if player has cash
void attemptToUnmortgage(GameState *game, int playerIdx) {
    Player *p = &game->players[playerIdx];
    
    for (int i = 0; i < SQUARE_COUNT; i++) {
        Square *sq = &game->board[i];
        
        if (sq->type == SQUARE_PROPERTY && sq->data.property.owner == playerIdx && sq->data.property.mortgaged) {
            int unmortgageCost = getDynamicMortgageValue(game, i) * 110 / 100; // 10% interest
            
            // keeps a standard buffer of 1000 cash
            int cashBuffer = 1000;
            
            if (p->cash - unmortgageCost >= cashBuffer) {
                p->cash -= unmortgageCost;
                sq->data.property.mortgaged = 0;
                printf("  >> %s paid LKR %d to UNMORTGAGE %s!\n", p->name, unmortgageCost, sq->name);
            }
        }
        else if (sq->type == SQUARE_RAILWAY && sq->data.railway.owner == playerIdx && sq->data.railway.mortgaged) {
            int unmortgageCost = getDynamicMortgageValue(game, i) * 110 / 100;
            if (p->cash - unmortgageCost >= 1000) {
                p->cash -= unmortgageCost;
                sq->data.railway.mortgaged = 0;
                printf("  >> %s paid LKR %d to UNMORTGAGE %s!\n", p->name, unmortgageCost, sq->name);
            }
        }
        else if (sq->type == SQUARE_UTILITY && sq->data.utility.owner == playerIdx && sq->data.utility.mortgaged) {
            int unmortgageCost = getDynamicMortgageValue(game, i) * 110 / 100;
            if (p->cash - unmortgageCost >= 1000) {
                p->cash -= unmortgageCost;
                sq->data.utility.mortgaged = 0;
                printf("  >> %s paid LKR %d to UNMORTGAGE %s!\n", p->name, unmortgageCost, sq->name);
            }
        }
    }
}

// Finds the property in the group with the fewest buildings (to enforce even-build rule)
// Returns the square index, or -1 if all are fully upgraded to hotels
int getPropertyToUpgrade(GameState *game, int playerIdx, PropertyGroup group) {
    int minBuildings = 999;
    int bestSquare = -1;
    
    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *p = &game->board[i].data.property;
            if (p->group == group && p->owner == playerIdx) {
                // Level: 0-4 houses, 5 = hotel
                int level = p->hotel ? 5 : p->houses;
                
                if (level < minBuildings) {
                    minBuildings = level;
                    bestSquare = i;
                }
            }
        }
    }
    
    if (minBuildings >= 5) return -1; // Everything has a hotel
    return bestSquare;
}

// Builds one house on a property. Returns the cost charged, or 0 if it cannot build.
int buildHouse(GameState *game, int sqIdx, int actualCost) {
    Property *prop = &game->board[sqIdx].data.property;
    Player *player = &game->players[prop->owner];
    
    if (prop->houses >= 4) return 0;  // Already at max houses, need a hotel next
    if (prop->hotel == 1) return 0;   // Already has a hotel
    
    prop->houses++;

    printf("%s constructed one house on %s.\n", player->name, game->board[sqIdx].name);
    printf("Construction Cost : LKR %d.\n\n", actualCost);
    return actualCost;
}

// Upgrades a property from 4 houses to 1 hotel. Returns the cost charged, or 0 if it cannot build.
int buildHotel(GameState *game, int sqIdx, int actualCost) {
    Property *prop = &game->board[sqIdx].data.property;
    Player *player = &game->players[prop->owner];
    
    if (prop->houses != 4) return 0;  // Must have exactly 4 houses first
    if (prop->hotel == 1) return 0;   // Already has a hotel
    
    prop->houses = 0;  // Remove the 4 houses
    prop->hotel = 1;   // Replace with 1 hotel

    printf("%s upgraded %s to a Hotel.\n", player->name, game->board[sqIdx].name);
    return actualCost;
}

// Called at the end of a player's turn to build houses/hotels
void developProperties(GameState *game, int playerIdx) {
    Player *player = &game->players[playerIdx];
    if (player->bankrupt) return;

    // check all 8 color groups
    PropertyGroup groups[] = {BROWN, LIGHT_BLUE, PINK, ORANGE, RED, YELLOW, GREEN, DARK_BLUE};
    
    for (int g = 0; g < 8; g++) {
        if (hasMonopoly(game, playerIdx, groups[g])) {
            
            // Loop until they stop building
            while (1) {
                int sqIdx = getPropertyToUpgrade(game, playerIdx, groups[g]);
                if (sqIdx == -1) break; // Fully upgraded this group
                
                Property *prop = &game->board[sqIdx].data.property;
                
                // Determine the cost of the next upgrade
                int cost;
                if (prop->houses == 4) {
                    cost = prop->hotelCost;   // Next step is a hotel
                } else {
                    cost = prop->houseCost;   // Next step is a house
                }
                cost = applyEventHouseCostModifier(game, cost);
                cost = applyDynamicMarketHouseCost(game, groups[g], cost);
                
                if (game->currentRegulation == REG_HOUSING_SUBSIDY) {
                    cost = (int)(cost * 0.70f); // 30% discount
                }
                
                // Should we build? 
                int buildDecision = 0;

                // --- AGGRESSIVE INVESTOR ---
                // Build as long as they can afford it
                if (strcmp(player->name, "Aggressive Investor") == 0) {
                    if (player->cash >= cost) buildDecision = 1;
                }
                
                // --- RISK TAKER ---
                // Build as long as they can afford it
                else if (strcmp(player->name, "Risk Taker") == 0) {
                    if (player->cash >= cost) buildDecision = 1;
                }
                
                // --- CONSERVATIVE BANKER ---
                // Build cautiously, maintain 50% cash reserve
                else if (strcmp(player->name, "Conservative Banker") == 0) {
                    if (player->cash - cost >= player->cash / 2) {
                        // Never develops hotels until all outstanding loans have been settled
                        if (prop->houses == 4 && player->activeLoan.active) {
                            buildDecision = 0;
                        } else {
                            buildDecision = 1;
                        }
                    }
                }
                
                // --- OPPORTUNISTIC TRADER ---
                // delays construction during inflation
                else if (strcmp(player->name, "Opportunistic Trader") == 0) {
                    if (game->currentRegulation == REG_HOUSING_SUBSIDY) {
                        if (player->cash >= cost) buildDecision = 1;
                    }
                    else if (game->currentInflationRate > 0) {
                        buildDecision = 0; // Delays construction
                    } else if (player->cash - cost >= 500) {
                        buildDecision = 1;
                    }
                }

                // Execute build using the appropriate function
                if (buildDecision) {
                    int charged = 0;
                    
                    if (prop->houses == 4) {
                        charged = buildHotel(game, sqIdx, cost);  // Upgrade to hotel
                    } else {
                        charged = buildHouse(game, sqIdx, cost);  // Build a house
                    }
                    
                    if (charged > 0) {
                        player->cash -= charged;
                    } else {
                        break; // Something went wrong, stop building
                    }
                } else {
                    break; // Player doesn't want to or can't afford to build anymore
                }
            }
        }
    }
}

// AI performs maintenance at the beginning of their turn (Rules-LK 27, 29)
void performMaintenance(GameState *game, int playerIdx) {
    Player *player = &game->players[playerIdx];

    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *prop = &game->board[i].data.property;
            
            if (prop->owner == playerIdx && (prop->houses > 0 || prop->hotel > 0)) {
                
                if (prop->isDisasterDamaged) {
                    // Disaster repair costs 50% of replacement value
                    int replacementValue = (prop->houses * prop->houseCost) + (prop->hotel * prop->hotelCost);
                    int cost = replacementValue / 2;
                    
                    if (player->cash >= cost) {
                        player->cash -= cost;
                        prop->isDisasterDamaged = 0;
                        printf("  >>> %s repaired disaster damage on %s for LKR %d\n", player->name, prop->name, cost);
                    }
                } else if (prop->hasStructuralDamage) {
                    // Rule 29: Renovation costs 25% of replacement value
                    int replacementValue = (prop->houses * prop->houseCost) + (prop->hotel * prop->hotelCost);
                    int cost = replacementValue * 25 / 100;
                    
                    if (player->cash >= cost) {
                        player->cash -= cost;
                        prop->hasStructuralDamage = 0;
                        prop->buildingCondition = 100;
                        prop->roundsUnmaintained = 0;
                        printf("  >>> %s renovated structural damage on %s for LKR %d\n", player->name, prop->name, cost);
                    }
                } else if (prop->buildingCondition < 90 || prop->roundsUnmaintained >= 15) {
                    // Rule 27: Normal Maintenance
                    int cost = (prop->houses * prop->houseCost * 5 / 100) + (prop->hotel * prop->hotelCost * 8 / 100);
                    
                    if (player->cash >= cost && cost > 0) {
                        player->cash -= cost;
                        prop->buildingCondition = 100;
                        prop->roundsUnmaintained = 0;
                        printf("  >>> %s performed maintenance on %s for LKR %d\n", player->name, prop->name, cost);
                    }
                }
            }
        }
    }
}
