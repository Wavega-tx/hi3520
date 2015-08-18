/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015-08-10 03:55:19 AM
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

	time_t sys_time;
	struct tm *p_time;
	rtc_time_t rtc_time;
	time(&sys_time);
	p_time = gmtime(&sys_time);
#ifdef DBUEG_TIME
	printf("time year:%d\n"
			"month :%d\n"
			"day : %d\n"
			"h :%d\n"
			"min :%d\n"
			"second %d\n",
			p_time->tm_year+1900,
			p_time->tm_mon+1,
			p_time->tm_mday,
			p_time->tm_hour + 8,
			p_time->tm_min,
			p_time->tm_sec);
	system("date");
#endif
	rtc_time.year	= p_time->tm_year + 1900;
	rtc_time.month	= p_time->tm_mon + 1;
	rtc_time.date	= p_time->tm_mday;
	rtc_time.hour	= p_time->tm_hour;
	rtc_time.minute = p_time->tm_min;
	rtc_time.second	= p_time->tm_sec;



	return 0;
}
