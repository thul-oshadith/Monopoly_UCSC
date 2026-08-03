#include <stdio.h>
#include "types.h"
#include <string.h>

void init_Players(GameState *game){
 
 //    Player 01
 strcpy(game->players[0].name, "Aggressive Investor");
 game->players[0].position = 0;
 game->players[0].cash = STARTING_CASH;
 game->players[0].inJail = 0;
 game->players[0].jailTurns = 0;
 game->players[0].bankrupt = 0;
 game->players[0].turnOrder = 0;

 //    Player 02
 strcpy(game->players[1].name, "Conservative Banker");
 game->players[1].position = 0;
 game->players[1].cash = STARTING_CASH;
 game->players[1].inJail = 0;
 game->players[1].jailTurns = 0;
 game->players[1].bankrupt = 0;
 game->players[1].turnOrder = 0;

strcpy(game->players[2].name, "Risk Taker");
 game->players[2].position = 0;
 game->players[2].cash = STARTING_CASH;
 game->players[2].inJail = 0;
 game->players[2].jailTurns = 0;
 game->players[2].bankrupt = 0;
 game->players[2].turnOrder = 0;

 strcpy(game->players[3].name, "Opportunistic Trader");
 game->players[3].position = 0;
 game->players[3].cash = STARTING_CASH;
 game->players[3].inJail = 0;
 game->players[3].jailTurns = 0;
 game->players[3].bankrupt = 0;
 game->players[3].turnOrder = 0;


}
void print_players(GameState *game) {                                //for testing
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("Player %d: %s\n", i + 1, game->players[i].name);
        printf("Position: %d\n", game->players[i].position);
        printf("Cash: %d\n", game->players[i].cash);
        printf("In Jail: %d\n", game->players[i].inJail);
        printf("Jail Turns: %d\n", game->players[i].jailTurns);
        printf("Bankrupt: %d\n", game->players[i].bankrupt);
        printf("\n");
    }
}

