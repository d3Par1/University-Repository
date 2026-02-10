/*
 * Task 1.10 - Tomorrow's date using enums (Gregorian & Julian)
 *
 * Compile: gcc -Wall -o task1_10 tomorrow_date.c
 */
#include <stdio.h>
#include <stdbool.h>

typedef enum { JANUARY = 1, FEBRUARY, MARCH, APRIL, MAY, JUNE,
               JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER } Month;

typedef enum { GREGORIAN, JULIAN } CalendarType;

const char *month_name(Month m) {
    const char *names[] = {"", "January", "February", "March", "April",
        "May", "June", "July", "August", "September",
        "October", "November", "December"};
    return (m >= 1 && m <= 12) ? names[m] : "Unknown";
}

bool is_leap_year(int year, CalendarType cal) {
    if (cal == GREGORIAN) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    } else { /* JULIAN */
        return year % 4 == 0;
    }
}

int days_in_month(Month month, int year, CalendarType cal) {
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == FEBRUARY && is_leap_year(year, cal))
        return 29;
    return days[month];
}

typedef struct {
    int day;
    Month month;
    int year;
} Date;

Date tomorrow(Date today, CalendarType cal) {
    Date next = today;
    next.day++;

    if (next.day > days_in_month(next.month, next.year, cal)) {
        next.day = 1;
        next.month++;
        if (next.month > DECEMBER) {
            next.month = JANUARY;
            next.year++;
        }
    }
    return next;
}

void print_date(Date d, CalendarType cal) {
    const char *cal_name = (cal == GREGORIAN) ? "Gregorian" : "Julian";
    printf("%02d %s %d (%s)\n", d.day, month_name(d.month), d.year, cal_name);
}

int main() {
    printf("=== Task 1.10: Tomorrow's Date ===\n\n");

    int day, month, year;
    printf("Enter date (day month year): ");
    if (scanf("%d %d %d", &day, &month, &year) != 3 ||
        month < 1 || month > 12 || day < 1 || day > 31) {
        fprintf(stderr, "Invalid date!\n");
        return 1;
    }

    Date today = {day, (Month)month, year};

    printf("\nToday:    ");
    print_date(today, GREGORIAN);

    Date greg_tomorrow = tomorrow(today, GREGORIAN);
    printf("Tomorrow (Gregorian): ");
    print_date(greg_tomorrow, GREGORIAN);

    Date jul_tomorrow = tomorrow(today, JULIAN);
    printf("Tomorrow (Julian):    ");
    print_date(jul_tomorrow, JULIAN);

    /* Special cases */
    printf("\n--- Special cases ---\n");
    Date leap_test = {28, FEBRUARY, 2024};
    printf("Feb 28, 2024 -> Greg: ");
    print_date(tomorrow(leap_test, GREGORIAN), GREGORIAN);
    printf("Feb 28, 2024 -> Jul:  ");
    print_date(tomorrow(leap_test, JULIAN), JULIAN);

    Date year_end = {31, DECEMBER, 2025};
    printf("Dec 31, 2025 -> ");
    print_date(tomorrow(year_end, GREGORIAN), GREGORIAN);

    /* 1900 is leap in Julian but not in Gregorian */
    Date test1900 = {28, FEBRUARY, 1900};
    printf("Feb 28, 1900 -> Greg: ");
    print_date(tomorrow(test1900, GREGORIAN), GREGORIAN);
    printf("Feb 28, 1900 -> Jul:  ");
    print_date(tomorrow(test1900, JULIAN), JULIAN);

    return 0;
}
