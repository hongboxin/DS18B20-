/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  parameter.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 21:20:53"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "project.h"

void print_usage(char *parameter)
{
	printf("%c usage: \n",parameter);
	printf("-i(--IP):please specify a specific IP address!\n");
	printf("-p(--port):Please specify a specific port number!\n");
	printf("-s(--second):Please give a specified time(in seconds)for temperature sampling!\n");
}

struct argument *parameter_analysis(int argc,char *argv[])
{
	int					ch = 0;
	struct argument		*argp =NULL ;

	struct option	opt[] = {
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"second",required_argument,NULL,'s'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while( (ch = getopt_long(argc,argv,"i:p:s:h",opt,NULL)) != -1)
	{
		switch(ch)
		{
			case 'i':
				(*argp).ip = optarg;
				break;

			case 'p':
				(*argp).port = atoi(optarg);
				break;

			case 's':
				(*argp).second = atoi(optarg);
				break;

			default:
				break;
		}
	}
	
	if( !(*argp).ip || !(*argp).port || !(*argp).second )
	{
		print_usage(argv[0]);

		return 0;
	}

	return argp;
}
