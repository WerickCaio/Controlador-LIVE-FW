#include "TimeCalculator.h"

TimeCalculator::TimeCalculator() : _years(0), _months(0), _days(0), _hours(0), _minutes(0), _seconds(0) {}

void TimeCalculator::calculateDifference(time_t startTimestamp, time_t endTimestamp)
{
    if (endTimestamp < startTimestamp) {
        time_t temp = startTimestamp; startTimestamp = endTimestamp; endTimestamp = temp;
    }

    struct tm startTm;
    localtime_r(&startTimestamp, &startTm);
    struct tm endTm;
    localtime_r(&endTimestamp, &endTm);

    int y1 = startTm.tm_year + 1900; int m1 = startTm.tm_mon; int d1 = startTm.tm_mday;
    int h1 = startTm.tm_hour; int min1 = startTm.tm_min; int s1 = startTm.tm_sec;
    
    int y2 = endTm.tm_year + 1900;   int m2 = endTm.tm_mon;   int d2 = endTm.tm_mday;
    int h2 = endTm.tm_hour; int min2 = endTm.tm_min; int s2 = endTm.tm_sec;

    // Segundos
    if (s2 < s1) { s2 += 60; min2--; }
    _seconds = s2 - s1;

    // Minutos
    if (min2 < min1) { min2 += 60; h2--; }
    _minutes = min2 - min1;

    // Horas
    if (h2 < h1) { h2 += 24; d2--; }
    _hours = h2 - h1;

    // Dias
    if (d2 < d1) {
        m2--; 
        int monthToCheck = m2; int yearToCheck = y2;
        if (monthToCheck < 0) {
            monthToCheck = 11; 
            yearToCheck--;
        }
        d2 += getDaysInMonth(monthToCheck + 1, yearToCheck); // monthToCheck + 1 converte de 0-11 para 1-12
    }
    _days = d2 - d1;

    // Meses
    if (m2 < m1) { m2 += 12; y2--; }
    _months = m2 - m1;
    
    // Anos
    _years = y2 - y1;
}

int TimeCalculator::getYears() { return _years; }
int TimeCalculator::getMonths() { return _months; }
int TimeCalculator::getDays() { return _days; }
int TimeCalculator::getHours() { return _hours; }
int TimeCalculator::getMinutes() { return _minutes; }
int TimeCalculator::getSeconds() { return _seconds; }

bool TimeCalculator::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int TimeCalculator::getDaysInMonth(int month, int year)
{
    if (month == 2)
        return isLeapYear(year) ? 29 : 28;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    return 31;
}
