#include "kernel/cos_time.h"
#include <stddef.h>

static const uint32_t days_in_month[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int is_leap_year(uint32_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void unix_to_datetime(uint64_t timestamp, datetime_t *dt)
{
    uint64_t seconds = timestamp;
    uint32_t days;
    uint32_t year = 1970;
    //uint32_t month = 1;
    uint32_t day_of_year;
    uint32_t i;

    dt->second = seconds % 60;
    seconds /= 60;
    dt->minute = seconds % 60;
    seconds /= 60;
    dt->hour = seconds % 24;
    days = seconds / 24;

    while (days >= (is_leap_year(year) ? 366 : 365)) {
        days -= is_leap_year(year) ? 366 : 365;
        year++;
    }
    dt->year = year;

    day_of_year = days;
    for (i = 0; i < 12; i++) {
        uint32_t days_this_month = days_in_month[i];
        if (i == 1 && is_leap_year(year)) {
            days_this_month = 29;
        }
        
        if (day_of_year < days_this_month) {
            dt->month = i + 1;
            dt->day = day_of_year + 1;
            break;
        }
        day_of_year -= days_this_month;
    }
}

void format_datetime(const datetime_t *dt, char *buffer, size_t buffer_size)
{
    if (buffer_size < 20) return; // Format: "YYYY-MM-DD HH:MM:SS"
    
    char temp[20];
    int pos = 0;
    
    // Year
    uint32_t year = dt->year;
    temp[pos++] = '0' + (year / 1000);
    temp[pos++] = '0' + ((year / 100) % 10);
    temp[pos++] = '0' + ((year / 10) % 10);
    temp[pos++] = '0' + (year % 10);
    temp[pos++] = '-';
    
    // Month
    temp[pos++] = '0' + (dt->month / 10);
    temp[pos++] = '0' + (dt->month % 10);
    temp[pos++] = '-';
    
    // Day
    temp[pos++] = '0' + (dt->day / 10);
    temp[pos++] = '0' + (dt->day % 10);
    temp[pos++] = ' ';
    
    // Hour
    temp[pos++] = '0' + (dt->hour / 10);
    temp[pos++] = '0' + (dt->hour % 10);
    temp[pos++] = ':';
    
    // Minute
    temp[pos++] = '0' + (dt->minute / 10);
    temp[pos++] = '0' + (dt->minute % 10);
    temp[pos++] = ':';
    
    // Seconde
    temp[pos++] = '0' + (dt->second / 10);
    temp[pos++] = '0' + (dt->second % 10);
    temp[pos++] = '\0';
    
    for (int i = 0; i < pos && i < (int)buffer_size - 1; i++) {
        buffer[i] = temp[i];
    }
    buffer[pos < (int)buffer_size ? pos : (int)buffer_size - 1] = '\0';
}
