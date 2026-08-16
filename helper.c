#include <stdio.h>
#include <string.h>
#include "types.h"

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
    if (ownedByPlayer + 1 == totalInGroup) {
        return 1; // Yes, it completes the set
    } else {
        return 0; // No, set is not complete
    }
}

// Helper: checks if the player already owns the entire color set (for building houses)
int hasMonopoly(GameState *game, int playerIdx, PropertyGroup group) {
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
    
    return (totalInGroup > 0 && ownedByPlayer == totalInGroup);
}

int applyEventValueModifier(GameState *game, int sqIdx, int value);
int applyDynamicMarketValue(GameState *game, int sqIdx, int value);
int applyDynamicMarketHouseCost(GameState *game, PropertyGroup group, int cost);
int applyEventHouseCostModifier(GameState *game, int cost);

int calculateNetWorth(GameState *game, int i) {
    if (game->players[i].bankrupt) return 0;

    int propertyValue = 0;
    int buildingValue = 0;
    for (int j = 0; j < SQUARE_COUNT; j++) {
        int basePrice = 0;
        if (game->board[j].type == SQUARE_PROPERTY) basePrice = game->board[j].data.property.purchasePrice;
        else if (game->board[j].type == SQUARE_RAILWAY) basePrice = game->board[j].data.railway.purchasePrice;
        else if (game->board[j].type == SQUARE_UTILITY) basePrice = game->board[j].data.utility.purchasePrice;

        int dynamicPrice = applyDynamicMarketValue(game, j, basePrice);
        dynamicPrice = applyEventValueModifier(game, j, dynamicPrice);

        if (game->board[j].type == SQUARE_PROPERTY && game->board[j].data.property.owner == i) {
            propertyValue += dynamicPrice;
            
            int hCost = game->board[j].data.property.houseCost;
            hCost = applyEventHouseCostModifier(game, hCost);
            hCost = applyDynamicMarketHouseCost(game, game->board[j].data.property.group, hCost);
            if (game->currentRegulation == REG_HOUSING_SUBSIDY) {
                hCost = (int)(hCost * 0.70f);
            }
            
            int hotCost = game->board[j].data.property.hotelCost;
            hotCost = applyEventHouseCostModifier(game, hotCost);
            hotCost = applyDynamicMarketHouseCost(game, game->board[j].data.property.group, hotCost);
            if (game->currentRegulation == REG_HOUSING_SUBSIDY) {
                hotCost = (int)(hotCost * 0.70f);
            }

            buildingValue += (game->board[j].data.property.houses * hCost);
            buildingValue += (game->board[j].data.property.hotel * hotCost);
            
            if (game->board[j].data.property.hasStructuralDamage) {
                buildingValue -= ((game->board[j].data.property.houses * hCost) + (game->board[j].data.property.hotel * hotCost)) / 2;
            }
        } else if (game->board[j].type == SQUARE_RAILWAY && game->board[j].data.railway.owner == i) {
            propertyValue += dynamicPrice;
        } else if (game->board[j].type == SQUARE_UTILITY && game->board[j].data.utility.owner == i) {
            propertyValue += dynamicPrice;
        }
    }
    
    int outstandingLoans = game->players[i].activeLoan.active ? game->players[i].activeLoan.amount : 0;
    int accruedInterest = game->players[i].activeLoan.active ? (int)(game->players[i].activeLoan.amount * game->players[i].activeLoan.interestRate) : 0;

    return game->players[i].cash + propertyValue + buildingValue - outstandingLoans - accruedInterest;
}
