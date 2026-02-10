/*
 * Task 1.1 - erf_module.c
 * Implementation of error function module for confidence intervals
 */
#include <math.h>
#include <stdio.h>
#include "erf_module.h"

/* Compute the integral of Normal(0,1) between -z and z using erf */
double compute_normal_confidence(double z) {
    return erf(z / sqrt(2.0));  /* Fixed: use 2.0 instead of 1/2. for clarity */
}

/*
 * Approximate Student's t-distribution confidence interval
 * Uses approximation: for large df, t-distribution -> normal distribution
 * For small df, uses a refined approximation
 */
double compute_student_confidence(double t, int df) {
    if (df <= 0) {
        fprintf(stderr, "Error: degrees of freedom must be positive\n");
        return -1.0;
    }
    /* Approximation using the incomplete beta function relationship */
    double x = df / (df + t * t);
    /* Simple approximation for moderate df */
    double approx = compute_normal_confidence(t) * (1.0 + (x / (4.0 * df)));
    if (approx > 1.0) approx = 1.0;
    if (approx < -1.0) approx = -1.0;
    return approx;
}

/* Get Z-score for common confidence levels */
double get_z_for_confidence(double confidence_percent) {
    if (fabs(confidence_percent - 90.0) < 0.01) return 1.645;
    if (fabs(confidence_percent - 95.0) < 0.01) return 1.960;
    if (fabs(confidence_percent - 99.0) < 0.01) return 2.576;
    /* For other values, approximate using inverse erf */
    fprintf(stderr, "Warning: using approximation for %.1f%% confidence\n",
            confidence_percent);
    return sqrt(2.0) * erfc(1.0 - confidence_percent / 100.0);
}

/* Print confidence interval results for 90%, 95%, 99% */
void print_confidence_results(double z) {
    printf("\n=== Confidence Interval Results ===\n");
    printf("Z-score: %.4f\n", z);
    printf("Normal(0,1) integral between -%.4f and %.4f: %g\n",
           z, z, compute_normal_confidence(z));
    printf("\nStandard confidence levels:\n");

    double levels[] = {90.0, 95.0, 99.0};
    for (int i = 0; i < 3; i++) {
        double z_val = get_z_for_confidence(levels[i]);
        printf("  %.0f%% -> Z=%.3f, P=%.6f\n",
               levels[i], z_val, compute_normal_confidence(z_val));
    }
}
