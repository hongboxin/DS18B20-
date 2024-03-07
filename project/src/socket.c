/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(06/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "06/03/24 14:36:18"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "project.h"

int socket_client(int argc,char *argv[])
{
	int					fd = 0;
	int					on = 1;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("client use socket() create a socket fd failure:%s\n",strerror(errno));

		return -1;
	}

	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	return fd;
}

int socket_server(int argc,char *argv[])
{
	int						fd = 0;
	struct sockaddr_in		servaddr;
	int						on = 1;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("server use socket() create a socket fd failure:%s\n",strerror(errno));

		return -1;
	}

	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	if( !(argp = parameter_analysis(argc,argv)) )
	{
		printf("Server parameter analysis failure:%s\n",strerror(errno));

		return -1;
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(argp->port);
	inet_pton(AF_INET,argp->ip,&servaddr.sin_addr);

	if( bind(fd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0 )
	{
		printf("server use bind() bind on the socket failure:%s\n",strerror(errno));

		return -1;
	}

	listen(fd,128);

	return fd;
}

