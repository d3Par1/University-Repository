#include <stdio.h>
#include <string.h>

void extract_words(char lines[][100], char words[][100], int num_lines) {
    for (int i = 0; i < num_lines; i++) {
        sscanf(lines[i], "%s", words[i]);
    }
}

int main() {
    char lines[3][100] = {"Hello world!", "This is a test.", "C programming"};
    char words[3][100];
    extract_words(lines, words, 3);
    for (int i = 0; i < 3; i++) {
        printf("First word: %s\n", words[i]);
    }
    return 0;
}
