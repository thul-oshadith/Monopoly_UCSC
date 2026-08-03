#include <stdio.h>
#include "types.h"
#include <stdlib.h>
#include <time.h>

void init_Board(GameState *game);
void init_Players(GameState *game);
void print_Players(GameState *game);

int main(){
    srand(time(NULL)); // seed random numbers for dice
    GameState game;
    init_Board(&game);
    init_Players(&game);
    print_Players(&game);
    return 0;

}
