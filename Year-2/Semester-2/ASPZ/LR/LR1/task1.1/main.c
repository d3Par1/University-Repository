/*
 * Task 1.1 - main.c
 * Confidence interval calculator using erf function
 *
 * Bug fix: original had `int main  {` -> fixed to `int main() {`
 * Bug fix: original had `erf(z * sqrt(1/2.))` with misplaced parenthesis
 *
 * Compile: gcc -O2 -Wall -o task1_1 main.c erf_module.c -lm
 * Or O3:   gcc -O3 -Wall -o task1_1_o3 main.c erf_module.c -lm
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erf_module.h"

int main(void) {
    double z;
    int df = 0;  /* degrees of freedom for Student's t */

    printf("=== Confidence Interval Calculator ===\n\n");

    /* Option 1: User enters Z-score directly */
    printf("Enter Z-score (or 0 for preset confidence levels): ");
    if (scanf("%lf", &z) != 1) {
        fprintf(stderr, "Invalid input!\n");
        return 1;
    }

    if (fabs(z) < 1e-10) {
        /* Show standard confidence levels */
        printf("\nPreset confidence levels:\n");
        double levels[] = {90.0, 95.0, 99.0};
        for (int i = 0; i < 3; i++) {
            double z_val = get_z_for_confidence(levels[i]);
            printf("  %.0f%%: Z = %.4f, P = %.6f\n",
                   levels[i], z_val, compute_normal_confidence(z_val));
        }

        /* Allow custom confidence level */
        printf("\nEnter custom confidence level (e.g. 97.5), or 0 to skip: ");
        double custom;
        if (scanf("%lf", &custom) == 1 && custom > 0 && custom < 100) {
            double z_custom = get_z_for_confidence(custom);
            printf("  %.1f%%: Z = %.4f, P = %.6f\n",
                   custom, z_custom, compute_normal_confidence(z_custom));
        }
    } else {
        /* Fixed bug: original code had mismatched parentheses */
        /* Original: erf(z * sqrt(1/2.))  -- the closing ')' was wrong */
        /* Correct:  erf(z / sqrt(2.0))  or  erf(z * sqrt(0.5)) */
        printf("The integral of a Normal(0,1) distribution "
               "between -Z and Z is: %g\n", erf(z / sqrt(2.0)));

        print_confidence_results(z);
    }

    /* Student's t-distribution */
    printf("\n--- Student's t-distribution ---\n");
    printf("Enter degrees of freedom (0 to skip): ");
    if (scanf("%d", &df) == 1 && df > 0) {
        printf("Enter t-value: ");
        double t;
        if (scanf("%lf", &t) == 1) {
            double conf = compute_student_confidence(t, df);
            printf("Student's t(df=%d): P(-%.4f < T < %.4f) ≈ %.6f\n",
                   df, t, t, conf);
        }
    }

    return 0;
}
