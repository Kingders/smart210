#include <stdio.h>	//printf()
#include <time.h>
#include <sys/time.h>	//struct timeval, struct timezone

int main (void)
{
	time_t otime;

	struct tm *tme;

	struct tm *localtm;

	char *stime;

	struct timeval tv;
	struct timezone tz;
	int ret;


	/**** 获取日历时间 ****/
	//从1970.1.1开始到现在的总秒数
	otime = time(NULL);
	printf("%ld\n\r",((long)otime));


	/**** 获取格林威治时间(国际时间) ****/
	//将日历时间转化为世界标准时间
	tme = gmtime(&otime);
	printf("%d 年 %d 月 %d 日 %d 时 %d 分 %d 秒,星期%d\n\r", \
						tme->tm_year,	\
						tme->tm_mon,	\
						tme->tm_mday,	\
						tme->tm_hour,	\
						tme->tm_min,	\
						tme->tm_sec,	\
						tme->tm_wday);


	/**** 获取本地时间 ****/
	//将日历时间转化为世界标准时间
	localtm = localtime(&otime);
	printf("%d 年 %d 月 %d 日 %d 时 %d 分 %d 秒,星期%d\n\r", \
						tme->tm_year,	\
						tme->tm_mon,	\
						tme->tm_mday,	\
						tme->tm_hour,	\
						tme->tm_min,	\
						tme->tm_sec,	\
						tme->tm_wday);


	/**** 以字符串方式显示时间 ****/
	stime = asctime(localtm);
	printf("%s\n\r",stime);


	/**** 获取高精度时间 (微秒级别) ****/
	//tv: 从1970.1.1开始到现在的总秒数 加 微秒
	//tz: &tz 通常为NULL
	ret = gettimeofday(&tv,NULL);
	if(ret == 0)
	printf("sec is %d, usec is %d\n\r", ((int)tv.tv_sec), ((int)tv.tv_usec) );

	return 0;	
}
