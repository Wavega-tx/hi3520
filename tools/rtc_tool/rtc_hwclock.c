/*
 * RTC sample&test code.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>

#include "hi_rtc.h"
char *program;
void usage()
{
	printf(
			"\n"
			"Usage: %s [options] [parameter1] ...\n"
			"Options: \n"
			"	-s(set)		Set time/alarm,     e.g '-s time 2012/7/15/13/37/59'\n"
			"	-g(get)		Get time/alarm,     e.g '-g alarm'\n"
//			"	-w(write)	Write RTC register, e.g '-w <reg> <val>'\n"
			"	-w 			read the system time and write it to the RTC\n"
			"	-i 			read the RTC time and set it to the system,use it to the system startup\n"
			"	-r(ead)		Read RTC register,  e.g '-r <reg>'\n"
			"	-a(alarm)	Alarm ON/OFF'\n"
			"	-reset		RTC reset\n"
			"	-m(mode)	Mode of temperature gather, e.g '-m <mode> <temp>, mode[0-2]'\n"
			"\n",
			program);
	exit(1);
}


static int _atoul(const char *str, unsigned char *pvalue)
{
	unsigned int result=0;

	while (*str)
	{
		if (isdigit((int)*str))
		{
			if ((result<429496729) || ((result==429496729) && (*str<'6')))
			{
				result = result*10 + (*str)-48;
			}
			else
			{
				*pvalue = result;
				return -1;
			}
		}
		else
		{
			*pvalue=result;
			return -1;
		}
		str++;
	}
	*pvalue=result;
	return 0;
}


#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

static int  _atoulx(const char *str, unsigned char *pvalue)
{
	unsigned int   result=0;
	unsigned char  ch;

	while (*str)
	{
		ch=toupper(*str);
		if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
		{
			if (result < 0x10000000)
			{
				result = (result << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
			}
			else
			{
				*pvalue=result;
				return -1;
			}
		}
		else
		{
			*pvalue=result;
			return -1;
		}
		str++;
	}

	*pvalue=result;
	return 0;
}

/*used for convert hex value from string to int*/
static int str_to_num(const char *str, unsigned char *pvalue)
{
	if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') ){
		if (*(str+2) == '\0'){
			return -1;
		}
		else{
			return _atoulx(str+2, pvalue);
		}
	}
	else {
		return _atoul(str,pvalue);
	}
}

/*used for convert time frome string to struct rtc_time_t*/
static int parse_string(char *string, rtc_time_t *p_tm)
{
	char *comma, *head;
	int value[10];
	int i;

	if (!string || !p_tm)
		return -1;

	if (!strchr(string, '/'))
		return -1;

	head = string;
	i = 0;
	comma = NULL;

	for(;;) {	
		comma = strchr(head, '/');

		if (!comma){
			value[i++] = atoi(head);
			break;
		}

		*comma = '\0';
		value[i++] = atoi(head);
		head = comma+1;	
	}
	
	if (i < 5)
		return -1;

	p_tm->year   = value[0];
	p_tm->month  = value[1];
	p_tm->date   = value[2];
	p_tm->hour   = value[3];
	p_tm->minute = value[4];
	p_tm->second = value[5];
	p_tm->weekday = 0;

	return 0;
}
#define DEBUG_TIME
int main(int argc, const char *argv[])
{
	program = argv[0];
	rtc_time_t tm;
	reg_data_t regv;
	reg_temp_mode_t mode;
	int ret = -1;
	int fd = -1;
	//const char *dev_name = "/dev/hi_rtc";
	const char *dev_name = "/dev/rtc0";
	char string[50];

	if (argc < 2){
		usage();
		return 0;
	}

	fd = open(dev_name, O_RDWR);
	if (!fd) {
		printf("open %s failed\n", dev_name);
		return -1;
	}

	if (!strcmp(argv[1],"-s")) {
		if (argc < 3) {
			usage();
			goto err1;	
		}

		if (!strcmp(argv[2], "time")) {

			strcpy(string, argv[3]);

			ret = parse_string(string, &tm);
			if (ret < 0)
			{
				printf("parse time param failed\n");
				goto err1;
			}
			printf("set time\n");
#if 1			
			/* code */
			printf("year:%d\n", tm.year);
			printf("month:%d\n",tm.month);
			printf("date:%d\n", tm.date); 
			printf("hour:%d\n", tm.hour);
			printf("minute:%d\n", tm.minute);
			printf("second:%d\n", tm.second);
#endif			
			ret = ioctl(fd, HI_RTC_SET_TIME, &tm);
			if (ret < 0) {
				printf("ioctl: HI_RTC_SET_TIME failed\n");
				goto err1;
			}	
		} else if (!strcmp(argv[2], "alarm")) {

			strcpy(string, argv[3]);

			ret = parse_string(string, &tm);
			if (ret < 0) {
				printf("parse alarm param failed\n");
				goto err1;
			}
			printf("set alarm\n");
#if 1			
			printf("year:%d\n", tm.year);
			printf("month:%d\n",tm.month);
			printf("date:%d\n", tm.date); 
			printf("hour:%d\n", tm.hour);
			printf("minute:%d\n", tm.minute);
			printf("second:%d\n", tm.second);
#endif			
			ret = ioctl(fd, HI_RTC_ALM_SET, &tm);
			if (ret < 0) {
				printf("ioctl: HI_RTC_ALM_SET failed\n");
				goto err1;
			}	
	 	} else {
			printf("unknown options %s\n", argv[2]);
			goto err1;
		}
	} else if (!strcmp(argv[1],"-g")) {
		if (argc < 3) {
			usage();
			goto err1;	
		}

		if (!strcmp(argv[2], "time")) {

			printf("[RTC_RD_TIME]\n");
			
			ret = ioctl(fd, HI_RTC_RD_TIME, &tm);
			if (ret < 0) {
				printf("ioctl: HI_RTC_RD_TIME failed\n");
				goto err1;
			}
			
			printf("Current time value: \n");
		} else if (!strcmp(argv[2], "alarm")) {
		
			printf("[RTC_RD_ALM]\n");

			ret = ioctl(fd, HI_RTC_ALM_READ, &tm);
			if (ret < 0) {
				printf("ioctl: HI_RTC_ALM_READ failed\n");
				goto err1;
			}
			
			printf("Current alarm value: \n");
		} else {
			printf("unknow options %s\n", argv[2]);
			goto err1;
		}
#ifdef DEBUG_TIME
		printf("year %d\n", tm.year);
		printf("month %d\n", tm.month);
		printf("date %d\n", tm.date);
		printf("hour %d\n", tm.hour);
		printf("minute %d\n", tm.minute);
		printf("second %d\n", tm.second);
		printf("weekday %d\n", tm.weekday);
#endif
		
	} else if (!strcmp(argv[1],"-w")) {

		time_t sys_time;
		struct tm *p_time;
		rtc_time_t rtc_time;
		time(&sys_time);
		p_time = gmtime(&sys_time);
#ifdef DEBUG_TIME
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
		printf("system date :\r\n");
		system("date");
#endif
		rtc_time.year	= p_time->tm_year + 1900;
		rtc_time.month	= p_time->tm_mon + 1;
		rtc_time.date	= p_time->tm_mday;
		rtc_time.hour	= p_time->tm_hour;
		rtc_time.minute = p_time->tm_min;
		rtc_time.second	= p_time->tm_sec;

		ret = ioctl(fd, HI_RTC_SET_TIME, &rtc_time);
		if(ret < 0){
			printf("set rtc of localtime error!\r\n");
			goto err1;
		}

	} else if (!strcmp(argv[1],"-i")) {

	rtc_time_t	rtc_time;
	struct timeval		val_time;
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

	} else if (!strcmp(argv[1],"-r")) {

		if (argc < 3) {
			usage();
			goto err1;
		}
		
		ret = str_to_num(argv[2], &(regv.reg));
		if (ret != 0) {
			printf("reg 0x%08x invalid\n", regv.reg);
			goto err1;
		}

		regv.val = 0;

		ret = ioctl(fd, HI_RTC_REG_READ, &regv);
		if (ret < 0) {
			printf("ioctl: HI_RTC_REG_READ failed\n");
			goto err1;
		}

		printf("\n");
		printf("[RTC_REG_GET] reg:0x%02x, val:0x%02x\n", regv.reg, regv.val);
		printf("\n");
	} else if (!strcmp(argv[1],"-a")) {

		if (argc < 3) {
			usage();
			goto err1;
		}

		if (!strcmp(argv[2], "ON")) {
			ret = ioctl(fd, HI_RTC_AIE_ON);
		} else if (!strcmp(argv[2], "OFF")) {
			ret = ioctl(fd, HI_RTC_AIE_OFF);
		}

		if (ret < 0) {
			printf("ioctl: HI_RTC_AIE_ON/OFF failed\n");
			goto err1;
		}

	} else if (!strcmp(argv[1],"-reset")) {

		printf("[RTC_RESET]\n");

		ret = ioctl(fd, HI_RTC_RESET);
		if(ret){
			printf("reset err\n");
			goto err1;
		}
	} else if (!strcmp(argv[1], "-m")) {

		int mod, value;	
		
		if (argc < 3) {
			usage();
			goto err1;
		}

		mod = atoi(argv[2]);

		if (mod > 2 || mod < 0) {
			printf("invalid mode %d\n", mod);
			goto err1;
		}
		
		if (mod == 0) {
			if (argc < 4) {
				usage();
				goto err1;
			}

			value = atoi(argv[3]);
		}	
		else {
			value = 0;
		}
		
		printf("[RTC_SET_TEMP_MODE] %d\n", mod);

		mode.mode = (enum temp_sel_mode)mod;	
		mode.value = value;

		ret = ioctl(fd, HI_RTC_SET_TEMP_MODE, &mode);
		if(ret) {
			printf("ioctl: HI_RTC_SET_TEMP_MODE failed\n");
			goto err1;
		}
	} else {
		printf("unknown download mode.\n");
		goto err1;
	}

err1:
	close(fd);

	return 0;
}

