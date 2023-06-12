#include <sg_syrtc.h>

#include <time.h>

#include <sg_syhw.h>
#include <ninjaapi.h>

// RTC functions
Sint32 syRtcInit(void)
{
	njEntryWaitVSyncFunc(mwPlyRestoreRsc);
	return SYD_RTC_ERR_OK;
}

void syRtcFinish(void)
{
	njDeleteWaitVSyncFunc(mwPlyRestoreRsc);
}

Sint32 syRtcGetDate(SYS_RTC_DATE *date)
{
	if (date != NULL)
	{
		// Get current time
		time_t rawtime;
		time(&rawtime);
		struct tm *tm_time = localtime(&rawtime);

		// Setup structure
		date->year = tm_time->tm_year + SYD_RTC_FIRSTYEAR;
		date->month = tm_time->tm_mon + SYD_RTC_FIRSTMONTH;
		date->day = tm_time->tm_mday + SYD_RTC_FIRSTDAY;
		date->hour = tm_time->tm_hour + SYD_RTC_FIRSTHOUR;
		date->minute = tm_time->tm_min + SYD_RTC_FIRSTMINUTE;
		date->second = tm_time->tm_sec + SYD_RTC_FIRSTSECOND;
		date->dayofweek = tm_time->tm_wday;
		date->ageofmoon = 0;
	}
	return 1; // Not a valid error code
}

void syRtcDateToCount(const SYS_RTC_DATE *date, Uint32 *count)
{
	if (date != NULL && count != NULL)
	{
		// Setup structure
		tm tm_time;
		tm_time.tm_year = date->year - SYD_RTC_FIRSTYEAR;
		tm_time.tm_mon = date->month - SYD_RTC_FIRSTMONTH;
		tm_time.tm_mday = date->day - SYD_RTC_FIRSTDAY;
		tm_time.tm_hour = date->hour - SYD_RTC_FIRSTHOUR;
		tm_time.tm_min = date->minute - SYD_RTC_FIRSTMINUTE;
		tm_time.tm_sec = date->second - SYD_RTC_FIRSTSECOND;

		// Return integer
		*count = (Uint32)mktime(&tm_time);
	}
}
