/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "09/03/24 20:40:11"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <netinet/tcp.h>

#define SN "device1"

struct argument
{
	char	*ip;
	int		port;
	int		second;
};
struct argument		arg_1;
struct argument		*argp;

struct pack
{
	char	device[32];
	char	datime[32];
	float	temper;
};

int			g_status;

int get_temperature(float *temp)
{
	char			w1_path[64] = "/sys/bus/w1/devices/";
	DIR				*dir = NULL;
	struct dirent   *diren = NULL;
	int				found = 0;
	char			cp_path[32];
	int 			fd = 0;
	char			buf[1024];
	char			*ptr;

	if( !(dir = opendir(w1_path)) )
	{
		printf("can not find w1_path file:%s\n",strerror(errno));
		return -1;
	}

	while( diren = readdir(dir) )
	{
		if(strstr(diren->d_name,"28-"))
		{
			strncpy(cp_path,diren->d_name,sizeof(cp_path));
			found = 1;
		}
	}

	if( !found )
	{
		printf("can not find ds18b20 file:%s\n",strerror(errno));
		return -1;
	}

	strncat(w1_path,cp_path,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	closedir(dir);

	fd = open(w1_path,O_RDONLY);
	if( fd < 0 )
	{
		printf("can not fin w1_path document:%s\n",strerror(errno));
		return -1;
	}

	lseek(fd,0,SEEK_SET);
	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));

	ptr = strstr(buf,"t=");
	if( NULL == ptr )
	{
		printf("can not find t= string!\n");
		return -1;
	}
	ptr += 2;

	*temp = atof(ptr)/1000;

	close(fd);
	return 0;
}

void print_usage(char *parameter)
{
	printf("%s usage: \n",parameter);
	printf("-i(--IP):please specify a specific IP address!\n");
	printf("-p(--port):Please specify a specific port number!\n");
	printf("-s(--second):Please give a specified time(in seconds)for temperature sampling!\n");
}

struct argument *parameter_analysis(int argc,char *argv[])
{
	int                 ch = 0;

	argp = &arg_1;

	struct option   opt[] = {
		{"ip",optional_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"second",optional_argument,NULL,'s'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while( (ch = getopt_long(argc,argv,"i::p:s::h",opt,NULL)) != -1)
	{
		switch(ch)
		{
			case 'i':
				arg_1.ip = optarg;
				break;

			case 'p':
				arg_1.port = atoi(optarg);
				break;

			case 's':
				arg_1.second = atoi(optarg);
				break;

			default:
				break;
		}
	}

	if( !arg_1.port )
	{
		print_usage(argv[0]);
		return 0;
	}

	return argp;
}

int client_connect(int fd,struct argument *arg)
{
	struct sockaddr_in		servaddr;
	socklen_t				len = sizeof(servaddr);
	int						rv = 0;

	printf("IP:%s\n",argp->ip);
	printf("PORT:%d\n",argp->port);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(argp->port);
	inet_aton(argp->ip,&servaddr.sin_addr);

	if( (rv = connect(fd,(struct sockaddr*)&servaddr,len)) < 0 )
	{
		printf("client connect failure:%s\n",strerror(errno));
		return -1;
	}

	return 0;
}

int get_time(char *datime)
{
	time_t		seconds;
	struct tm	*local;

	time(&seconds);
	local = localtime(&seconds);

	snprintf(datime,64,"%d-%d-%d %d:%d:%d",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	return 0;
}

int net_status(int fd)
{
	struct tcp_info		info;
	socklen_t			len = sizeof(info);

	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,&len);

	if( info.tcpi_state == TCP_ESTABLISHED )
	{
		printf("The TCP network connection is normal!\n");
		return 0;
	}
	else
	{
		printf("The TCP network connection disconnected!\n");
		return -1;
	}
}

int main(int argc,char*argv[])
{
	int				fd = 0;
	int				rv = 0;
	char			datime[64];
	float       	temp;
	struct pack			pack_1;

	get_time(datime);
	if( (rv = get_temperature(&temp)) < 0 )
	{
		printf("get temperature failure:%s\n",strerror(errno));
		return -1;
	}

	memset(&pack_1,0,sizeof(pack_1));
	strcpy(pack_1.device,SN);
	strcpy(pack_1.datime,datime);
	pack_1.temper = temp;
	printf("device:%s\n",pack_1.device);
	printf("time:%s\n",pack_1.datime);
	printf("temp:%.2f\n",pack_1.temper);

	
	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("client use socket() create a sockfd failure:%s\n",strerror(errno));
		return -1;
	}
	
	if( !(argp = parameter_analysis(argc,argv)) )
	{
		printf("parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}
	printf("parameter analysis successfully!\n");

	if( (rv = client_connect(fd,argp)) < 0 )
	{
		printf("client connect failure:%s\n",strerror(errno));
		return -1;
	}
	printf("client fd[%d] connect successfully!\n",fd);
	
	printf("Start to get temperature,please wait!\n");
	while(1)
	{
		if( (rv = get_time(datime)) < 0 )
		{
			printf("client get local time failure:%s\n",strerror(errno));
			return -1;
		}

		if( (rv = get_temperature(&temp)) < 0 )
		{
			printf("client get temperature failure:%s\n",strerror(errno));
			return -1;
		}

		 
		/*  判断数据库是否存在，不存在则建立*/

		/*Determine network status */
		if( (rv = net_status(fd)) < 0 )
		{
			printf("The TCP network is abnormal:%s\n",strerror(errno));
			g_status = 1;
		}
		else
		{
			printf("The TCP network is normal!\n");
			g_status = 0;
		}
		
		/*network is abnormal*/
		if( g_status )
		{
				/* 将读取信息写入数据库中 */
				/* 断线重连 */
				/* 跳出循环 */
		}
		
		/*network is normal*/
		else
		{
				/*正常read、write给服务端 */
		}

		/* 检测表中是否有数据 */
		/* 有数据则上传至服务端，并删除数据 */
	}  

	return 0;
}
