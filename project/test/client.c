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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "project.h"

int get_temperature(float *temp);
struct argument *parameter_analysis(int argc,char *argv[]);
int socket_client(int argc,char *argv[]);

int main(int argc,char *argv[])
{
	int						fd = 0;
	int						rv = 0;
	float					temp;
	struct sockaddr_in		servaddr;
	socklen_t				len = sizeof(servaddr);
	char					buf[1024];
	char					tem[32];
	struct argument			*argp;
	
	
	if( (fd = socket_client(argc,argv)) < 0 )
	{
		printf("The client bulid socket communication failure:%s\n",strerror(errno));

		return -1;
	}

	argp = (struct argument *)malloc(sizeof(struct argument *));
	
	if( !(argp = parameter_analysis(argc,argv)) )
	{  
		printf("Client parameter analysis failure:%s\n",strerror(errno));
		
		return -1;
	}   
			    
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(argp->port);
	inet_aton(argp->ip,&servaddr.sin_addr);

	if( (rv = connect(fd,(struct sockaddr*)&servaddr,len)) < 0 )
	{
		printf("client connect failure:%s\n",strerror(errno));

		return -1;
	}
	printf("client connect successfully!\n");

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
		
		memset(tem,0,sizeof(tem));
		sprintf(tem,"%.2f",temp);

		if( (write(fd,tem,sizeof(tem))) < 0 )
		{
			printf("client write failure:%s\n",strerror(errno));

			return -1;
		}
		printf("client write bytes:%s\n",tem);
		
		memset(buf,0,sizeof(buf));
		if( (rv = read(fd,buf,sizeof(buf))) <= 0 )
		{
			printf("client read failure:%s\n",strerror(errno));

			return -1;
		}
		printf("client read [%d] bytes:%s\n",rv,buf);
	}
	
	free(argp);

	return 0;
}

