#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "types.h"

int turnOrder[PLAYER_COUNT]; // turnOrder[0] = index of 1st player, turnOrder[1] = 2nd, etc.

// Ranks a subset of players by rolling dice.
// players[]    = the full players array (passed in, no extern needed)
// candidates[] = array of player indices to rank
// count        = how many players in this subset
// resultPos    = pointer to the next open slot in turnOrder[]
// isReroll     = 1 if this is a tie-breaker reroll, 0 if first roll
void rankPlayers(Player players[], int candidates[], int count, int *resultPos, int isReroll) {
    // Base case: only one player left, no need to roll
    if (count == 1) {
        turnOrder[*resultPos] = candidates[0];
        (*resultPos)++;
        return;
    }
    if (count == 0) return;

    // 1. Roll dice for every candidate
    int rolls[PLAYER_COUNT];

    for (int i = 0; i < count; i++) {
        int playerIdx = candidates[i];
        rolls[i] = (rand() % 6 + 1) + (rand() % 6 + 1);

        if (isReroll) {
            printf("  %s rerolls: %d\n", players[playerIdx].name, rolls[i]);
        } else {
            printf("  %s rolls: %d\n", players[playerIdx].name, rolls[i]);
        }
    }

    // 2. Collect the unique roll values
    int uniqueRolls[12]; // at most 11 unique values (2-12)
    int uniqueCount = 0;

    for (int i = 0; i < count; i++) {
        int found = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (uniqueRolls[j] == rolls[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            uniqueRolls[uniqueCount++] = rolls[i];
        }
    }

    // 3. Sort the unique rolls in descending order (highest first)
    for (int i = 0; i < uniqueCount - 1; i++) {
        for (int j = i + 1; j < uniqueCount; j++) {
            if (uniqueRolls[j] > uniqueRolls[i]) {
                int temp = uniqueRolls[i];
                uniqueRolls[i] = uniqueRolls[j];
                uniqueRolls[j] = temp;
            }
        }
    }

    // 4. Process each group from highest roll to lowest
    for (int g = 0; g < uniqueCount; g++) {
        int group[PLAYER_COUNT];
        int groupSize = 0;

        // Gather all candidates who rolled this value
        for (int i = 0; i < count; i++) {
            if (rolls[i] == uniqueRolls[g]) {
                group[groupSize++] = candidates[i];
            }
        }

        if (groupSize == 1) {
            // No tie — this player's position is locked in
            turnOrder[*resultPos] = group[0];
            (*resultPos)++;
        } else {
            // Tie — these players need to reroll among themselves
            printf("\n  Tie between ");
            for (int i = 0; i < groupSize; i++) {
                printf("%s", players[group[i]].name);
                if (i < groupSize - 2) {
                    printf(", ");
                } else if (i == groupSize - 2) {
                    printf(" and ");
                }
            }
            printf(" (both rolled %d)! Rerolling...\n", uniqueRolls[g]);
            rankPlayers(players, group, groupSize, resultPos, 1);
        }
    }
}

int firstDiceRoll(Player players[]) {
    printf("=== Determining Turn Order ===\n\n");

    // Start with all players as candidates
    int allPlayers[PLAYER_COUNT];
    for (int i = 0; i < PLAYER_COUNT; i++) {
        allPlayers[i] = i;
    }

    int resultPos = 0;
    rankPlayers(players, allPlayers, PLAYER_COUNT, &resultPos, 0);

    // Store each player's position (1st, 2nd, 3rd, 4th) in their struct
    for (int i = 0; i < PLAYER_COUNT; i++) {
        players[turnOrder[i]].turnOrder = i + 1;
    }

    // Print the final turn order
    printf("\n=== Final Turn Order ===\n");
    for (int i = 0; i < PLAYER_COUNT; i++) {
        printf("  %d. %s\n", i + 1, players[turnOrder[i]].name);
    }
    printf("\n");

    return turnOrder[0]; // Return index of the player who goes first
}