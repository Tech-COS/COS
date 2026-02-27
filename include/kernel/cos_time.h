#pragma once

#include <stdint.h>
#include "interrupts.h"

typedef struct {
    uint64_t year;
    uint64_t month;
    uint64_t day;
    uint64_t hour;
    uint64_t minute;
    uint64_t second;
} datetime_t;

void unix_to_datetime(uint64_t timestamp, datetime_t *dt);
void format_datetime(const datetime_t *dt, char *buffer, uint64_t buffer_size);
