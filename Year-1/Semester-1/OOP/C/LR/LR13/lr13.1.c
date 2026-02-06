#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int choose_new_secret() {
    return rand() % 100 + 1; // Новий номер елемента від 1 до 100
}

void read_guesses(int secret_number) {
    int guess;
    printf("Guess the number: ");
    while (scanf("%d", &guess)) {
        if (guess < secret_number) {
            printf("Too low!\n");
        } else if (guess > secret_number) {
            printf("Too high!\n");
        } else {
            printf("Congratulations! You guessed the secret number!\n");
            break;
        }
        printf("Try again: ");
    }
}

int main() {
    srand(time(NULL));
    int secret_number = choose_new_secret();
    read_guesses(secret_number);
    return 0;
}
