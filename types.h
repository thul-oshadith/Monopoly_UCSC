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

typedef struct Loan{
    int amount;
    float interestRate;
    int remainingRounds;
    int active;
}Loan;

typedef enum EconomicEvent{
    EVENT_NONE,
    EVENT_TOURISM_BOOM,
    EVENT_FUEL_CRISIS,
    EVENT_HEAVY_MONSOON,
    EVENT_ECONOMIC_RECESSION,
    EVENT_STOCK_MARKET_BOOM,
    EVENT_GOVERNMENT_HOUSING,
    EVENT_FOREIGN_INVESTMENT,
    EVENT_POLITICAL_UNREST
} EconomicEvent;

typedef enum GovernmentRegulation{
    REG_NONE,
    REG_INCREASE_PROPERTY_TAX,
    REG_REDUCE_LOAN_INTEREST,
    REG_HOUSING_SUBSIDY,
    REG_LUXURY_PROPERTY_TAX,
    REG_RAILWAY_MODERNIZATION,
    REG_ELECTRICITY_TARIFF_REVISION,
    REG_INSURANCE_REGULATION,
    REG_ANTI_SPECULATION_ACT
} GovernmentRegulation;

typedef struct Player{
    char name[30];
    int position;
    int cash;
    int inJail;
    int jailTurns;
    int bankrupt;
    int turnOrder;
    Loan activeLoan; // Added active loan
    int hasSufferedLoss; // Set to 1 if player suffers an uninsured loss
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
    int loanLocked; // Added loan lock status

    int isCoastal;
    int isSouthernCoastal;
    int isCommercial;

    InsuranceType insurance;
    int insuranceRoundsRemaining;

    int houses;
    int hotel;

    int buildingCondition;
    int roundsUnmaintained;
    int hasStructuralDamage;
    int isDisasterDamaged;
    int propertyAge;
}Property;

typedef struct Railway{
    char name[40];
    int purchasePrice;
    int mortgageValue;
    int owner;
    int mortgaged;
    int loanLocked; // Added loan lock status
}Railway;

typedef struct Utility{
    char name[50];
    int purchasePrice;
    int mortgageValue;
    int owner;
    int mortgaged;
    int loanLocked; // Added loan lock status
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
    int turnOrder[PLAYER_COUNT];
    
    EconomicEvent currentEvent;
    GovernmentRegulation currentRegulation;
    
    PropertyGroup currentBoomGroup;
    PropertyGroup currentDeclineGroup;
    int boomCooldowns[8];    // Cooldown before a group can Boom again
    int declineCooldowns[8]; // Cooldown before a group can Decline again
    
    float currentInflationRate;
    float currentLoanInterestRate;

}GameState;

#endif
