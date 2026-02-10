/*
 * Task 1.1 - erf_module.h
 * Module for computing error function and confidence intervals
 */
#ifndef ERF_MODULE_H
#define ERF_MODULE_H

/* Compute confidence interval using erf for Normal(0,1) */
double compute_normal_confidence(double z);

/* Compute confidence interval for Student's t-distribution (approx) */
double compute_student_confidence(double t, int df);

/* Get Z-score for a given confidence level (common values) */
double get_z_for_confidence(double confidence_percent);

/* Print confidence interval results */
void print_confidence_results(double z);

#endif /* ERF_MODULE_H */
