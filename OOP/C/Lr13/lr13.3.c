#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void roll_dice(int rolls) {
    for (int i = 0; i < rolls; i++) {
        int dice1 = rand() % 8 + 1;
        int dice2 = rand() % 8 + 1;
        printf("Roll %d: Dice 1 = %d, Dice 2 = %d\n", i + 1, dice1, dice2);
    }
}

int main() {
    srand(time(NULL));
    int rolls;

    printf("Enter number of rolls: ");
    while (scanf("%d", &rolls) == 1) {
        if (rolls <= 0) {
            printf("Number of rolls must be greater than 0. Try again: ");
            continue;
        }
        roll_dice(rolls);
        printf("Enter number of rolls (or a negative number to quit): ");
    }

    printf("Game ended.\n");
    return 0;
}
