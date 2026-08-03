#include <stdio.h>
#include <string.h>
#include "types.h"

void init_Board(GameState *game) {
    //Important: Base rent is an assumption
    // ---------------------------------------------------------------------
    // Index 0: GO
    // ---------------------------------------------------------------------
    game->board[0].index = 0;
    game->board[0].type = SQUARE_GO;
    strcpy(game->board[0].name, "GO");

    // ---------------------------------------------------------------------
    // Index 1: Pettah
    // ---------------------------------------------------------------------
    game->board[1].index = 1;
    game->board[1].type = SQUARE_PROPERTY;
    strcpy(game->board[1].name, "Pettah");

    strcpy(game->board[1].data.property.name, "Pettah");
    game->board[1].data.property.group = BROWN;
    game->board[1].data.property.purchasePrice = 1500;
    game->board[1].data.property.mortgageValue = 750;
    game->board[1].data.property.rent = 0;
    game->board[1].data.property.houseCost = 500;
    game->board[1].data.property.hotelCost = 2000;
    game->board[1].data.property.owner = -1;
    game->board[1].data.property.mortgaged = 0;
    game->board[1].data.property.insurance = NONE;
    game->board[1].data.property.houses = 0;
    game->board[1].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 2: Community Development Fund
    // ---------------------------------------------------------------------
    game->board[2].index = 2;
    game->board[2].type = SQUARE_EVENT;
    strcpy(game->board[2].name, "Community Development Fund");

    // ---------------------------------------------------------------------
    // Index 3: Maradana
    // ---------------------------------------------------------------------
    game->board[3].index = 3;
    game->board[3].type = SQUARE_PROPERTY;
    strcpy(game->board[3].name, "Maradana");

    strcpy(game->board[3].data.property.name, "Maradana");
    game->board[3].data.property.group = BROWN;
    game->board[3].data.property.purchasePrice = 1500;
    game->board[3].data.property.mortgageValue = 750;
    game->board[3].data.property.rent = 0;
    game->board[3].data.property.houseCost = 500;
    game->board[3].data.property.hotelCost = 2000;
    game->board[3].data.property.owner = -1;
    game->board[3].data.property.mortgaged = 0;
    game->board[3].data.property.insurance = NONE;
    game->board[3].data.property.houses = 0;
    game->board[3].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 4: Income Tax
    // ---------------------------------------------------------------------
    game->board[4].index = 4;
    game->board[4].type = SQUARE_TAX;
    strcpy(game->board[4].name, "Income Tax");

    game->board[4].data.taxAmount = 0;

    // ---------------------------------------------------------------------
    // Index 5: Colombo Fort Railway Station
    // ---------------------------------------------------------------------
    game->board[5].index = 5;
    game->board[5].type = SQUARE_RAILWAY;
    strcpy(game->board[5].name, "Colombo Fort Railway Station");

    strcpy(game->board[5].data.railway.name, "Colombo Fort Railway Station");
    game->board[5].data.railway.owner = -1;
    game->board[5].data.railway.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 6: Bambalapitiya
    // ---------------------------------------------------------------------
    game->board[6].index = 6;
    game->board[6].type = SQUARE_PROPERTY;
    strcpy(game->board[6].name, "Bambalapitiya");

    strcpy(game->board[6].data.property.name, "Bambalapitiya");
    game->board[6].data.property.group = LIGHT_BLUE;
    game->board[6].data.property.purchasePrice = 2500;
    game->board[6].data.property.mortgageValue = 1250;
    game->board[6].data.property.rent = 0;
    game->board[6].data.property.houseCost = 750;
    game->board[6].data.property.hotelCost = 3000;
    game->board[6].data.property.owner = -1;
    game->board[6].data.property.mortgaged = 0;
    game->board[6].data.property.insurance = NONE;
    game->board[6].data.property.houses = 0;
    game->board[6].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 7: National Event Card
    // ---------------------------------------------------------------------
    game->board[7].index = 7;
    game->board[7].type = SQUARE_EVENT;
    strcpy(game->board[7].name, "National Event Card");

    // ---------------------------------------------------------------------
    // Index 8: Wellawatte
    // ---------------------------------------------------------------------
    game->board[8].index = 8;
    game->board[8].type = SQUARE_PROPERTY;
    strcpy(game->board[8].name, "Wellawatte");

    strcpy(game->board[8].data.property.name, "Wellawatte");
    game->board[8].data.property.group = LIGHT_BLUE;
    game->board[8].data.property.purchasePrice = 2500;
    game->board[8].data.property.mortgageValue = 1250;
    game->board[8].data.property.rent = 0;
    game->board[8].data.property.houseCost = 750;
    game->board[8].data.property.hotelCost = 3000;
    game->board[8].data.property.owner = -1;
    game->board[8].data.property.mortgaged = 0;
    game->board[8].data.property.insurance = NONE;
    game->board[8].data.property.houses = 0;
    game->board[8].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 9: Mount Lavinia
    // ---------------------------------------------------------------------
    game->board[9].index = 9;
    game->board[9].type = SQUARE_PROPERTY;
    strcpy(game->board[9].name, "Mount Lavinia");

    strcpy(game->board[9].data.property.name, "Mount Lavinia");
    game->board[9].data.property.group = LIGHT_BLUE;
    game->board[9].data.property.purchasePrice = 2500;
    game->board[9].data.property.mortgageValue = 1250;
    game->board[9].data.property.rent = 0;
    game->board[9].data.property.houseCost = 750;
    game->board[9].data.property.hotelCost = 3000;
    game->board[9].data.property.owner = -1;
    game->board[9].data.property.mortgaged = 0;
    game->board[9].data.property.insurance = NONE;
    game->board[9].data.property.houses = 0;
    game->board[9].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 10: Jail / Just Visiting
    // ---------------------------------------------------------------------
    game->board[10].index = 10;
    game->board[10].type = SQUARE_JAIL;
    strcpy(game->board[10].name, "Jail / Just Visiting");

    game->board[10].data.jailBailAmount = 300;

    // ---------------------------------------------------------------------
    // Index 11: Nugegoda
    // ---------------------------------------------------------------------
    game->board[11].index = 11;
    game->board[11].type = SQUARE_PROPERTY;
    strcpy(game->board[11].name, "Nugegoda");

    strcpy(game->board[11].data.property.name, "Nugegoda");
    game->board[11].data.property.group = PINK;
    game->board[11].data.property.purchasePrice = 3500;
    game->board[11].data.property.mortgageValue = 1750;
    game->board[11].data.property.rent = 0;
    game->board[11].data.property.houseCost = 1000;
    game->board[11].data.property.hotelCost = 4000;
    game->board[11].data.property.owner = -1;
    game->board[11].data.property.mortgaged = 0;
    game->board[11].data.property.insurance = NONE;
    game->board[11].data.property.houses = 0;
    game->board[11].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 12: Ceylon Electricity Board
    // ---------------------------------------------------------------------
    game->board[12].index = 12;
    game->board[12].type = SQUARE_UTILITY;
    strcpy(game->board[12].name, "Ceylon Electricity Board");

    strcpy(game->board[12].data.utility.name, "Ceylon Electricity Board");
    game->board[12].data.utility.owner = -1;
    game->board[12].data.utility.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 13: Maharagama
    // ---------------------------------------------------------------------
    game->board[13].index = 13;
    game->board[13].type = SQUARE_PROPERTY;
    strcpy(game->board[13].name, "Maharagama");

    strcpy(game->board[13].data.property.name, "Maharagama");
    game->board[13].data.property.group = PINK;
    game->board[13].data.property.purchasePrice = 3500;
    game->board[13].data.property.mortgageValue = 1750;
    game->board[13].data.property.rent = 0;
    game->board[13].data.property.houseCost = 1000;
    game->board[13].data.property.hotelCost = 4000;
    game->board[13].data.property.owner = -1;
    game->board[13].data.property.mortgaged = 0;
    game->board[13].data.property.insurance = NONE;
    game->board[13].data.property.houses = 0;
    game->board[13].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 14: Kottawa
    // ---------------------------------------------------------------------
    game->board[14].index = 14;
    game->board[14].type = SQUARE_PROPERTY;
    strcpy(game->board[14].name, "Kottawa");

    strcpy(game->board[14].data.property.name, "Kottawa");
    game->board[14].data.property.group = PINK;
    game->board[14].data.property.purchasePrice = 3500;
    game->board[14].data.property.mortgageValue = 1750;
    game->board[14].data.property.rent = 0;
    game->board[14].data.property.houseCost = 1000;
    game->board[14].data.property.hotelCost = 4000;
    game->board[14].data.property.owner = -1;
    game->board[14].data.property.mortgaged = 0;
    game->board[14].data.property.insurance = NONE;
    game->board[14].data.property.houses = 0;
    game->board[14].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 15: Kandy Railway Station
    // ---------------------------------------------------------------------
    game->board[15].index = 15;
    game->board[15].type = SQUARE_RAILWAY;
    strcpy(game->board[15].name, "Kandy Railway Station");

    strcpy(game->board[15].data.railway.name, "Kandy Railway Station");
    game->board[15].data.railway.owner = -1;
    game->board[15].data.railway.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 16: Negombo
    // ---------------------------------------------------------------------
    game->board[16].index = 16;
    game->board[16].type = SQUARE_PROPERTY;
    strcpy(game->board[16].name, "Negombo");

    strcpy(game->board[16].data.property.name, "Negombo");
    game->board[16].data.property.group = ORANGE;
    game->board[16].data.property.purchasePrice = 4500;
    game->board[16].data.property.mortgageValue = 2250;
    game->board[16].data.property.rent = 0;
    game->board[16].data.property.houseCost = 1250;
    game->board[16].data.property.hotelCost = 5000;
    game->board[16].data.property.owner = -1;
    game->board[16].data.property.mortgaged = 0;
    game->board[16].data.property.insurance = NONE;
    game->board[16].data.property.houses = 0;
    game->board[16].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 17: Sri Lanka Insurance
    // ---------------------------------------------------------------------
    game->board[17].index = 17;
    game->board[17].type = SQUARE_INSURANCE;
    strcpy(game->board[17].name, "Sri Lanka Insurance");

    // ---------------------------------------------------------------------
    // Index 18: Katunayake
    // ---------------------------------------------------------------------
    game->board[18].index = 18;
    game->board[18].type = SQUARE_PROPERTY;
    strcpy(game->board[18].name, "Katunayake");

    strcpy(game->board[18].data.property.name, "Katunayake");
    game->board[18].data.property.group = ORANGE;
    game->board[18].data.property.purchasePrice = 4500;
    game->board[18].data.property.mortgageValue = 2250;
    game->board[18].data.property.rent = 0;
    game->board[18].data.property.houseCost = 1250;
    game->board[18].data.property.hotelCost = 5000;
    game->board[18].data.property.owner = -1;
    game->board[18].data.property.mortgaged = 0;
    game->board[18].data.property.insurance = NONE;
    game->board[18].data.property.houses = 0;
    game->board[18].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 19: Ja-Ela
    // ---------------------------------------------------------------------
    game->board[19].index = 19;
    game->board[19].type = SQUARE_PROPERTY;
    strcpy(game->board[19].name, "Ja-Ela");

    strcpy(game->board[19].data.property.name, "Ja-Ela");
    game->board[19].data.property.group = ORANGE;
    game->board[19].data.property.purchasePrice = 4500;
    game->board[19].data.property.mortgageValue = 2250;
    game->board[19].data.property.rent = 0;
    game->board[19].data.property.houseCost = 1250;
    game->board[19].data.property.hotelCost = 5000;
    game->board[19].data.property.owner = -1;
    game->board[19].data.property.mortgaged = 0;
    game->board[19].data.property.insurance = NONE;
    game->board[19].data.property.houses = 0;
    game->board[19].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 20: Free Parking
    // ---------------------------------------------------------------------
    game->board[20].index = 20;
    game->board[20].type = SQUARE_SPECIAL;
    strcpy(game->board[20].name, "Free Parking");

    // ---------------------------------------------------------------------
    // Index 21: Kandy City
    // ---------------------------------------------------------------------
    game->board[21].index = 21;
    game->board[21].type = SQUARE_PROPERTY;
    strcpy(game->board[21].name, "Kandy City");

    strcpy(game->board[21].data.property.name, "Kandy City");
    game->board[21].data.property.group = RED;
    game->board[21].data.property.purchasePrice = 5500;
    game->board[21].data.property.mortgageValue = 2750;
    game->board[21].data.property.rent = 0;
    game->board[21].data.property.houseCost = 1500;
    game->board[21].data.property.hotelCost = 6000;
    game->board[21].data.property.owner = -1;
    game->board[21].data.property.mortgaged = 0;
    game->board[21].data.property.insurance = NONE;
    game->board[21].data.property.houses = 0;
    game->board[21].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 22: National Event Card
    // ---------------------------------------------------------------------
    game->board[22].index = 22;
    game->board[22].type = SQUARE_EVENT;
    strcpy(game->board[22].name, "National Event Card");

    // ---------------------------------------------------------------------
    // Index 23: Peradeniya
    // ---------------------------------------------------------------------
    game->board[23].index = 23;
    game->board[23].type = SQUARE_PROPERTY;
    strcpy(game->board[23].name, "Peradeniya");

    strcpy(game->board[23].data.property.name, "Peradeniya");
    game->board[23].data.property.group = RED;
    game->board[23].data.property.purchasePrice = 5500;
    game->board[23].data.property.mortgageValue = 2750;
    game->board[23].data.property.rent = 0;
    game->board[23].data.property.houseCost = 1500;
    game->board[23].data.property.hotelCost = 6000;
    game->board[23].data.property.owner = -1;
    game->board[23].data.property.mortgaged = 0;
    game->board[23].data.property.insurance = NONE;
    game->board[23].data.property.houses = 0;
    game->board[23].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 24: Katugastota
    // ---------------------------------------------------------------------
    game->board[24].index = 24;
    game->board[24].type = SQUARE_PROPERTY;
    strcpy(game->board[24].name, "Katugastota");

    strcpy(game->board[24].data.property.name, "Katugastota");
    game->board[24].data.property.group = RED;
    game->board[24].data.property.purchasePrice = 5500;
    game->board[24].data.property.mortgageValue = 2750;
    game->board[24].data.property.rent = 0;
    game->board[24].data.property.houseCost = 1500;
    game->board[24].data.property.hotelCost = 6000;
    game->board[24].data.property.owner = -1;
    game->board[24].data.property.mortgaged = 0;
    game->board[24].data.property.insurance = NONE;
    game->board[24].data.property.houses = 0;
    game->board[24].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 25: Galle Railway Station
    // ---------------------------------------------------------------------
    game->board[25].index = 25;
    game->board[25].type = SQUARE_RAILWAY;
    strcpy(game->board[25].name, "Galle Railway Station");

    strcpy(game->board[25].data.railway.name, "Galle Railway Station");
    game->board[25].data.railway.owner = -1;
    game->board[25].data.railway.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 26: Galle Fort
    // ---------------------------------------------------------------------
    game->board[26].index = 26;
    game->board[26].type = SQUARE_PROPERTY;
    strcpy(game->board[26].name, "Galle Fort");

    strcpy(game->board[26].data.property.name, "Galle Fort");
    game->board[26].data.property.group = YELLOW;
    game->board[26].data.property.purchasePrice = 6500;
    game->board[26].data.property.mortgageValue = 3250;
    game->board[26].data.property.rent = 0;
    game->board[26].data.property.houseCost = 2000;
    game->board[26].data.property.hotelCost = 8000;
    game->board[26].data.property.owner = -1;
    game->board[26].data.property.mortgaged = 0;
    game->board[26].data.property.insurance = NONE;
    game->board[26].data.property.houses = 0;
    game->board[26].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 27: Unawatuna
    // ---------------------------------------------------------------------
    game->board[27].index = 27;
    game->board[27].type = SQUARE_PROPERTY;
    strcpy(game->board[27].name, "Unawatuna");

    strcpy(game->board[27].data.property.name, "Unawatuna");
    game->board[27].data.property.group = YELLOW;
    game->board[27].data.property.purchasePrice = 6500;
    game->board[27].data.property.mortgageValue = 3250;
    game->board[27].data.property.rent = 0;
    game->board[27].data.property.houseCost = 2000;
    game->board[27].data.property.hotelCost = 8000;
    game->board[27].data.property.owner = -1;
    game->board[27].data.property.mortgaged = 0;
    game->board[27].data.property.insurance = NONE;
    game->board[27].data.property.houses = 0;
    game->board[27].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 28: National Water Supply and Drainage Board
    // ---------------------------------------------------------------------
    game->board[28].index = 28;
    game->board[28].type = SQUARE_UTILITY;
    strcpy(game->board[28].name, "National Water Supply and Drainage Board");

    strcpy(game->board[28].data.utility.name, "National Water Supply and Drainage Board");
    game->board[28].data.utility.owner = -1;
    game->board[28].data.utility.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 29: Hikkaduwa
    // ---------------------------------------------------------------------
    game->board[29].index = 29;
    game->board[29].type = SQUARE_PROPERTY;
    strcpy(game->board[29].name, "Hikkaduwa");

    strcpy(game->board[29].data.property.name, "Hikkaduwa");
    game->board[29].data.property.group = YELLOW;
    game->board[29].data.property.purchasePrice = 6500;
    game->board[29].data.property.mortgageValue = 3250;
    game->board[29].data.property.rent = 0;
    game->board[29].data.property.houseCost = 2000;
    game->board[29].data.property.hotelCost = 8000;
    game->board[29].data.property.owner = -1;
    game->board[29].data.property.mortgaged = 0;
    game->board[29].data.property.insurance = NONE;
    game->board[29].data.property.houses = 0;
    game->board[29].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 30: Go To Jail
    // ---------------------------------------------------------------------
    game->board[30].index = 30;
    game->board[30].type = SQUARE_SPECIAL;
    strcpy(game->board[30].name, "Go To Jail");

    // ---------------------------------------------------------------------
    // Index 31: Jaffna Town
    // ---------------------------------------------------------------------
    game->board[31].index = 31;
    game->board[31].type = SQUARE_PROPERTY;
    strcpy(game->board[31].name, "Jaffna Town");

    strcpy(game->board[31].data.property.name, "Jaffna Town");
    game->board[31].data.property.group = GREEN;
    game->board[31].data.property.purchasePrice = 8000;
    game->board[31].data.property.mortgageValue = 4000;
    game->board[31].data.property.rent = 0;
    game->board[31].data.property.houseCost = 2500;
    game->board[31].data.property.hotelCost = 10000;
    game->board[31].data.property.owner = -1;
    game->board[31].data.property.mortgaged = 0;
    game->board[31].data.property.insurance = NONE;
    game->board[31].data.property.houses = 0;
    game->board[31].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 32: Nallur
    // ---------------------------------------------------------------------
    game->board[32].index = 32;
    game->board[32].type = SQUARE_PROPERTY;
    strcpy(game->board[32].name, "Nallur");

    strcpy(game->board[32].data.property.name, "Nallur");
    game->board[32].data.property.group = GREEN;
    game->board[32].data.property.purchasePrice = 8000;
    game->board[32].data.property.mortgageValue = 4000;
    game->board[32].data.property.rent = 0;
    game->board[32].data.property.houseCost = 2500;
    game->board[32].data.property.hotelCost = 10000;
    game->board[32].data.property.owner = -1;
    game->board[32].data.property.mortgaged = 0;
    game->board[32].data.property.insurance = NONE;
    game->board[32].data.property.houses = 0;
    game->board[32].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 33: Ceylinco Insurance
    // ---------------------------------------------------------------------
    game->board[33].index = 33;
    game->board[33].type = SQUARE_INSURANCE;
    strcpy(game->board[33].name, "Ceylinco Insurance");

    // ---------------------------------------------------------------------
    // Index 34: Trincomalee
    // ---------------------------------------------------------------------
    game->board[34].index = 34;
    game->board[34].type = SQUARE_PROPERTY;
    strcpy(game->board[34].name, "Trincomalee");

    strcpy(game->board[34].data.property.name, "Trincomalee");
    game->board[34].data.property.group = GREEN;
    game->board[34].data.property.purchasePrice = 8000;
    game->board[34].data.property.mortgageValue = 4000;
    game->board[34].data.property.rent = 0;
    game->board[34].data.property.houseCost = 2500;
    game->board[34].data.property.hotelCost = 10000;
    game->board[34].data.property.owner = -1;
    game->board[34].data.property.mortgaged = 0;
    game->board[34].data.property.insurance = NONE;
    game->board[34].data.property.houses = 0;
    game->board[34].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 35: Jaffna Railway Station
    // ---------------------------------------------------------------------
    game->board[35].index = 35;
    game->board[35].type = SQUARE_RAILWAY;
    strcpy(game->board[35].name, "Jaffna Railway Station");

    strcpy(game->board[35].data.railway.name, "Jaffna Railway Station");
    game->board[35].data.railway.owner = -1;
    game->board[35].data.railway.mortgaged = 0;

    // ---------------------------------------------------------------------
    // Index 36: National Event Card
    // ---------------------------------------------------------------------
    game->board[36].index = 36;
    game->board[36].type = SQUARE_EVENT;
    strcpy(game->board[36].name, "National Event Card");

    // ---------------------------------------------------------------------
    // Index 37: Nuwara Eliya
    // ---------------------------------------------------------------------
    game->board[37].index = 37;
    game->board[37].type = SQUARE_PROPERTY;
    strcpy(game->board[37].name, "Nuwara Eliya");

    strcpy(game->board[37].data.property.name, "Nuwara Eliya");
    game->board[37].data.property.group = DARK_BLUE;
    game->board[37].data.property.purchasePrice = 10000;
    game->board[37].data.property.mortgageValue = 5000;
    game->board[37].data.property.rent = 0;
    game->board[37].data.property.houseCost = 3000;
    game->board[37].data.property.hotelCost = 12000;
    game->board[37].data.property.owner = -1;
    game->board[37].data.property.mortgaged = 0;
    game->board[37].data.property.insurance = NONE;
    game->board[37].data.property.houses = 0;
    game->board[37].data.property.hotel = 0;

    // ---------------------------------------------------------------------
    // Index 38: Bank of Ceylon
    // ---------------------------------------------------------------------
    game->board[38].index = 38;
    game->board[38].type = SQUARE_BANK;
    strcpy(game->board[38].name, "Bank of Ceylon");

    // ---------------------------------------------------------------------
    // Index 39: Galle Face
    // ---------------------------------------------------------------------
    game->board[39].index = 39;
    game->board[39].type = SQUARE_PROPERTY;
    strcpy(game->board[39].name, "Galle Face");

    strcpy(game->board[39].data.property.name, "Galle Face");
    game->board[39].data.property.group = DARK_BLUE;
    game->board[39].data.property.purchasePrice = 10000;
    game->board[39].data.property.mortgageValue = 5000;
    game->board[39].data.property.rent = 0;
    game->board[39].data.property.houseCost = 3000;
    game->board[39].data.property.hotelCost = 12000;
    game->board[39].data.property.owner = -1;
    game->board[39].data.property.mortgaged = 0;
    game->board[39].data.property.insurance = NONE;
    game->board[39].data.property.houses = 0;
    game->board[39].data.property.hotel = 0;
}