/*
 * Sri Lankan Monopoly - Determine Starting Turn Order
 * -----------------------------------------------------
 * Rules implemented:
 *  - 4 players each roll two dice and their total is compared.
 *  - The player with the highest total goes first, and so on in
 *    descending order.
 *  - If two or more players get the SAME total, only those tied
 *    players roll again to break the tie.
 *  - Players who got a UNIQUE total on their first roll keep the
 *    position that total earns them (they don't have to re-roll
 *    even if other players are still re-rolling to break a tie).
 *
 * Approach:
 *  We solve this recursively. Given a set of players that need to be
 *  ordered, everyone in that set rolls two dice. Totals are grouped.
 *  Any player with a total that no one else in the CURRENT group has
 *  is immediately placed in the final order. Any group of players who
 *  tied re-rolls again, but only among themselves, and the same
 *  process repeats for just that smaller group.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PLAYERS 4

typedef struct {
    int id;      /* player number (1-4)          */
    int total;   /* dice total for current round */
} Player;

/* Rolls a single six-sided die -> returns 1 to 6 */
int rollDie(void) {
    return (rand() % 6) + 1;
}

/* Rolls two dice for a player, prints the individual dice, returns total */
int rollTwoDice(int playerId) {
    int d1 = rollDie();
    int d2 = rollDie();
    printf("Player %d rolls: %d + %d = %d\n", playerId, d1, d2, d1 + d2);
    return d1 + d2;
}

/*
 * Recursively determines turn order for a set of players.
 *
 * ids[]     : array of player ids that still need to be ordered
 * n         : number of players in ids[]
 * order[]   : output array where the resolved order is appended
 * pos       : pointer to the current write index into order[]
 * roundNum  : just used for nicer printed output
 */
void resolveOrder(int ids[], int n, int order[], int *pos, int roundNum) {
    /* Base case: only one player left in this group -> their position is fixed */
    if (n == 1) {
        order[(*pos)++] = ids[0];
        return;
    }

    printf("\n--- Round %d: %d player(s) rolling ---\n", roundNum, n);

    Player players[MAX_PLAYERS];
    for (int i = 0; i < n; i++) {
        players[i].id = ids[i];
        players[i].total = rollTwoDice(ids[i]);
    }

    /* Find the distinct totals rolled this round */
    int totals[MAX_PLAYERS];
    int totalCount = 0;
    for (int i = 0; i < n; i++) {
        int t = players[i].total;
        int found = 0;
        for (int j = 0; j < totalCount; j++) {
            if (totals[j] == t) { found = 1; break; }
        }
        if (!found) {
            totals[totalCount++] = t;
        }
    }

    /* Sort the distinct totals in descending order (highest goes first) */
    for (int i = 0; i < totalCount - 1; i++) {
        for (int j = i + 1; j < totalCount; j++) {
            if (totals[j] > totals[i]) {
                int tmp = totals[i];
                totals[i] = totals[j];
                totals[j] = tmp;
            }
        }
    }

    /* Process each total, highest first */
    for (int t = 0; t < totalCount; t++) {
        int group[MAX_PLAYERS];
        int groupCount = 0;

        for (int i = 0; i < n; i++) {
            if (players[i].total == totals[t]) {
                group[groupCount++] = players[i].id;
            }
        }

        if (groupCount == 1) {
            /* Unique total this round -> position is locked in immediately */
            printf("Player %d has a unique total of %d -> position confirmed.\n",
                   group[0], totals[t]);
            order[(*pos)++] = group[0];
        } else {
            /* Tie -> only these players roll again */
            printf("Tie at total %d between players:", totals[t]);
            for (int i = 0; i < groupCount; i++) {
                printf(" %d", group[i]);
            }
            printf(" -> they must roll again.\n");
            resolveOrder(group, groupCount, order, pos, roundNum + 1);
        }
    }
}

int main(void) {
    srand((unsigned int) time(NULL));

    int ids[MAX_PLAYERS] = {1, 2, 3, 4};
    int order[MAX_PLAYERS];
    int pos = 0;

    printf("=== Sri Lankan Monopoly - Determining Turn Order ===\n");

    resolveOrder(ids, MAX_PLAYERS, order, &pos, 1);

    printf("\n=== Final Turn Order ===\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("%d. Player %d\n", i + 1, order[i]);
    }

    return 0;
}