#include "kernel/types.h"
#include "user/user.h"

#define BILLION 1000000000LL
#define SECONDS_IN_DAY 86400

static int month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};

void print_elem(int elem) {
    if (elem < 10)
        printf("0");
    printf("%d", elem);
}

void print_date(uint64 timestamp) {
    uint64 timestamp_s = timestamp / BILLION;

    uint64 millis = timestamp % BILLION;
    millis = millis / 1000000;

    int sec = timestamp_s % 60;
    int min = (timestamp_s / 60) % 60;
    int hour = (timestamp_s / 3600) % 24;

    int day = timestamp_s / SECONDS_IN_DAY;
    int month = 0;
    int year = 1970;

    while (1) {
        int days_in_year;
        int leap_year = (int)((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0));
        if (leap_year)
            days_in_year = 366;
        else 
            days_in_year = 365;

        if (day < days_in_year) break;
        day -= days_in_year;

        year++;
    }

    while (day >= month_days[month]) {
        day -= month_days[month];
        month++;
    }
    
    print_elem(year);
    printf("-");
    print_elem(month + 1);
    printf("-");
    print_elem(day + 1);
    printf(" ");

    print_elem(hour);
    printf(":");
    print_elem(min);
    printf(":");
    print_elem(sec);
    printf(".");

    if ((int)(millis) < 100)
        printf("0");
    
    print_elem((int)millis);
    printf("\n");
}

int main() {
    uint64 time = read_time();
    print_date(time);

    exit(0);
  }