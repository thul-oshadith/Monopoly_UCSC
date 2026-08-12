#include <stdio.h>
#include <string.h>
#include "types.h"

// Helper: checks if buying this property would complete a color set for the player
int checkIfCompletesSet(GameState *game, int playerIdx, PropertyGroup group) {
    if (group == NO_GROUP) return 0;
    
    int totalInGroup = 0;
    int ownedByPlayer = 0;
    
    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (game->board[i].type == SQUARE_PROPERTY) {
            Property *p = &game->board[i].data.property;
            if (p->group == group) {
                totalInGroup++;
                if (p->owner == playerIdx) {
                    ownedByPlayer++;
                }
            }
        }
    }
    
    // If buying THIS property makes owned == total, set is complete
    if (ownedByPlayer + 1 == totalInGroup) {
        return 1; // Yes, it completes the set
    } else {
        return 0; // No, set is not complete
    }
}
