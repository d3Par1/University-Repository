// Завдання 1.1: Модуль функції помилок
#ifndef ERF_MODULE_H
#define ERF_MODULE_H

double compute_normal_confidence(double z);

double compute_student_confidence(double t, int df);

double get_z_for_confidence(double confidence_percent);

void print_confidence_results(double z);

#endif
