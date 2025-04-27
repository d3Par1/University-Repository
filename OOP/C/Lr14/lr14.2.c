#include <stdio.h>

int main() {
    FILE *fp1, *fp2; 
    char ch;

    fp1 = fopen("terky", "r");
    fp2 = fopen("jerky", "w");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Помилка відкриття файлів!\n");
        return 1;
    }

    while ((ch = getc(fp1)) != EOF) {
        fprintf(fp2, "%c\n", ch); 
        putc(ch, stdout);        
    }

    fclose(fp1); 
    fclose(fp2); 

    return 0;
}
