/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  temperature.c
 *    Description:  This file 
 *                 
 *Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 18:51:14"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "project.h"

int get_temperature(float *temp)
{
	char			w1_path[64] = "/sys/bus/w1/devices/";
	DIR				*dir = NULL;
	struct dirent	*diren = NULL;
	int				found = 0;
	char			cp_path[32];
	int				fd = 0;
	char			buf[1024];
	char			*ptr;

	if( !(dir = opendir(w1_path)) )
	{
		printf("Can not find w1_path file:%s\n",strerror(errno));
		
		return -1;
	}

	while( diren = readdir(dir) )
	{
		if( strstr(diren->d_name,"28-") )
		{
			strncpy(cp_path,diren->d_name,sizeof(cp_path));
			found = 1;
		}
	}

	if( !found )
	{
		printf("Can not find ds18b20 file:%s\n",strerror(errno));

		return -1;
	}

	strncat(w1_path,cp_path,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	closedir(dir);

	fd = open(w1_path,O_RDONLY);
	if( fd < 0 )
	{
		printf("Can not find w1_path document:%s\n",strerror(errno));

		return -1;
	}
	
	lseek(fd,0,SEEK_SET);
	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));

	ptr = strstr(buf,"t=");
	if( NULL == ptr )
	{
		printf("Can not find t= string!\n");

		return -1;
	}
	ptr += 2;

	*temp = atof(ptr)/1000;

	close(fd);
	return 0;
}

