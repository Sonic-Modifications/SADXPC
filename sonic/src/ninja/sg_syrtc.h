#pragma once

#include <sg_xpt.h>

// RTC constants
// Error code
#define SYD_RTC_ERR_OK        (0)
#define SYD_RTC_ERR_FATAL     (-1)

// Error code
#define SYD_RTC_STAT_ACTIVE   (0)
#define SYD_RTC_STAT_PASSIVE  (1)

// Date definition
#define SYD_RTC_FIRSTYEAR     (0) // SYS_RTC_DATE.year is equal to the clock year in this implementation
#define SYD_RTC_FIRSTMONTH    (1)
#define SYD_RTC_FIRSTDAY      (1)
#define SYD_RTC_FIRSTHOUR     (0)
#define SYD_RTC_FIRSTMINUTE   (0)
#define SYD_RTC_FIRSTSECOND   (0)

// RTC globals
struct SYS_RTC_DATE
{
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 minute;
    Uint8 second;
    Uint8 dayofweek;
    Uint8 ageofmoon;
};

// RTC functions
Sint32 syRtcInit(void);
void syRtcFinish(void);

Sint32 syRtcGetDate(SYS_RTC_DATE *date);
void syRtcDateToCount(const SYS_RTC_DATE *date, Uint32 *count);
