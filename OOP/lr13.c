#include <stdio.h>
#include <string.h>

void display_seven_segment(char digit) {
    switch (digit) {
        case '0':
            printf(" _ \n| |\n|_|\n");
            break;
        case '1':
            printf("   \n  |\n  |\n");
            break;
        case '2':
            printf(" _ \n _|\n|_ \n");
            break;
        case '3':
            printf(" _ \n _|\n _|\n");
            break;
        case '4':
            printf("   \n|_|\n  |\n");
            break;
        case '5':
            printf(" _ \n|_ \n _|\n");
            break;
        case '6':
            printf(" _ \n|_ \n|_|\n");
            break;
        case '7':
            printf(" _ \n  |\n  |\n");
            break;
        case '8':
            printf(" _ \n|_|\n|_|\n");
            break;
        case '9':
            printf(" _ \n|_|\n _|\n");
            break;
        default:
            printf("Invalid digit: %c\n", digit);
    }
}

int main() {
    char input[100];
    printf("Enter numbers to display (0-9): ");
    scanf("%s", input);
    for (int i = 0; i < strlen(input); i++) {
        display_seven_segment(input[i]);
        printf("\n");
    }
    return 0;
}
