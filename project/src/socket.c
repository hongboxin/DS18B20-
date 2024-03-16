/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 11:16:25"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include "project.h"

int	client_connect(int fd,struct argument *argp)
{
	struct sockaddr_in		servaddr;
	socklen_t				len = sizeof(servaddr);
	int						rv = 0;

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(argp->port);
	inet_aton(argp->ip,&servaddr.sin_addr);

	if( (rv = connect(fd,(struct sockaddr *)&servaddr,len)) < 0 )
	{
		printf("Client connect failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Client connect successfully!\n");

	return 0;
}

int net_status(int fd)
{
	struct tcp_info		info;
	socklen_t			len = sizeof(info);

	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,&len);
	if( info.tcpi_state == TCP_ESTABLISHED )
	{
		printf("TCP net is normal!\n");
		return 1;
	}
	else
	{
		printf("TCP net is abnormal!\n");
		return 0;
	}
}


