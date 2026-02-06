#include <stdio.h>
#include <string.h>
#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Використання: %s <символ> <ім'я файлу>\n", argv[0]);
        return 1;
    }

    char search_char = argv[1][0]; 
    char line[MAX_LINE_LENGTH];
    FILE *file = fopen(argv[2], "r");

    if (file == NULL) {
        printf("Помилка відкриття файлу %s\n", argv[2]);
        return 1;
    }

    printf("Рядки, які містять символ '%c':\n", search_char);
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strchr(line, search_char) != NULL) { 
            printf("%s", line); 
        }
    }

    fclose(file);
    return 0;
}
