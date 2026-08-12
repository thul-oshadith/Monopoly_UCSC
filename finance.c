#include <stdio.h>
#include <string.h>
#include "types.h"

// Forward declaration of helper function
int checkIfCompletesSet(GameState *game, int playerIdx, PropertyGroup group);

// AI Purchase Decision — each player has their own brain
void attemptPurchase(GameState *game, int playerIdx, Square *sq, int purchasePrice, PropertyGroup group) {
    Player *player = &game->players[playerIdx];
    int buyDecision = 0;

    // --- AGGRESSIVE INVESTOR ---
    // Rule: "Always purchases if sufficient funds remain to pay at least one future rent"
    // We use 100 (the cheapest rent on the board) as the minimum rent threshold
    if (strcmp(player->name, "Aggressive Investor") == 0) {
        if (player->cash - purchasePrice >= 100) {
            buyDecision = 1;
        }
    }

    // --- CONSERVATIVE BANKER ---
    // Rule: "Purchases properties only if at least 50% of current cash remains after purchase"
    // So: purchasePrice must be <= half of their current cash
    else if (strcmp(player->name, "Conservative Banker") == 0) {
        if (purchasePrice <= player->cash / 2) {
            buyDecision = 1;
        }
    }

    // --- RISK TAKER ---
    // Rule: "Purchases every available property whenever legally possible"
    // Simply: buy if they have the cash
    else if (strcmp(player->name, "Risk Taker") == 0) {
        if (player->cash >= purchasePrice) {
            buyDecision = 1;
        }
    }

    // --- OPPORTUNISTIC TRADER ---
    // Rule: "Purchases only when projected appreciation exceeds construction costs"
    // Placeholder: Buy if it completes a color set, OR if 30% of cash remains after purchase
    // (We will refine this once the economic event system is built)
    else if (strcmp(player->name, "Opportunistic Trader") == 0) {
        int completesSet = 0;
        if (sq->type == SQUARE_PROPERTY) {
            completesSet = checkIfCompletesSet(game, playerIdx, group);
        }
        
        if (completesSet) {
            if (player->cash >= purchasePrice) {
                buyDecision = 1;
            }
        } else if (player->cash - purchasePrice > player->cash * 3 / 10) {
            buyDecision = 1;
        }
    }

    // EXECUTE THE DECISION
    if (buyDecision) {
        player->cash -= purchasePrice;

        if (sq->type == SQUARE_PROPERTY)  sq->data.property.owner = playerIdx;
        else if (sq->type == SQUARE_RAILWAY) sq->data.railway.owner = playerIdx;
        else if (sq->type == SQUARE_UTILITY) sq->data.utility.owner = playerIdx;

        printf("  >> %s bought %s for LKR %d\n", player->name, sq->name, purchasePrice);
    } else {
        printf("  >> %s declined to buy %s. (Going to Auction)\n", player->name, sq->name);
        // TODO: implement auction logic here later
    }
}


// Helper: Processes payments and handles bankruptcy if a player cannot pay
void payAmount(GameState *game, int payerIdx, int payeeIdx, int amount) {
    Player *payer = &game->players[payerIdx];
    
    // Does the player have enough cash?
    if (payer->cash >= amount) {
        payer->cash -= amount;
        
        // If they are paying another player (not the bank), give the payee the cash
        if (payeeIdx != -1) {
            game->players[payeeIdx].cash += amount;
        }
    } else {
        // Player cannot afford the payment! (We will add Mortgage logic here later)
        // For now, they go Bankrupt immediately.
        
        payer->bankrupt = 1;
        payer->cash = 0; // Wipe their remaining cash
        printf("\n  %s is BANKRUPT!\n", payer->name);
        
        // Return all their properties to the Bank
        for (int i = 0; i < SQUARE_COUNT; i++) {
            if (game->board[i].type == SQUARE_PROPERTY && game->board[i].data.property.owner == payerIdx) {
                game->board[i].data.property.owner = -1;
                game->board[i].data.property.mortgaged = 0; // Reset mortgage status
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            } 
            else if (game->board[i].type == SQUARE_RAILWAY && game->board[i].data.railway.owner == payerIdx) {
                game->board[i].data.railway.owner = -1;
                game->board[i].data.railway.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            } 
            else if (game->board[i].type == SQUARE_UTILITY && game->board[i].data.utility.owner == payerIdx) {
                game->board[i].data.utility.owner = -1;
                game->board[i].data.utility.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (TODO: Auction)\n", game->board[i].name);
            }
        }
    }
}
