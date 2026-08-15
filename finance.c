#include <stdio.h>
#include <string.h>
#include "types.h"

// Forward declaration of helper functions
int checkIfCompletesSet(GameState *game, int playerIdx, PropertyGroup group);
void startAuction(GameState *game, int sqIdx);

int applyEventValueModifier(GameState *game, int sqIdx, int value);
float applyEventLoanInterest(GameState *game, float baseInterest);

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
        startAuction(game, sq->index);
    }
}

// Executes an auction for an unowned property
void startAuction(GameState *game, int sqIdx) {
    Square *sq = &game->board[sqIdx];
    int marketValue = 0;
    
    if (sq->type == SQUARE_PROPERTY) marketValue = sq->data.property.purchasePrice;
    else if (sq->type == SQUARE_RAILWAY) marketValue = sq->data.railway.purchasePrice;
    else if (sq->type == SQUARE_UTILITY) marketValue = sq->data.utility.purchasePrice;
    
    if (marketValue == 0) return; // Should not happen
    
    printf("\n  🔨 AUCTION STARTED for %s (Market Value: LKR %d)\n", sq->name, marketValue);
    
    int activeBidders[PLAYER_COUNT];
    int activeCount = 0;
    
    // Everyone except bankrupt players starts in the auction
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (game->players[i].bankrupt) {
            activeBidders[i] = 0;
        } else {
            activeBidders[i] = 1;
            activeCount++;
        }
    }
    
    int currentBid = marketValue / 2; // Starts at 50%
    int highestBidder = -1;
    
    // We will keep looping until 1 or fewer bidders remain
    int currentPlayer = 0; // Index of the player currently being asked
    
    while (activeCount > 1 || (activeCount == 1 && highestBidder == -1)) {
        if (!activeBidders[currentPlayer]) {
            currentPlayer = (currentPlayer + 1) % PLAYER_COUNT;
            continue;
        }
        
        Player *p = &game->players[currentPlayer];
        
        // Calculate what their bid has to be to stay in
        int proposedBid = (highestBidder == -1) ? currentBid : currentBid + 250;
        int willingToBid = 0;
        
        // --- AI BIDDING RULES ---
        if (p->cash >= proposedBid) {
            if (strcmp(p->name, "Aggressive Investor") == 0) {
                if (proposedBid <= marketValue * 1.2) willingToBid = 1;
            } 
            else if (strcmp(p->name, "Conservative Banker") == 0) {
                if (proposedBid < marketValue) willingToBid = 1;
            } 
            else if (strcmp(p->name, "Risk Taker") == 0) {
                willingToBid = 1; // Bids until cash exhausted
            } 
            else if (strcmp(p->name, "Opportunistic Trader") == 0) {
                if (proposedBid <= marketValue * 0.9) willingToBid = 1; // 10% discount cap
            }
        }
        
        if (willingToBid) {
            currentBid = proposedBid;
            highestBidder = currentPlayer;
            printf("    %s bids LKR %d\n", p->name, currentBid);
        } else {
            activeBidders[currentPlayer] = 0;
            activeCount--;
            printf("    %s withdraws from the auction.\n", p->name);
        }
        
        currentPlayer = (currentPlayer + 1) % PLAYER_COUNT;
    }
    
    if (highestBidder != -1) {
        Player *winner = &game->players[highestBidder];
        winner->cash -= currentBid;
        
        if (sq->type == SQUARE_PROPERTY)  sq->data.property.owner = highestBidder;
        else if (sq->type == SQUARE_RAILWAY) sq->data.railway.owner = highestBidder;
        else if (sq->type == SQUARE_UTILITY) sq->data.utility.owner = highestBidder;
        
        printf("  SOLD! %s wins the auction for %s at LKR %d\n\n", winner->name, sq->name, currentBid);
    } else {
        printf("  No one bid. Ownership remains with the Bank.\n\n");
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
                printf("  >> %s is returned to the Bank! (Auctioning...)\n", game->board[i].name);
                startAuction(game, i);
            } 
            else if (game->board[i].type == SQUARE_RAILWAY && game->board[i].data.railway.owner == payerIdx) {
                game->board[i].data.railway.owner = -1;
                game->board[i].data.railway.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (Auctioning...)\n", game->board[i].name);
                startAuction(game, i);
            } 
            else if (game->board[i].type == SQUARE_UTILITY && game->board[i].data.utility.owner == payerIdx) {
                game->board[i].data.utility.owner = -1;
                game->board[i].data.utility.mortgaged = 0;
                printf("  >> %s is returned to the Bank! (Auctioning...)\n", game->board[i].name);
                startAuction(game, i);
            }
        }
    }
}

// Calculate max loan available for player based on eligible un-locked, un-mortgaged collateral
int calculateMaxLoan(GameState *game, int playerIdx) {
    int totalMortgageValue = 0;
    for (int i = 0; i < SQUARE_COUNT; i++) {
        Square *sq = &game->board[i];
        if (sq->type == SQUARE_PROPERTY && sq->data.property.owner == playerIdx && !sq->data.property.mortgaged && !sq->data.property.loanLocked) {
            totalMortgageValue += applyEventValueModifier(game, i, sq->data.property.mortgageValue);
        } else if (sq->type == SQUARE_RAILWAY && sq->data.railway.owner == playerIdx && !sq->data.railway.mortgaged && !sq->data.railway.loanLocked) {
            totalMortgageValue += applyEventValueModifier(game, i, sq->data.railway.mortgageValue);
        } else if (sq->type == SQUARE_UTILITY && sq->data.utility.owner == playerIdx && !sq->data.utility.mortgaged && !sq->data.utility.loanLocked) {
            totalMortgageValue += applyEventValueModifier(game, i, sq->data.utility.mortgageValue);
        }
    }
    return (totalMortgageValue * 75) / 100;
}

// Lock properties until amount is reached
void takeLoan(GameState *game, int playerIdx, int amountToBorrow) {
    Player *p = &game->players[playerIdx];
    int collateralNeeded = (amountToBorrow * 100) / 75; 
    int collateralLocked = 0;

    for (int i = 0; i < SQUARE_COUNT; i++) {
        if (collateralLocked >= collateralNeeded) break;

        Square *sq = &game->board[i];
        if (sq->type == SQUARE_PROPERTY && sq->data.property.owner == playerIdx && !sq->data.property.mortgaged && !sq->data.property.loanLocked) {
            sq->data.property.loanLocked = 1;
            collateralLocked += applyEventValueModifier(game, i, sq->data.property.mortgageValue);
        } else if (sq->type == SQUARE_RAILWAY && sq->data.railway.owner == playerIdx && !sq->data.railway.mortgaged && !sq->data.railway.loanLocked) {
            sq->data.railway.loanLocked = 1;
            collateralLocked += applyEventValueModifier(game, i, sq->data.railway.mortgageValue);
        } else if (sq->type == SQUARE_UTILITY && sq->data.utility.owner == playerIdx && !sq->data.utility.mortgaged && !sq->data.utility.loanLocked) {
            sq->data.utility.loanLocked = 1;
            collateralLocked += applyEventValueModifier(game, i, sq->data.utility.mortgageValue);
        }
    }

    p->activeLoan.amount = amountToBorrow;
    p->activeLoan.interestRate = applyEventLoanInterest(game, 0.08f); // 8% default modified by events
    p->activeLoan.remainingRounds = 20;
    p->activeLoan.active = 1;
    p->cash += amountToBorrow;
    printf("  >> %s took a loan of LKR %d (20 rounds at %.0f%%).\n", p->name, amountToBorrow, p->activeLoan.interestRate * 100);
}

// Unlocks properties
void unlockCollateral(GameState *game, int playerIdx) {
    for (int i = 0; i < SQUARE_COUNT; i++) {
        Square *sq = &game->board[i];
        if (sq->type == SQUARE_PROPERTY && sq->data.property.owner == playerIdx) {
            sq->data.property.loanLocked = 0;
        } else if (sq->type == SQUARE_RAILWAY && sq->data.railway.owner == playerIdx) {
            sq->data.railway.loanLocked = 0;
        } else if (sq->type == SQUARE_UTILITY && sq->data.utility.owner == playerIdx) {
            sq->data.utility.loanLocked = 0;
        }
    }
}

// Repay the loan
void repayLoan(GameState *game, int playerIdx) {
    Player *p = &game->players[playerIdx];
    if (!p->activeLoan.active) return;

    if (p->cash >= p->activeLoan.amount) {
        p->cash -= p->activeLoan.amount;
        printf("  >> %s repaid their loan of LKR %d!\n", p->name, p->activeLoan.amount);
        p->activeLoan.active = 0;
        p->activeLoan.amount = 0;
        unlockCollateral(game, playerIdx);
    }
}

// Handle Bank Square logic (AI Behaviors)
void handleBankSquare(GameState *game, int playerIdx) {
    Player *p = &game->players[playerIdx];
    int maxLoan = calculateMaxLoan(game, playerIdx);
    
    if (strcmp(p->name, "Aggressive Investor") == 0) {
        if (p->activeLoan.active) {
            if (p->cash > (2 * p->activeLoan.amount)) {
                repayLoan(game, playerIdx);
            }
        } else {
            if (p->cash < 500 && maxLoan >= 1000) {
                takeLoan(game, playerIdx, maxLoan);
            }
        }
    } else if (strcmp(p->name, "Conservative Banker") == 0) {
        if (p->activeLoan.active) {
            if (p->cash >= p->activeLoan.amount) {
                repayLoan(game, playerIdx);
            }
        } else {
            if (p->cash < 200 && maxLoan > 0) {
                takeLoan(game, playerIdx, maxLoan);
            }
        }
    } else if (strcmp(p->name, "Risk Taker") == 0) {
        if (p->activeLoan.active) {
            unlockCollateral(game, playerIdx);
            int newMax = calculateMaxLoan(game, playerIdx);
            if (newMax > p->activeLoan.amount) {
                printf("  >> %s refinanced their loan!\n", p->name);
                int difference = newMax - p->activeLoan.amount;
                p->cash += difference;
                takeLoan(game, playerIdx, newMax); // Overwrites old loan
            } else {
                takeLoan(game, playerIdx, p->activeLoan.amount); // Re-lock
            }
        } else {
            if (maxLoan > 0) {
                takeLoan(game, playerIdx, maxLoan);
            }
        }
    } else if (strcmp(p->name, "Opportunistic Trader") == 0) {
        // To be provided later
    }
}

// Process loans at the end of every complete round
void processEndRoundLoans(GameState *game) {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        Player *p = &game->players[i];
        if (p->bankrupt) continue;

        if (p->activeLoan.active) {
            // Add interest (Rule-LK 4)
            int interest = (int)(p->activeLoan.amount * p->activeLoan.interestRate);
            p->activeLoan.amount += interest;
            p->activeLoan.remainingRounds--;

            // Default check (Rule-LK 6)
            if (p->activeLoan.remainingRounds <= 0) {
                printf("\n======================================================\n");
                printf("  >> LOAN DEFAULT: %s failed to repay their loan within the duration!\n", p->name);
                
                int hasRemainingAssets = 0;
                // Foreclose pledged assets and check for remaining assets
                for (int j = 0; j < SQUARE_COUNT; j++) {
                    Square *sq = &game->board[j];
                    if (sq->type == SQUARE_PROPERTY && sq->data.property.owner == i) {
                        if (sq->data.property.loanLocked) {
                            sq->data.property.owner = -1;
                            sq->data.property.loanLocked = 0;
                            sq->data.property.houses = 0; // Demolished
                            sq->data.property.hotel = 0;
                            sq->data.property.insurance = NONE;
                        } else {
                            hasRemainingAssets = 1;
                        }
                    } else if (sq->type == SQUARE_RAILWAY && sq->data.railway.owner == i) {
                        if (sq->data.railway.loanLocked) {
                            sq->data.railway.owner = -1;
                            sq->data.railway.loanLocked = 0;
                        } else {
                            hasRemainingAssets = 1;
                        }
                    } else if (sq->type == SQUARE_UTILITY && sq->data.utility.owner == i) {
                        if (sq->data.utility.loanLocked) {
                            sq->data.utility.owner = -1;
                            sq->data.utility.loanLocked = 0;
                        } else {
                            hasRemainingAssets = 1;
                        }
                    }
                }

                // Outstanding debt is cleared
                p->activeLoan.active = 0;
                p->activeLoan.amount = 0;
                printf("  >> All Loan Locked properties foreclosed to the Bank. Outstanding debt cleared.\n");

                // Check for bankruptcy (Rule-LK 7)
                if (p->cash <= 0 && !hasRemainingAssets) {
                    p->bankrupt = 1;
                    printf("  >> %s has no remaining assets and is declared BANKRUPT!\n", p->name);
                } else {
                    printf("  >> %s continues the game using remaining assets.\n", p->name);
                }
                printf("======================================================\n\n");
            }
        }
    }
}
