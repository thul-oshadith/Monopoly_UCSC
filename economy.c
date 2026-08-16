#include "types.h"
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------
// ECONOMIC EVENTS 
// -------------------------------------------------------------

//displays the economic event in the console
void triggerEconomicEvent(GameState *game) {
    game->currentEvent = (EconomicEvent)(rand() % 8 + 1); // Random event from 1 to 8
    printf("\n======================================================\n");
    printf("  >>> NATIONAL ECONOMIC EVENT TRIGGERED! <<<\n");
    
    switch (game->currentEvent) {
        case EVENT_TOURISM_BOOM:
            printf("  Event: TOURISM BOOM\n  Hotels receive double rent. Southern coastal properties increase by 15%%.\n");
            break;
        case EVENT_FUEL_CRISIS:
            printf("  Event: FUEL CRISIS\n  Railway rent doubles. Property development costs increase 20%%.\n");
            break;
        case EVENT_HEAVY_MONSOON:
            printf("  Event: HEAVY MONSOON\n  Flood risk increases. Insurance premiums increase. Coastal properties lose 10%% value.\n");
            break;
        case EVENT_ECONOMIC_RECESSION:
            printf("  Event: ECONOMIC RECESSION\n  Property values decrease 15%%. Rent decreases 10%%. Loan interest increases by 15%%.\n");
            break;
        case EVENT_STOCK_MARKET_BOOM:
            printf("  Event: STOCK MARKET BOOM\n  Property values increase 10%%. Loan interest decreases by 10%%.\n");
            break;
        case EVENT_GOVERNMENT_HOUSING:
            printf("  Event: GOVERNMENT HOUSING PROGRAMME\n  House construction costs reduce 25%%.\n");
            break;
        case EVENT_FOREIGN_INVESTMENT:
            printf("  Event: FOREIGN INVESTMENT\n  Commercial properties increase 20%%.\n");
            break;
        case EVENT_POLITICAL_UNREST:
            printf("  Event: POLITICAL UNREST\n  Riot probability doubles. Hotel rent drops by 50%%. Business interruption claims increase.\n");
            break;
        default:
            break;
    }
    printf("======================================================\n\n");
}

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

// Modifies the property value
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

// -------------------------------------------------------------
// DEPRECIATION 
// -------------------------------------------------------------

#define DEPRECIATION_GRACE_ROUNDS 50
#define DEPRECIATION_INTERVAL 5
#define MAX_DEPRECIATION_PERCENT 30
#define STRUCTURAL_DAMAGE_PENALTY 15
#define STRUCTURAL_DAMAGE_THRESHOLD 20

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


// -------------------------------------------------------------
// DISASTER
// -------------------------------------------------------------

// Triggers random disasters every 10 rounds
void triggerRandomDisaster(GameState *game) {
    // Find all developed properties
    int developed[40];
    int count = 0;
    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *prop = &game->board[i].data.property;
            if (prop->houses > 0 || prop->hotel > 0) {
                developed[count++] = i;
            }
        }
    }
    if (count == 0) return; // No developed properties to hit
    
    // Pick a random developed property and a random disaster
    int targetIdx = developed[rand() % count];
    Square *sq = &game->board[targetIdx];
    Property *prop = &sq->data.property;
    Player *owner = &game->players[prop->owner];
    
    char *disasters[] = {"Fire", "Flood", "Riot", "Building Collapse", "Electrical Failure"};
    int dIdx = rand() % 5;
    char *disasterName = disasters[dIdx];
    
    printf("\n======================================================\n");
    printf("  >>> DISASTER STRIKES: %s hit %s! <<<\n", disasterName, prop->name);
    
    int repairCost = ((prop->houses * prop->houseCost) + (prop->hotel * prop->hotelCost)) / 2;
    int payout = 0;
    
    // Check coverage
    if (prop->insurance == BUSINESS_INTERRUPTION) {
        payout = repairCost + (prop->rent * 5 * 10); // hotel multiplier is 10
    } else if (prop->insurance == COMPREHENSIVE) {
        if (dIdx < 3) payout = repairCost; // Covers Fire, Flood, Riot
    } else if (prop->insurance == BASIC) {
        if (dIdx < 2) payout = (repairCost * 80) / 100; // Covers Fire, Flood
    }
    
    prop->isDisasterDamaged = 1;
    
    if (payout > 0) {
        owner->cash += payout;
        printf("  [!] Insurance payout of LKR %d credited to %s!\n", payout, owner->name);
    } else {
        owner->hasSufferedLoss = 1;
        printf("  [!] NO COVERAGE! %s bears the full repair cost of LKR %d and rent drops to 0 until repaired.\n", owner->name, repairCost);
    }
    printf("======================================================\n");
}

// -------------------------------------------------------------
// DYNAMIC PROPERTY MARKET 
// -------------------------------------------------------------

// Forward declaration (defined in helper.c)
const char* getGroupName(PropertyGroup group);


void processDynamicPropertyMarket(GameState *game) {
    // Tick down cooldowns
    for (int i = 0; i < 8; i++) {
        if (game->boomCooldowns[i] > 0) game->boomCooldowns[i]--;
        if (game->declineCooldowns[i] > 0) game->declineCooldowns[i]--;
    }
    
    // Review market every 10 rounds
    if (game->currentRound > 0 && game->currentRound % 10 == 0) {
        game->currentBoomGroup = NO_GROUP;
        game->currentDeclineGroup = NO_GROUP;
        
        // Find available Boom groups
        int availableBoomGroups[8];
        int boomCount = 0;
        for (int i = 0; i < 8; i++) {
            if (game->boomCooldowns[i] == 0) {
                availableBoomGroups[boomCount++] = i;
            }
        }
        
        if (boomCount > 0) {
            int boomIdx = rand() % boomCount;
            game->currentBoomGroup = (PropertyGroup)availableBoomGroups[boomIdx];
            game->boomCooldowns[game->currentBoomGroup] = 30;    // 30 rounds before Booming again
            game->declineCooldowns[game->currentBoomGroup] = 10; // 10 rounds before it can Decline
        }
        
        // Find available Decline groups
        int availableDeclineGroups[8];
        int declineCount = 0;
        for (int i = 0; i < 8; i++) {
            // Must not be on decline cooldown, and must not be the group we JUST picked for boom
            if (game->declineCooldowns[i] == 0 && i != game->currentBoomGroup) {
                availableDeclineGroups[declineCount++] = i;
            }
        }
        
        if (declineCount > 0) {
            int declineIdx = rand() % declineCount;
            game->currentDeclineGroup = (PropertyGroup)availableDeclineGroups[declineIdx];
            game->declineCooldowns[game->currentDeclineGroup] = 30; // 30 rounds before Declining again
            game->boomCooldowns[game->currentDeclineGroup] = 10;    // 10 rounds before it can Boom
        }
        
        if (game->currentBoomGroup != NO_GROUP || game->currentDeclineGroup != NO_GROUP) {
            printf("\n======================================================\n");
            printf("  >>> DYNAMIC PROPERTY MARKET REVIEW <<<\n");
            if (game->currentBoomGroup != NO_GROUP) {
                printf("   MARKET BOOM: %s Group properties!\n", getGroupName(game->currentBoomGroup));
            }
            if (game->currentDeclineGroup != NO_GROUP) {
                printf("   MARKET DECLINE: %s Group properties!\n", getGroupName(game->currentDeclineGroup));
            }
            printf("======================================================\n");
        }
    }
}

int applyDynamicMarketRent(GameState *game, int sqIdx, int rent) {
    Square *sq = &game->board[sqIdx];
    if (sq->type == SQUARE_PROPERTY) {
        PropertyGroup group = sq->data.property.group;
        if (group == game->currentBoomGroup) {
            rent = (rent * 125) / 100; // +25%
        } else if (group == game->currentDeclineGroup) {
            rent = (rent * 80) / 100; // -20%
        }
    }
    return rent;
}

int applyDynamicMarketValue(GameState *game, int sqIdx, int value) {
    Square *sq = &game->board[sqIdx];
    if (sq->type == SQUARE_PROPERTY) {
        PropertyGroup group = sq->data.property.group;
        if (group == game->currentBoomGroup) {
            value = (value * 120) / 100; // +20%
        } else if (group == game->currentDeclineGroup) {
            value = (value * 85) / 100; // -15%
        }
    }
    return value;
}

int applyDynamicMarketHouseCost(GameState *game, PropertyGroup group, int cost) {
    if (group == game->currentBoomGroup) {
        cost = (cost * 110) / 100; // +10%
    }
    return cost;
}

int getDynamicPurchasePrice(GameState *game, int sqIdx) {
    Square *sq = &game->board[sqIdx];
    int price = 0;
    if (sq->type == SQUARE_PROPERTY) {
        price = sq->data.property.purchasePrice;
        PropertyGroup group = sq->data.property.group;
        if (group == game->currentBoomGroup) {
            price = (price * 115) / 100; // +15%
        } else if (group == game->currentDeclineGroup) {
            price = (price * 85) / 100; // -15%
        }
    } else if (sq->type == SQUARE_RAILWAY) {
        price = sq->data.railway.purchasePrice;
    } else if (sq->type == SQUARE_UTILITY) {
        price = sq->data.utility.purchasePrice;
    }
    return price;
}

int getDynamicMortgageValue(GameState *game, int sqIdx) {
    Square *sq = &game->board[sqIdx];
    int mValue = 0;
    if (sq->type == SQUARE_PROPERTY) {
        mValue = sq->data.property.mortgageValue;
        PropertyGroup group = sq->data.property.group;
        if (group == game->currentBoomGroup) {
            mValue = (mValue * 115) / 100; // +15%
        } else if (group == game->currentDeclineGroup) {
            mValue = (mValue * 90) / 100; // -10%
        }
    } else if (sq->type == SQUARE_RAILWAY) {
        mValue = sq->data.railway.mortgageValue;
    } else if (sq->type == SQUARE_UTILITY) {
        mValue = sq->data.utility.mortgageValue;
    }
    return mValue;
}

// -------------------------------------------------------------
// INFLATION 
// -------------------------------------------------------------

void processInflation(GameState *game) {
 
    float possibleRates[] = {-0.03f, 0.00f, 0.02f, 0.05f, 0.08f, 0.12f};
    int rateIdx = rand() % 6;
    float rate = possibleRates[rateIdx];
    
    game->currentInflationRate = rate;
    
    // Set loan interest rate based on inflation severity
    // Moderate (5%, 8%) -> 10% interest
    // High (12%) -> 12% interest
    // Otherwise -> 8% interest
    if (rate >= 0.04f && rate <= 0.09f) {
        game->currentLoanInterestRate = 0.10f;
    } else if (rate >= 0.11f) {
        game->currentLoanInterestRate = 0.12f;
    } else {
        game->currentLoanInterestRate = 0.08f;
    }
    
    // Announce Inflation
    printf("\n======================================================\n");
    printf("  >>> NATIONAL ECONOMY REVIEW <<<\n");
    if (rate < 0) {
        printf("   DEFLATION! The economy shrank by %.0f%%.\n", -rate * 100);
    } else if (rate == 0) {
        printf("   STAGNATION. The economy remains stable at 0%% inflation.\n");
    } else {
        printf("   INFLATION! The economy grew by %.0f%%.\n", rate * 100);
    }
    printf("   New Loan Interest Rate: %.0f%%\n", game->currentLoanInterestRate * 100);
    printf("======================================================\n");
    
    // Compound all base property values
    if (rate != 0.0f) {
        float multiplier = 1.0f + rate;
        for (int i = 0; i < SQUARE_COUNT; i++) {
            Square *sq = &game->board[i];
            if (sq->type == SQUARE_PROPERTY) {
                sq->data.property.purchasePrice = (int)(sq->data.property.purchasePrice * multiplier);
                sq->data.property.mortgageValue = (int)(sq->data.property.mortgageValue * multiplier);
                sq->data.property.rent = (int)(sq->data.property.rent * multiplier);
                sq->data.property.houseCost = (int)(sq->data.property.houseCost * multiplier);
                sq->data.property.hotelCost = (int)(sq->data.property.hotelCost * multiplier);
            } else if (sq->type == SQUARE_RAILWAY) {
                sq->data.railway.purchasePrice = (int)(sq->data.railway.purchasePrice * multiplier);
                sq->data.railway.mortgageValue = (int)(sq->data.railway.mortgageValue * multiplier);
            } else if (sq->type == SQUARE_UTILITY) {
                sq->data.utility.purchasePrice = (int)(sq->data.utility.purchasePrice * multiplier);
                sq->data.utility.mortgageValue = (int)(sq->data.utility.mortgageValue * multiplier);
            }
        }
    }
}


// -------------------------------------------------------------
// GOVERNMENT REGULATIONS
// -------------------------------------------------------------


// Prototype for payAmount
void payAmount(GameState *game, int payerIdx, int payeeIdx, int amount);

// Triggers government regulations every 20 rounds (Rule LK-24)
void processGovernmentRegulations(GameState *game) {
    game->currentRegulation = (GovernmentRegulation)(rand() % 8 + 1); // 1 to 8
    
    printf("\n======================================================\n");
    printf("    GOVERNMENT REGULATION ISSUED!  \n");
    
    switch (game->currentRegulation) {
        case REG_INCREASE_PROPERTY_TAX:
            printf("  Regulation: INCREASE PROPERTY TAX\n  Income Tax increases by 50%%.\n");
            break;
        case REG_REDUCE_LOAN_INTEREST:
            printf("  Regulation: REDUCE LOAN INTEREST\n  Loan interest decreases by 2%%.\n");
            break;
        case REG_HOUSING_SUBSIDY:
            printf("  Regulation: HOUSING SUBSIDY\n  House construction costs reduce 30%%.\n");
            break;
        case REG_LUXURY_PROPERTY_TAX:
            printf("  Regulation: LUXURY PROPERTY TAX\n  Instant 25%% tax on properties with hotels!\n");
            
            // Instantly charge 25% of total property value for hotels
            for (int i = 0; i < SQUARE_COUNT; i++) {
                if (game->board[i].type == SQUARE_PROPERTY) {
                    Property *prop = &game->board[i].data.property;
                    if (prop->hotel > 0 && prop->owner != -1) {
                        int totalValue = prop->purchasePrice + (4 * prop->houseCost) + prop->hotelCost;
                        int tax = (int)(totalValue * 0.25f);
                        printf("  >> %s hit by Luxury Tax on %s! Pays LKR %d.\n", game->players[prop->owner].name, prop->name, tax);
                        payAmount(game, prop->owner, -1, tax);
                    }
                }
            }
            break;
        case REG_RAILWAY_MODERNIZATION:
            printf("  Regulation: RAILWAY MODERNIZATION\n  Railway rents increase 25%%.\n");
            break;
        case REG_ELECTRICITY_TARIFF_REVISION:
            printf("  Regulation: ELECTRICITY TARIFF REVISION\n  Utility rents increase 20%%.\n");
            break;
        case REG_INSURANCE_REGULATION:
            printf("  Regulation: INSURANCE REGULATION\n  Insurance premiums decrease 15%%.\n");
            break;
        case REG_ANTI_SPECULATION_ACT:
            printf("  Regulation: ANTI-SPECULATION ACT\n  Players may own at most 3 undeveloped properties.\n");
            break;
        default:
            break;
    }
    printf("======================================================\n\n");
}