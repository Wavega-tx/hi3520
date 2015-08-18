/*
 * =====================================================================================
 *
 *       Filename:  settime.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015-08-10 05:22:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>

#include "hi_rtc.h"

int main(int argc, char *argv[])
{
	rtc_time_t	rtc_time;
	timeval		val_time;
	struct tm	tm_time;
	// 1 get the time from rtc
	// ioctl HI_RTC_RD_TIME	
			
	ret = ioctl(fd, HI_RTC_RD_TIME, &rtc_time);
	if (ret < 0) {
		printf("ioctl: HI_RTC_RD_TIME failed\n");
		goto err1;
	}

	tm_time.tm_year	= rtc_time.year - 1900;
	tm_time.tm_mon	= rtc_time.month -1;
	tm_time.tm_mday	= rtc_time.date;
	tm_time.tm_hour	= rtc_time.hour;
	tm_time.tm_min	= rtc_time.minute;
	tm_time.tm_sec	= rtc_time.second;
	tm_time.tm_wday	= rtc_time.weekday;

	val_time.tv_sec		= mktime(&tm_time);
	val_time.tv_usec	= 0;
	settimeofday(&val_time,NULL);

#ifdef DEBUG_TIME
	printf("the value will write to the RTC is:"
			"year:%d\n"
			"month%d\n"
			"day:%d\n"
			"hour:%d\n"
			"min:%d\n"
			"sec:%d\n",
			tm_time.tm_yday,
			tm_time.tm_mon,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec);
#endif
		



	return 0;
}
