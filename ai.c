#include <stdio.h>
#include <string.h>
#include "types.h"

// Forward declaration
int hasMonopoly(GameState *game, int playerIdx, PropertyGroup group);

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
    
    if (minBuildings >= 5) return -1; // Everything has a hotel!
    return bestSquare;
}

// Builds one house on a property. Returns the cost charged, or 0 if it cannot build.
int buildHouse(GameState *game, int sqIdx) {
    Property *prop = &game->board[sqIdx].data.property;
    
    if (prop->houses >= 4) return 0;  // Already at max houses, need a hotel next
    if (prop->hotel == 1) return 0;   // Already has a hotel
    
    prop->houses++;
    printf("  🏠 %s now has %d house(s) (cost LKR %d)\n", game->board[sqIdx].name, prop->houses, prop->houseCost);
    return prop->houseCost;
}

// Upgrades a property from 4 houses to 1 hotel. Returns the cost charged, or 0 if it cannot build.
int buildHotel(GameState *game, int sqIdx) {
    Property *prop = &game->board[sqIdx].data.property;
    
    if (prop->houses != 4) return 0;  // Must have exactly 4 houses first
    if (prop->hotel == 1) return 0;   // Already has a hotel
    
    prop->houses = 0;  // Remove the 4 houses
    prop->hotel = 1;   // Replace with 1 hotel
    printf("  🏗️  %s upgraded to a HOTEL! (cost LKR %d)\n", game->board[sqIdx].name, prop->hotelCost);
    return prop->hotelCost;
}

// Called at the end of a player's turn to build houses/hotels
void developProperties(GameState *game, int playerIdx) {
    Player *player = &game->players[playerIdx];
    if (player->bankrupt) return;

    // We check all 8 color groups
    PropertyGroup groups[] = {BROWN, LIGHT_BLUE, PINK, ORANGE, RED, YELLOW, GREEN, DARK_BLUE};
    
    for (int g = 0; g < 8; g++) {
        if (hasMonopoly(game, playerIdx, groups[g])) {
            
            // Loop until they stop building
            while (1) {
                int sqIdx = getPropertyToUpgrade(game, playerIdx, groups[g]);
                if (sqIdx == -1) break; // Fully upgraded this group!
                
                Property *prop = &game->board[sqIdx].data.property;
                
                // Determine the cost of the next upgrade
                int cost;
                if (prop->houses == 4) {
                    cost = prop->hotelCost;   // Next step is a hotel
                } else {
                    cost = prop->houseCost;   // Next step is a house
                }
                
                // --- AI DECISION: Should we build? ---
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
                    if (player->cash - cost >= player->cash / 2) buildDecision = 1;
                }
                
                // --- OPPORTUNISTIC TRADER ---
                // Balanced approach
                else if (strcmp(player->name, "Opportunistic Trader") == 0) {
                    if (player->cash - cost >= 500) buildDecision = 1;
                }

                // Execute build using the appropriate function
                if (buildDecision) {
                    int charged = 0;
                    
                    if (prop->houses == 4) {
                        charged = buildHotel(game, sqIdx);  // Upgrade to hotel
                    } else {
                        charged = buildHouse(game, sqIdx);  // Build a house
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
