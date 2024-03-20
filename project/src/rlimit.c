/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  rlimit.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 15:46:52"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "project.h"

void set_rlimit()
{
	struct rlimit		limit = {0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);
	printf("Set socket open fd max count to %d\n",limit.rlim_max);

}
