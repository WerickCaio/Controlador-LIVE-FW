#ifndef TIMECALCULATOR_H
#define TIMECALCULATOR_H

#include <Arduino.h>
#include <time.h>

class TimeCalculator {
public:
    TimeCalculator();

    // Calcula diferença total: anos, meses, dias, horas, minutos, segundos
    void calculateDifference(time_t startTimestamp, time_t endTimestamp);

    int getYears();
    int getMonths();
    int getDays();
    int getHours();
    int getMinutes();
    int getSeconds();

private:
    int _years;
    int _months;
    int _days;
    int _hours;
    int _minutes;
    int _seconds;

    bool isLeapYear(int year);
    int getDaysInMonth(int month, int year);
};

#endif
