/*
 * Task 1.21 - Roots of quadratic & cubic equations (real + complex)
 *
 * Compile: gcc -Wall -o task1_21 equation_roots.c -lm
 */
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

/* Quadratic: ax^2 + bx + c = 0 */
void solve_quadratic(double a, double b, double c) {
    printf("\n--- Quadratic: %.2fx² + %.2fx + %.2f = 0 ---\n", a, b, c);

    if (fabs(a) < 1e-12) {
        if (fabs(b) < 1e-12) {
            printf("Not an equation.\n");
        } else {
            printf("Linear: x = %.6f\n", -c / b);
        }
        return;
    }

    double D = b * b - 4 * a * c;
    printf("Discriminant D = %.6f\n", D);

    if (D > 1e-12) {
        double x1 = (-b + sqrt(D)) / (2 * a);
        double x2 = (-b - sqrt(D)) / (2 * a);
        printf("Two real roots: x1 = %.6f, x2 = %.6f\n", x1, x2);
    } else if (fabs(D) <= 1e-12) {
        double x = -b / (2 * a);
        printf("One root (double): x = %.6f\n", x);
    } else {
        double real = -b / (2 * a);
        double imag = sqrt(-D) / (2 * a);
        printf("Complex roots: x1 = %.6f + %.6fi, x2 = %.6f - %.6fi\n",
               real, imag, real, imag);
    }
}

/* Cubic: ax^3 + bx^2 + cx + d = 0 (Cardano's method) */
void solve_cubic(double a, double b, double c, double d) {
    printf("\n--- Cubic: %.2fx³ + %.2fx² + %.2fx + %.2f = 0 ---\n", a, b, c, d);

    if (fabs(a) < 1e-12) {
        solve_quadratic(b, c, d);
        return;
    }

    /* Reduce to depressed cubic: t^3 + pt + q = 0 */
    double p = (3 * a * c - b * b) / (3 * a * a);
    double q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) / (27 * a * a * a);
    double shift = b / (3 * a);

    double D = (q * q / 4) + (p * p * p / 27);
    printf("p = %.6f, q = %.6f, D = %.6f\n", p, q, D);

    if (D > 1e-12) {
        /* One real root, two complex */
        double u = cbrt(-q / 2 + sqrt(D));
        double v = cbrt(-q / 2 - sqrt(D));
        double x1 = u + v - shift;
        double re = -(u + v) / 2 - shift;
        double im = (u - v) * sqrt(3.0) / 2;
        printf("One real root: x1 = %.6f\n", x1);
        printf("Complex: x2 = %.6f + %.6fi, x3 = %.6f - %.6fi\n",
               re, fabs(im), re, fabs(im));
    } else if (fabs(D) <= 1e-12) {
        /* All real, at least two equal */
        double u = cbrt(-q / 2);
        printf("x1 = %.6f (double), x2 = %.6f\n",
               2 * u - shift, -u - shift);
    } else {
        /* Three distinct real roots (trigonometric method) */
        double r = sqrt(-p * p * p / 27);
        double theta = acos(-q / (2 * r));
        double m = 2 * cbrt(r);
        double x1 = m * cos(theta / 3) - shift;
        double x2 = m * cos((theta + 2 * PI) / 3) - shift;
        double x3 = m * cos((theta + 4 * PI) / 3) - shift;
        printf("Three real roots: x1 = %.6f, x2 = %.6f, x3 = %.6f\n", x1, x2, x3);
    }
}

int main() {
    printf("=== Task 1.21: Equation Roots ===\n");

    /* Quadratic examples */
    solve_quadratic(1, -5, 6);     /* x=2, x=3 */
    solve_quadratic(1, -2, 1);     /* x=1 (double) */
    solve_quadratic(1, 0, 1);      /* complex */

    /* Cubic examples */
    solve_cubic(1, -6, 11, -6);    /* x=1, x=2, x=3 */
    solve_cubic(1, 0, 0, -8);      /* x=2, complex */
    solve_cubic(1, -3, 3, -1);     /* x=1 (triple) */

    /* User input */
    printf("\n--- Enter your equation ---\n");
    printf("Quadratic (a b c): ");
    double a, b, c;
    if (scanf("%lf %lf %lf", &a, &b, &c) == 3) {
        solve_quadratic(a, b, c);
    }

    return 0;
}
