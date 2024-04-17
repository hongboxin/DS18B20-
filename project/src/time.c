/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  time.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 10:40:37"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <time.h>
#include "mytime.h"

int get_time(char *datime,int size)
{
	time_t		seconds;
	struct tm	*local;

	time(&seconds);
	local = localtime(&seconds);
	snprintf(datime,size,"%d-%d-%d---%d:%d:%d",local->tm_year+1900,local->tm_mon+1,
			local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	return 0;
}

int check_time(time_t *last_time,int interval)
{
	time_t		now_time;
	int			rv = 0;
	double		diff;

	time(&now_time);
	diff = difftime(now_time,*last_time);

	if( diff >= (double)interval )
	{
		rv = 1;
		*last_time = now_time;
	}
	
	return rv;
}
