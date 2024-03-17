/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "16/03/24 16:15:16"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "project.h"

#define MAX_EVENTS		512
#define database_name	"server.db"
#define table_name		"temperature"

int server_connect(int argc,char *argv[])
{
	int						fd = 0;
	struct sockaddr_in		servaddr;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("Server create sockfd failure:%s\n",strerror(errno));
		return -1;
	}
	
	argp = parameter_analysis(argc,argv);
	if( !argp )
	{
		printf("Parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(argp->port);
	if( !argp->ip )
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_pton(AF_INET,argp->ip,&servaddr.sin_addr);
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
	
	return 0;
}

void set_rlimit()
{
	struct rlimit		limit = {0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

	printf("Set socket open fd max count to %d\n",limit.rlim_max);
}

int main(int argc,char *argv[])
{
	int						listenfd = 0;
	struct sockaddr_in		servaddr;
	int						epollfd;
	struct epoll_event		event;
	int						nfds;
	struct epoll_event		event_array[MAX_EVENTS];
	int						connfd = 0;
	int						rv = -1;
	char					buf[1024];
	char					*ptr = NULL;
	struct pack				pack_1;
	struct pack				pack_2;
	int						i = 0;

	set_rlimit();

	if( (rv = create_database(database_name,table_name)) < 0 )
	{
		printf("Create or open database failure:%s\n",strerror(errno));
		return -1;
	}

	listenfd = server_connect(argc,argv);
	if( listenfd < 0 )
	{
		printf("Server establish socket communication failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Server start to listen...\n");

	if( (epollfd = epoll_create(1)) < 0 )
	{
		printf("Server create epollfd failure:%s\n",strerror(errno));
		return -1;
	}

	event.events  = EPOLLIN;
	event.data.fd = listenfd;

	if( epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0 )
	{
		printf("epoll add listenfd failure:%s\n",strerror(errno));
		return -1;
	}

	while(1)
	{
		nfds = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if( nfds < 0 )
		{
			printf("epoll failiure:%s\n",strerror(errno));
			break;
		}
		else if( nfds == 0 )
		{
			printf("epoll get timeout\n",strerror);
			continue;
		}

		for(i=0; i<nfds; i++)
		{
			if( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
			{
				printf("epoll wait get error:%s\n",strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}

			if( event_array[i].data.fd == listenfd )
			{
				if( (connfd = accept(listenfd,(struct sockaddr*)NULL,NULL)) < 0 )
				{
					printf("accept new client failure:%s\n",strerror(errno));
					continue;
				}

				event.data.fd = connfd;
				event.events  = EPOLLIN;
				if( epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event) < 0 )
				{
					printf("epoll add client socket failure:%s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add client socket[%d] successfully!\n",connfd);
			}
			else
			{
				memset(buf,0,sizeof(buf));
				if( (rv = read(event_array[i].data.fd,buf,sizeof(buf))) <= 0 )
				{
					printf("Server read failure or get timeout:%s\n",strerror(errno));
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					printf("Socket[%d] read %d bytes data:%s\n",event_array[i].data.fd,rv,buf);
					
					ptr = strtok(buf,"/");
					while( NULL != ptr )
					{
						strcpy(pack_1.device,ptr);
						strtok(NULL,"/");
						strcpy(pack_1.datime,ptr);
						strtok(NULL,"/");
						pack_1.temp = atof(ptr);
						strtok(NULL,"/");
					}

					if( (rv = insert_database(database_name,table_name,&pack_1)) < 0 )
					{
						printf("Server insert data into database failure:%s\n",strerror(errno));
						return -1;
					}

					if( (rv = get_database(database_name,table_name,&pack_2)) < 0 )
					{
						printf("Server get data from database failure:%s\n",strerror(errno));
						return -1;
					}
					printf("Server write in database successfully!\n");
					printf("Device:%s,DATIME:%s,TEMP:%.2f\n",pack_2.device,pack_2.datime,pack_2.temp);

					if( write(event_array[i].data.fd,buf,rv) < 0 )
					{
						printf("Server write failure:%s\n",strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
					printf("Server write to client successfully!\n");
				}
			}
		}
	}
	
	return 0;
}
