/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  data.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 19:25:52"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "project.h"

int processing_data(int fd,struct pack packp)
{
	char		buf[1024];
	int			rv = -1;

	memset(buf,0,sizeof(buf));
	sprintf(buf,"%s/%s/%f\n",packp.device,packp.datime,packp.temp);
	if( (rv = write(fd,buf,sizeof(buf))) < 0 )
	{
		printf("Client write failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Client write to server successfully and data is %s\n",buf);

	if( (rv = read(fd,buf,sizeof(buf))) < 0 )
	{
		printf("Client read failure:%s\n",strerror(errno));
		return -1;
	}

	return 0;
}

