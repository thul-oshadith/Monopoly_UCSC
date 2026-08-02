#include <stdio.h>
#include "types.h"


    Square board[SQUARE_COUNT] = {
        
    {0,  "GO",                                  SQUARE_GO,         -1},
    {1,  "Pettah",                              SQUARE_PROPERTY,    0},
    {2,  "Community Development Fund",          SQUARE_EVENT,      -1},
    {3,  "Maradana",                            SQUARE_PROPERTY,    1},
    {4,  "Income Tax",                          SQUARE_TAX,        -1},
    {5,  "Colombo Fort Railway Station",        SQUARE_RAILWAY,     0},
    {6,  "Bambalapitiya",                       SQUARE_PROPERTY,    2},
    {7,  "National Event Card",                 SQUARE_EVENT,      -1},
    {8,  "Wellawatte",                          SQUARE_PROPERTY,    3},
    {9,  "Mount Lavinia",                       SQUARE_PROPERTY,    4},
    {10, "Jail / Just Visiting",                SQUARE_JAIL,       -1},
    {11, "Nugegoda",                            SQUARE_PROPERTY,    5},
    {12, "Ceylon Electricity Board",            SQUARE_UTILITY,     0},
    {13, "Maharagama",                          SQUARE_PROPERTY,    6},
    {14, "Kottawa",                             SQUARE_PROPERTY,    7},
    {15, "Kandy Railway Station",               SQUARE_RAILWAY,     1},
    {16, "Negombo",                             SQUARE_PROPERTY,    8},
    {17, "Sri Lanka Insurance",                 SQUARE_INSURANCE,   0},
    {18, "Katunayake",                          SQUARE_PROPERTY,    9},
    {19, "Ja-Ela",                              SQUARE_PROPERTY,   10},
    {20, "Free Parking",                        SQUARE_SPECIAL,    -1},
    {21, "Kandy City",                          SQUARE_PROPERTY,   11},
    {22, "National Event Card",                 SQUARE_EVENT,      -1},
    {23, "Peradeniya",                          SQUARE_PROPERTY,   12},
    {24, "Katugastota",                         SQUARE_PROPERTY,   13},
    {25, "Galle Railway Station",               SQUARE_RAILWAY,     2},
    {26, "Galle Fort",                          SQUARE_PROPERTY,   14},
    {27, "Unawatuna",                           SQUARE_PROPERTY,   15},
    {28, "National Water Supply and Drainage Board", SQUARE_UTILITY, 1},
    {29, "Hikkaduwa",                           SQUARE_PROPERTY,   16},
    {30, "Go To Jail",                          SQUARE_SPECIAL,    -1},
    {31, "Jaffna Town",                         SQUARE_PROPERTY,   17},
    {32, "Nallur",                              SQUARE_PROPERTY,   18},
    {33, "Ceylinco Insurance",                  SQUARE_INSURANCE,   1},
    {34, "Trincomalee",                         SQUARE_PROPERTY,   19},
    {35, "Jaffna Railway Station",              SQUARE_RAILWAY,     3},
    {36, "National Event Card",                 SQUARE_EVENT,      -1},
    {37, "Nuwara Eliya",                        SQUARE_PROPERTY,   20},
    {38, "Bank of Ceylon",                      SQUARE_BANK,       -1},
    {39, "Galle Face",                          SQUARE_PROPERTY,   21}
    };

void print_board() {                                                    // For testing
    for (int i = 0; i < SQUARE_COUNT; i++) {
        printf("Square %d: %s\n", board[i].index, board[i].name);
        printf("Type: %d\n", board[i].type);
        printf("Data Index: %d\n", board[i].dataIndex);
        printf("\n");
    }
}

int main(){                                // For tetsting
    print_board();
    return 0;
}
    



































/*void init_board(BoardSquare board[]) {
    for(int i = 0; i < SQUARE_TOTAL; i++) {
        board[i].id = i;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;

        strcpy(board[i].name, "Go");
        board[0].type = SQUARE_GO;



    
    }         
}                              

int main(){
    BoardSquare board[SQUARE_TOTAL];
    init_board(board);
    return 0;

    
}*/