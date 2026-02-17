// Завдання 1.6: Перетворення секунд у час
#include <stdio.h>
#include <stdlib.h>

void convert_seconds(long total_seconds) {
    int negative = 0;
    if (total_seconds < 0) {
        negative = 1;
        total_seconds = -total_seconds;
    }

    long hours = total_seconds / 3600;
    long minutes = (total_seconds % 3600) / 60;
    long seconds = total_seconds % 60;

    if (negative) {
        printf("%ld секунд тому еквівалентно %ld годинам %ld хвилинам %ld секундам у минулому.\n",
               total_seconds, hours, minutes, seconds);
    } else {
        printf("%ld секунд еквівалентно %ld годинам %ld хвилинам %ld секундам.\n",
               total_seconds, hours, minutes, seconds);
    }
}

int main() {
    long seconds;

    printf("Enter number of seconds: ");
    if (scanf("%ld", &seconds) != 1) {
        fprintf(stderr, "Invalid input!\n");
        return 1;
    }

    convert_seconds(seconds);

    printf("\n--- Examples ---\n");
    convert_seconds(7322);
    convert_seconds(-3661);
    convert_seconds(0);
    convert_seconds(86400);

    return 0;
}
