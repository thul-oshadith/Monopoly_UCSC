#include <stdio.h>
#include "types.h"

Player players[PLAYER_COUNT] = {

    {"Aggressive_Investor", 0, STARTING_CASH, 0, 0, 0},
    {"Conservative_Banker", 0, STARTING_CASH, 0, 0, 0},
    {"Risk_Taker", 0, STARTING_CASH, 0, 0, 0},
    {"Oppertunistic_Trader", 0, STARTING_CASH, 0, 0, 0}
}; 

void print_players() {                                //for testing
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("Player %d: %s\n", i + 1, players[i].name);
        printf("Position: %d\n", players[i].position);
        printf("Cash: %d\n", players[i].cash);
        printf("In Jail: %d\n", players[i].inJail);
        printf("Jail Turns: %d\n", players[i].jailTurns);
        printf("Bankrupt: %d\n", players[i].bankrupt);
        printf("\n");
    }
}

int main() {                                      //for testing
    print_players();
    return 0;
}