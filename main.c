#include <stdio.h>
#include "types.h"
#include <stdlib.h>
#include <time.h>

void init_Board(GameState *game);
void init_Players(GameState *game);
void decideTurnOrder(GameState *game);
void handleLanding(GameState *game, int playerIdx, int diceTotal);

int main(){
    srand(time(NULL)); // seed random numbers for dice
    GameState game;
    
    //Setting up the game
    init_Board(&game);
    init_Players(&game);

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
            
            printf(" %s rolled total dice score of %d, moved to %s (square %d)\n", game.players[p].name,
            diceTotal, game.board[newPosition].name,  newPosition);

            handleLanding(&game , p, diceTotal);

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
