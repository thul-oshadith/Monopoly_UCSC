#include "types.h"
#include <stdio.h>

// Depreciation Constants (Rule-LK 15-16, 28)
#define DEPRECIATION_GRACE_ROUNDS 50
#define DEPRECIATION_INTERVAL 5
#define MAX_DEPRECIATION_PERCENT 30
#define STRUCTURAL_DAMAGE_PENALTY 15
#define STRUCTURAL_DAMAGE_THRESHOLD 20
// Modifies the base rent of a property based on active economic events
int applyEventRentModifier(GameState *game, int sqIdx, int rent, int isHotel) {
    Square *sq = &game->board[sqIdx];
    
    if (game->currentEvent == EVENT_TOURISM_BOOM) {
        if (sq->type == SQUARE_PROPERTY && isHotel) {
            rent *= 2;
        }
    } else if (game->currentEvent == EVENT_ECONOMIC_RECESSION) {
        if (sq->type == SQUARE_PROPERTY || sq->type == SQUARE_RAILWAY || sq->type == SQUARE_UTILITY) {
            rent = (rent * 90) / 100; // -10%
        }
    } else if (game->currentEvent == EVENT_FUEL_CRISIS) {
        if (sq->type == SQUARE_RAILWAY) {
            rent *= 2;
        }
    } else if (game->currentEvent == EVENT_POLITICAL_UNREST) {
        if (sq->type == SQUARE_PROPERTY && isHotel) {
            rent /= 2;
        }
    }
    return rent;
}

// Modifies the property value (for loans and auctions)
int applyEventValueModifier(GameState *game, int sqIdx, int value) {
    Square *sq = &game->board[sqIdx];
    
    if (sq->type == SQUARE_PROPERTY) {
        if (game->currentEvent == EVENT_TOURISM_BOOM && sq->data.property.isSouthernCoastal) {
            value = (value * 115) / 100; // +15%
        } else if (game->currentEvent == EVENT_HEAVY_MONSOON && sq->data.property.isCoastal) {
            value = (value * 90) / 100; // -10%
        } else if (game->currentEvent == EVENT_FOREIGN_INVESTMENT && sq->data.property.isCommercial) {
            value = (value * 120) / 100; // +20%
        }
    }
    
    if (game->currentEvent == EVENT_ECONOMIC_RECESSION) {
        value = (value * 85) / 100; // -15% for all
    } else if (game->currentEvent == EVENT_STOCK_MARKET_BOOM) {
        value = (value * 110) / 100; // +10%
    }

    return value;
}

// Applies property depreciation and structural damage penalties (Rules 15-16, 28)
int applyDepreciationModifier(GameState *game, int sqIdx, int value) {
    Square *sq = &game->board[sqIdx];

    if (sq->type == SQUARE_PROPERTY) {
        Property *prop = &sq->data.property;

        // Property Depreciation (Rules 15-16): value drops after grace period
        if (prop->propertyAge > DEPRECIATION_GRACE_ROUNDS) {
            int depPercent = (prop->propertyAge - DEPRECIATION_GRACE_ROUNDS) / DEPRECIATION_INTERVAL;
            if (depPercent > MAX_DEPRECIATION_PERCENT) depPercent = MAX_DEPRECIATION_PERCENT;

            int remainingPercent = 100 - depPercent;
            value = (value * remainingPercent) / 100;
        }

        // Building Structural Damage (Rule 28): additional penalty
        if (prop->hasStructuralDamage) {
            int remainingPercent = 100 - STRUCTURAL_DAMAGE_PENALTY;
            value = (value * remainingPercent) / 100;
        }
    }

    return value;
}

// Modifies house building costs
int applyEventHouseCostModifier(GameState *game, int cost) {
    if (game->currentEvent == EVENT_FUEL_CRISIS) {
        cost = (cost * 120) / 100; // +20%
    } else if (game->currentEvent == EVENT_GOVERNMENT_HOUSING) {
        cost = (cost * 75) / 100; // -25%
    }
    return cost;
}

// Modifies loan interest rate
float applyEventLoanInterest(GameState *game, float baseInterest) {
    if (game->currentEvent == EVENT_ECONOMIC_RECESSION) {
        return baseInterest + 0.15f; // +15%
    } else if (game->currentEvent == EVENT_STOCK_MARKET_BOOM) {
        float rate = baseInterest - 0.10f; // -10%
        return rate < 0 ? 0.0f : rate;
    }
    return baseInterest;
}

// Processes end-of-round depreciation for properties and buildings
void processDepreciation(GameState *game) {
    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *prop = &game->board[i].data.property;
            
            // Property Depreciation (Rule-LK 15): increase age if owned
            if (prop->owner != -1) {
                prop->propertyAge++;
            }

            // Building Depreciation (Rule-LK 25-28): deteriorate if buildings exist
            if (prop->houses > 0 || prop->hotel > 0) {
                prop->buildingCondition -= 2;
                if (prop->buildingCondition < 0) prop->buildingCondition = 0;
                
                prop->roundsUnmaintained++;
                
                // Rule-LK 28: Structural damage after threshold
                if (prop->roundsUnmaintained > STRUCTURAL_DAMAGE_THRESHOLD && prop->hasStructuralDamage == 0) {
                    prop->hasStructuralDamage = 1;
                    printf("  [!] %s has suffered structural damage due to lack of maintenance!\n", prop->name);
                }
            }
        }
    }
}
