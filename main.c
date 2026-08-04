#include <stdio.h>
#include "types.h"
#include <stdlib.h>
#include <time.h>

void init_Board(GameState *game);
void init_Players(GameState *game);
void decideTurnOrder(GameState *game);


int main(){
    srand(time(NULL)); // seed random numbers for dice
    GameState game;
    
    //Setting up the game
    init_Board(&game);
    init_Players(&game);

    //Determine the play order
    decideTurnOrder(&game);
    
    for(int round = 1; round <=500; round++){
        printf("\n===Round %d ===\n", round);

        for(int t = 0; t < PLAYER_COUNT; t++){ 
            int p = game.turnOrder[t];

            if(game.players[p].bankrupt){
                continue;
            }

            int diceTotal = (rand()%6 +1) + (rand()%6 + 1);

            int oldPosition = game.players[p].position;
            int newPosition = (oldPosition + diceTotal) % SQUARE_COUNT;

            game.players[p].position = newPosition;

            if (newPosition < oldPosition){
                game.players[p].cash += 2000;
                printf("%s passed Go! Collected LKR 2000\n", game.players[p].name);

            }
            
            printf(" %s rolled total dice score of %d, moved to %s (square %d)\n", game.players[p].name,
            diceTotal, game.board[newPosition].name,  newPosition);

            

        }


    }

    return 0;

}
