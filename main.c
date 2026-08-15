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

int main(){
    srand(time(NULL)); // seed random numbers for dice
    GameState game;
    game.currentEvent = EVENT_NONE;
    game.currentRound = 0;
    
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

            // Rule 27: Maintenance at the start of the turn
            performMaintenance(&game, p);

            int die1 = rand() % 6 + 1;
            int die2 = rand() % 6 + 1;
            int diceTotal = die1 + die2;

            //JAIL LOGIC
            if (game.players[p].inJail){
                printf("%s is in the JAIL (Turn%d). They rolled %d and %d.\n", game.players[p].name,
                game.players[p].jailTurns + 1, die1, die2);

                if(die1 == die2){
                    printf(" >> Doubles! %s escapes Jail \n", game.players[p].name);
                    game.players[p].inJail = 0;
                    game.players[p].jailTurns = 0;
                }
                else if (game.players[p].cash>=300)
                {
                    printf(" >> No Doubles, but %s pays LKR 300 bail to escap.\n", game.players[p].name);
                    game.players[p].cash -=300;
                    game.players[p].inJail = 0;
                    game.players[p].jailTurns = 0;
                }

                else
                {
                    game.players[p].jailTurns++;
                    if (game.players[p].jailTurns >=3)
                    {
                        printf(" >> %s has served 3 turns.Forced release!\n",game.players[p].name);
                        game.players[p].inJail = 0;
                        game.players[p].jailTurns = 0;
                    }
                    else
                    {
                        printf(" >> %s stays in Jail.\n", game.players[p].name);
                        continue; // skip rest of turn
                    }

                }

            }

            //END OF JAIL LOGIC

            int oldPosition = game.players[p].position;
            int newPosition = (oldPosition + diceTotal) % SQUARE_COUNT;

            game.players[p].position = newPosition;

            if (newPosition < oldPosition){
                game.players[p].cash += 2000;
                goCount[p]++;
                printf("%s passed Go! Collected LKR 2000\n", game.players[p].name);
            }
            
            printf("%s rolled %d.\n", game.players[p].name, diceTotal);
            printf("%s moves from Square %d to Square %d.\n", game.players[p].name, oldPosition, newPosition);

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

            int minGO = goCount[game.turnOrder[0]];
            for (int i = 1; i < PLAYER_COUNT; i++){
                int idx = game.turnOrder[i];
                if (!game.players[idx].bankrupt && goCount[idx] < minGO)
                {
                    minGO = goCount[idx];

                }


            }

            if (minGO>= 500){
                gameOver = 1;
                break;
            }

        }

        // Process loans and depreciation at the end of the round
        processEndRoundLoans(&game);
        processDepreciation(&game);
        processEndRoundInsurance(&game);
        
        if (game.currentRound > 0 && game.currentRound % 10 == 0) {
            triggerRandomDisaster(&game);
        }

        game.currentRound++;

        // Process Economic Events (Rule-LK 18)
        if (game.currentRound > 0 && game.currentRound % 15 == 0) {
            game.currentEvent = (EconomicEvent)(rand() % 8 + 1); // Random event from 1 to 8
            printf("\n======================================================\n");
            printf("  >>> NATIONAL ECONOMIC EVENT TRIGGERED! <<<\n");
            
            switch (game.currentEvent) {
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

        int propertyValue = 0;
        for (int j = 0; j < SQUARE_COUNT; j++) {
            if (game.board[j].type == SQUARE_PROPERTY && game.board[j].data.property.owner == i) {
                propertyValue += game.board[j].data.property.purchasePrice;
            } else if (game.board[j].type == SQUARE_RAILWAY && game.board[j].data.railway.owner == i) {
                propertyValue += game.board[j].data.railway.purchasePrice;
            } else if (game.board[j].type == SQUARE_UTILITY && game.board[j].data.utility.owner == i) {
                propertyValue += game.board[j].data.utility.purchasePrice;
            }
        }

        netWorths[i] = game.players[i].cash + propertyValue;
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
