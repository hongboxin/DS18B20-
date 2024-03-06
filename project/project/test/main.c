/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 20:36:44"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "project.h"

int main(int argc,char *argv[])
{
	int			rv = 0;
	float		temp;

	printf("Start to get temperature,please wait!\n");
	while(1)
	{
		sleep(10);
		if( (rv = get_temperature(&temp)) < 0 )
		{
			printf("ds18b20 temperature sensor get temperature failure:%s\n",strerror(errno));

			return -1;
		}
		printf("ds18b20 get temperature successfully and temperature is %.2f\n",temp);
	}

	return 0;
}

