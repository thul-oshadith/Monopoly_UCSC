#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

void runSimulation();

int main() {

    srand(time(NULL)); 
    runSimulation();
    return 0;
}
