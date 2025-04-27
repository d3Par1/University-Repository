#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void solve_quadratic(double a, double b, double c, FILE *file) {
    if (a == 0) {
        fprintf(file, "Це не квадратне рівняння!\n");
        printf("Це не квадратне рівняння!\n");
        return;
    }

    double discriminant = b * b - 4 * a * c;
    fprintf(file, "Рівняння: %.2lfx^2 + %.2lfx + %.2lf = 0\n", a, b, c);
    printf("Рівняння: %.2lfx^2 + %.2lfx + %.2lf = 0\n", a, b, c);

    if (discriminant > 0) {
        double x1 = (-b - sqrt(discriminant)) / (2 * a);
        double x2 = (-b + sqrt(discriminant)) / (2 * a);
        fprintf(file, "Корені: x1 = %.6lf, x2 = %.6lf\n\n", x1, x2);
        printf("Корені: x1 = %.6lf, x2 = %.6lf\n\n", x1, x2);
    } else if (discriminant == 0) {
        double x = -b / (2 * a);
        fprintf(file, "Єдиний корінь: x = %.6lf\n\n", x);
        printf("Єдиний корінь: x = %.6lf\n\n", x);
    } else {
        fprintf(file, "Рівняння не має дійсних коренів.\n\n");
        printf("Рівняння не має дійсних коренів.\n\n");
    }
}

int main() {
    double a, b, c;
    char choice;
    FILE *file;

    file = fopen("results.txt", "w");
    if (file == NULL) {
        printf("Помилка відкриття файлу!\n");
        return 1;
    }

    do {
        printf("Введіть коефіцієнти a, b, c: ");
        scanf("%lf %lf %lf", &a, &b, &c);

        solve_quadratic(a, b, c, file);

        printf("Виконати ще раз? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    fclose(file);

    printf("Результати збережено у файл results.txt\n");
    return 0;
}
