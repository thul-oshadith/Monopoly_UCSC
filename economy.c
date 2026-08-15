#include "types.h"

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
