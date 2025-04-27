#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *file = NULL;
    float number, sum = 0.0;
    int count = 0;

    if (argc == 2) {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Помилка відкриття файлу %s\n", argv[1]);
            return 1;
        }
    } else {
        file = stdin;
        printf("Введіть числа з плаваючою комою, завершіть EOF (Ctrl+D):\n");
    }


    while (fscanf(file, "%f", &number) == 1) {
        sum += number;
        count++;
    }

    if (file != stdin) {
        fclose(file);
    }

    if (count > 0) {
        printf("Сума чисел: %.2f\n", sum);
        printf("Середнє арифметичне: %.2f\n", sum / count);
    } else {
        printf("Числа не введено.\n");
    }

    return 0;
}
