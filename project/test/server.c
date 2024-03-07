/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "07/03/24 14:05:49"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "project.h"

#define MAX_EVENTS		512

int socket_server(int argc,char *argv[]);

int main(int argc,char *argv[])
{
	int						listenfd,connfd;
	int						epollfd;
	int						events;
	struct epoll_event		event_stru;
	struct epoll_event		event_array[MAX_EVENTS];
	int						i;
	int						rv;
	char					buf[1024];

	if( (listenfd = socket_server(argc,argv)) < 0 )
	{
		printf("Server build socket communication failure:%s\n",strerror(errno));

		return -1;
	}

	if( (epollfd = epoll_create(1)) < 0 )
	{
		printf("epoll_create() create a epoll fd failure:%s\n",strerror(errno));

		return -1;
	}
	
	event_stru.data.fd = listenfd; 
	event_stru.events  = EPOLLIN;

	if( epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event_stru) < 0 )
	{
		printf("epoll_ctl() add fd to epollfd failure:%s\n",strerror(errno));

		return -1;
	}

	while(1)
	{
		events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if( events < 0 )
		{
			printf("epoll_wait() failure:%s\n",strerror(errno));

			break;
		}
		else if( events == 0 )
		{
			printf("epoll_wait() get timeout\n");

			continue;
		}

		for(i=0; i<events; i++)
		{
			if( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
			{
				printf("epoll_wait() get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}
		}

		if( event_array[i].data.fd == listenfd )
		{
			if( (connfd = accept(listenfd,(struct sockaddr*)NULL,NULL)) < 0 )
			{
				printf("accept new client failure:%s\n",strerror(errno));

				continue;
			}

			event_stru.data.fd = connfd;
			event_stru.events  = EPOLLIN;
			
			if( epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event_stru) < 0 )
			{
				printf("epoll add new client socket failure:%s\n",strerror(errno));
				close(event_array[i].data.fd);
				continue;
			}
			printf("epoll add new client socket[%d] successfully!\n",connfd);
		}
		
		else
		{
			if( (rv = read(event_array[i].data.fd,buf,sizeof(buf))) < 0 )
			{
				printf("read failure or get disconnected:%s\n",strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
				continue;
			}

			else
			{
				printf("Read [%d] bytes\n",rv);

				if( write(event_array[i].data.fd,buf,rv) < 0 )
				{
					printf("write failure:%s\n",strerror(errno));
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
				}
			}
		}	
	}
}

