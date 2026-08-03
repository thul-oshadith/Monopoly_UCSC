#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"

// Recursive function to handle rolling and ties
void rankPlayers(GameState *game, int candidates[], int count, int *resultPos, int isReroll) {
    // Base cases to stop recursion
    if (count == 1) {
        game->turnOrder[*resultPos] = candidates[0]; // lock in this player
        (*resultPos)++;                              // move to next chair
        return;
    }
    if (count == 0) return;

    int rolls[PLAYER_COUNT] = {0};

    // Step 1: Roll dice for all candidates
    for (int i = 0; i < count; i++) {
        int playerIdx = candidates[i];
        rolls[i] = (rand() % 6 + 1) + (rand() % 6 + 1);

        if (isReroll) {
            printf("  %s rerolls: %d\n", game->players[playerIdx].name, rolls[i]);
        } else {
            printf("  %s rolls: %d\n", game->players[playerIdx].name, rolls[i]);
        }
    }

    // Step 2: Find unique rolls
    int uniqueRolls[12];
    int uniqueCount = 0;

    // Collect unique rolls
    for (int i = 0; i < count; i++) {
        int found = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (uniqueRolls[j] == rolls[i]) {
                found = 1; break;
            }
        }
        if (!found) {
            uniqueRolls[uniqueCount++] = rolls[i];
        }
    }

    // Sort them from highest to lowest (Bubble Sort)
    for (int i = 0; i < uniqueCount - 1; i++) {
        for (int j = i + 1; j < uniqueCount; j++) {
            if (uniqueRolls[j] > uniqueRolls[i]) {
                int temp = uniqueRolls[i];
                uniqueRolls[i] = uniqueRolls[j];
                uniqueRolls[j] = temp;
            }
        }
    }

    // Step 3: Process groups highest to lowest
    for (int g = 0; g < uniqueCount; g++) {
        int group[PLAYER_COUNT];
        int groupSize = 0;
        
        // Find everyone who rolled this exact number
        for (int i = 0; i < count; i++) {
            if (rolls[i] == uniqueRolls[g]) {
                group[groupSize++] = candidates[i];
            }
        }

        if (groupSize == 1) {
            // No tie! Place them.
            game->turnOrder[*resultPos] = group[0];
            (*resultPos)++;
        } else {
            // Tie! Call this exact function again with just the tied group
            printf("\n  Tie detected! Rerolling...\n");
            rankPlayers(game, group, groupSize, resultPos, 1);
        }
    }
}

void decideTurnOrder(GameState *game) {
    printf("=== Determining Turn Order ===\n");
    
    int allPlayers[PLAYER_COUNT];
    for (int i = 0; i < PLAYER_COUNT; i++) {
        allPlayers[i] = i;
    }

    int resultPos = 0;
    // Kick off the recursive ranking
    rankPlayers(game, allPlayers, PLAYER_COUNT, &resultPos, 0);

    // Save their turn order inside the Player struct for easy access later
    for (int i = 0; i < PLAYER_COUNT; i++) {
        game->players[game->turnOrder[i]].turnOrder = i + 1;
    }

    // Print the final result
    printf("\n=== Final Turn Order ===\n");
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("  %d. %s\n", i + 1, game->players[game->turnOrder[i]].name);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    GameState game;
    
    // Initialize dummy players just for this test
    strcpy(game.players[0].name, "Aggressive Investor");
    strcpy(game.players[1].name, "Conservative Banker");
    strcpy(game.players[2].name, "Risk Taker");
    strcpy(game.players[3].name, "Opportunistic Trader");

    // Run the logic
    decideTurnOrder(&game);

    return 0;
}
