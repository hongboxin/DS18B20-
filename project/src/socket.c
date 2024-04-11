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
#include <unistd.h>
#include <netdb.h>

#include "project.h"
#include "parameter.h"

int	client_connect(arg_ctx_t *argp)
{
	int						fd = -1;
	int						rv = 0;
	char					service[20];
	struct addrinfo    	    hints, *rp;
	struct addrinfo   		*res = NULL;
	struct in_addr     		inaddr;
	struct sockaddr_in  	addr;
	int                 	len = sizeof(addr);

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if( inet_aton(argp->ip, &inaddr) )
	{
		hints.ai_flags |= AI_NUMERICHOST;
	}
	snprintf(service,sizeof(service),"%d",argp->port);

	if( (rv=getaddrinfo(argp->ip, service, &hints, &res)) )
	{
		printf("getaddrinfo() failure:%s\n",strerror(errno));
		return -1;
	}

	for (rp=res; rp!=NULL; rp=rp->ai_next)
	{
		fd = socket(rp->ai_family,rp->ai_socktype,0);
		if( fd < 0 )
		{
			printf("Create socket fd failure:%s\n",strerror(errno));
			return -1;
		}

		if( (rv = connect(fd,rp->ai_addr,len)) < 0 )
		{
			close(fd);
			continue;
		}
	}

	return fd;
}

int server_connect(char *ip,int port)
{
	int					fd = 0;
	int					on = 1;
	struct sockaddr_in  servaddr;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("Server create socket fd failure:%s\n",strerror(errno));
		return -1;
	}

	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);
	if( !ip )
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_pton(AF_INET,ip,&servaddr.sin_addr);
	}

	if( bind(fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
	{
		printf("Server bind the TCP socket failure:%s\n",strerror(errno));
		return -1;
	}

	if( listen(fd,128) < 0 )
	{
		printf("Server listen the TCP socket failure:%s\n",strerror(errno));
		return -1;
	}

	return fd;
}

int net_status(int fd)
{
	struct tcp_info		info;
	socklen_t			len = sizeof(info);

	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,&len);
	if( info.tcpi_state == TCP_ESTABLISHED )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int send_data(int fd,char *buf,pack_info_t pack)
{
	int 		rv = -1;

	memset(buf,0,sizeof(buf));
	sprintf(buf,"%s/%s/%.2f\n",pack.device,pack.datime,pack.temp);
	if( (rv = write(fd,buf,strlen(buf))) < 0 )
	{
		return -1;
	}

	return 0;
}
