#include <stdio.h>
#include "types.h"
#include <stdlib.h>
#include <time.h>

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
int handleJailTurn(GameState *game, int p, int die1, int die2);
void displayRoundSummary(GameState *game);

// Valuation helpers
int calculateNetWorth(GameState *game, int playerIdx);
int getDynamicPurchasePrice(GameState *game, int sqIdx);
int applyEventValueModifier(GameState *game, int sqIdx, int value);
int applyDynamicMarketValue(GameState *game, int sqIdx, int value);
int applyDynamicMarketHouseCost(GameState *game, PropertyGroup group, int cost);
int applyEventHouseCostModifier(GameState *game, int cost);

int main(){
    srand(time(NULL)); // seed random numbers for dice
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
    
    // Initialize board
    init_Board(&game);
    init_Players(&game);

    // Print starting banner
    printf("\nMONOPOLY-LK Simulation\n\n");
    for(int i = 0; i < PLAYER_COUNT; i++) {
        printf("Player %d : %s\n", i+1, game.players[i].name);
    }

    printf("\nEach player begins with LKR 30,000.\n\n");

    //Determine the play order
    decideTurnOrder(&game);
    
    int goCount[PLAYER_COUNT] = {0};
    int gameOver = 0;

    while(!gameOver){


        for(int t = 0; t < PLAYER_COUNT; t++){ 
            int p = game.turnOrder[t];

            if(game.players[p].bankrupt){
                continue;
            }

            // AI Unmortgages properties if they have enough cash
            attemptToUnmortgage(&game, p);

            // Rule 27: Maintenance at the start of the turn
            performMaintenance(&game, p);

            int die1 = rand() % 6 + 1;
            int die2 = rand() % 6 + 1;
            int diceTotal = die1 + die2;

            //JAIL LOGIC
            if (handleJailTurn(&game, p, die1, die2)) {
                continue; // skip rest of turn
            }
            

            int oldPosition = game.players[p].position;
            int newPosition = (oldPosition + diceTotal) % SQUARE_COUNT;

            game.players[p].position = newPosition;

            printf("%s rolled %d.\n", game.players[p].name, diceTotal);
            printf("%s moves from Square %d to Square %d.\n", game.players[p].name, oldPosition, newPosition);

            if (newPosition < oldPosition){
                game.players[p].cash += 2000;
                goCount[p]++;
                printf("%s passed Go! Collected LKR 2000\n", game.players[p].name);
            }

            handleLanding(&game , p, diceTotal);
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
        } // End of player turn loop

        // Calculate minGO across all active players
        int minGO = 99999;
        for (int i = 0; i < PLAYER_COUNT; i++){
            int idx = game.turnOrder[i];
            if (!game.players[idx].bankrupt && goCount[idx] < minGO) {
                minGO = goCount[idx];
            }
        }

        if (minGO >= 500){
            gameOver = 1;
            break;
        }

        // If minGO has increased, it means all active players have completed another full lap.
        // This marks a new ROUND.
        if (minGO > game.currentRound) {
            int roundsPassed = minGO - game.currentRound;
            
            for (int r = 0; r < roundsPassed; r++) {
                // Process loans and depreciation at the end of the round
                processEndRoundLoans(&game);
                processDepreciation(&game);
                processEndRoundInsurance(&game);
                
                game.currentRound++;
                
                if (game.currentRound > 0 && game.currentRound % 10 == 0) {
                    triggerRandomDisaster(&game);
                    processDynamicPropertyMarket(&game);
                    processInflation(&game);
                }

                // Process Economic Events (Rule-LK 18)
                if (game.currentRound > 0 && game.currentRound % 15 == 0) {
                    triggerEconomicEvent(&game);
                }

                // Process Government Regulations (Rule LK-24)
                if (game.currentRound > 0 && game.currentRound % 20 == 0) {
                    processGovernmentRegulations(&game);
                }
                
                displayRoundSummary(&game);
            }
        }

    }

    // === END OF GAME SUMMARY ===
    printf("\n========================================\n");
    printf("         GAME OVER - FINAL RESULTS       \n");
    printf("========================================\n\n");

    // First pass: calculate net worth for each player and find the winner
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

    // Second pass: print the results
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (game.players[i].bankrupt) {
            printf("  X %s -- BANKRUPT\n", game.players[i].name);
        } else if (i == winnerIdx) {
            printf("  >> WINNER: %s -- Net Worth: LKR %d\n", game.players[i].name, netWorths[i]);
        } else {
            printf("  %s -- Net Worth: LKR %d\n", game.players[i].name, netWorths[i]);
        }
    }

    printf("\n========================================\n");

    return 0;

}
