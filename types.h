#ifndef TYPES_H
#define TYPES_H

#define SQUARE_COUNT 40
#define PLAYER_COUNT 4
#define PROPERTY_COUNT 22
#define RAILWAY_COUNT 4
#define UTILITY_COUNT 2
#define STARTING_CASH 30000

typedef enum SquareType{
    SQUARE_PROPERTY,
    SQUARE_RAILWAY,
    SQUARE_UTILITY,
    SQUARE_BANK,
    SQUARE_INSURANCE,
    SQUARE_TAX,
    SQUARE_EVENT,
    SQUARE_SPECIAL,
    SQUARE_JAIL,
    SQUARE_GO
} SquareType;

typedef enum PropertyGroup{
     BROWN,
     LIGHT_BLUE,
     PINK,
     ORANGE,
     RED,
     YELLOW,
     GREEN,
     DARK_BLUE,
     NO_GROUP
} PropertyGroup;

typedef enum InsuranceType{
      NONE,
      BASIC,
      COMPREHENSIVE,
      BUSINESS_INTERRUPTION
} InsuranceType;

typedef struct Player{
    char name[30];
    int position;
    int cash;
    int inJail;
    int jailTurns;
    int bankrupt;
    int turnOrder;

}Player;

typedef struct Property{
    char name[40];
    PropertyGroup group;

    int purchasePrice;
    int mortgageValue;
    int rent;

    int houseCost;
    int hotelCost;

    int owner;

    int mortgaged;

    InsuranceType insurance;

    int houses;
    int hotel;
}Property;

typedef struct Railway{
    char name[40];

    int owner;
    int mortgaged;
}Railway;

typedef struct Utility{
    char name[50];

    int owner;
    int mortgaged;
}Utility;

typedef struct Square{
    int index;
    char name[50];
    SquareType type;
    
    union {
        Property property;
        Railway railway;
        Utility utility;
        int taxAmount;
        int jailBailAmount;
    } data;
}Square;

typedef struct Loan{
    int borrower;
    int amount;
    float interestRate;
    int remainingRounds;
    int active;
}Loan;

typedef struct Policy{
    int propertyID;
    InsuranceType type;
    int remainingRounds;
    int active;
} Policy;

typedef struct EventCard{
    char description[100];

}EventCard;

typedef struct RegionalCard{
    char description[100];
    int duration;
}RegionalCard;

typedef struct GameState{
    Square board[SQUARE_COUNT];
    Player players[PLAYER_COUNT];
    int currentRound;
    int currentPlayerIndex;
    double bankMoney;
    
}GameState;


#endif
