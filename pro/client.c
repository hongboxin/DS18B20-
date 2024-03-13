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
#include "sqlite3.h"

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

/* This function is used for ds18b20 temperature sampling */
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

/* This function is used to print the parameter help information */
void print_usage(char *parameter)
{
	printf("%s usage: \n",parameter);
	printf("-i(--IP):please specify a specific IP address!\n");
	printf("-p(--port):Please specify a specific port number!\n");
	printf("-s(--second):Please give a specified time(in seconds)for temperature sampling!\n");
}

/* This function is used for parameter parsing */
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

/* This function is used by the client for connection */
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

/* This function is used to obtain the system time */
int get_time(char *datime)
{
	time_t		seconds;
	struct tm	*local;

	time(&seconds);
	local = localtime(&seconds);

	snprintf(datime,64,"%d-%d-%d %d:%d:%d",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	return 0;
}

/* This function is used to determine the network status */
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

/* This function is used to open or create databases and tables */
int create_database(void)
{
	sqlite3			*db;
	int				rv = 0;
	char			sql[128] = {0};
	rv = sqlite3_open("client.db",&db);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_open() failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("Open or create database successfully!\n");

	sprintf(sql,"create table if not exists temperature(Device_name text,Sampling_time text,Sampling_temperature real);");
	rv = sqlite3_exec(db,sql,NULL,NULL,NULL);
	if( rv != SQLITE_OK )
	{
		printf("create table failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("create table successfully!\n");

	sqlite3_close(db);
	return 0;

}

/* This function is used to insert data into the database */
int insert_database(struct pack *packp,char *table_name)
{
	sqlite3			*db;
	int				rv;
	char			sql[128] = {0};

	rv = sqlite3_open("client.db",&db);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_open() failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("Open or create database successfully!\n");

	sprintf(sql,"insert into %s values(%s,%s,%f);",table_name,packp->device,packp->datime,packp->temper);
	rv = sqlite3_exec(db,sql,NULL,NULL,NULL);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_insert() failure:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	printf("sqlite3_insert() successfully!\n");

	sqlite3_close(db);
	return 0;
}

int callback(void * data, int col_count, char ** col_values, char ** col_Name)
{

}
int check_database(char *table_name)
{
	sqlite3		*db;
	int			rv = 0;
	char		sql[128] = {0};

	rv = sqlite3_open("client.db",&db);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_open() failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("Open or create database successfully!\n");

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from %s ORDER BY ROWID ASC limit 1",table_name);

	rv = sqlite3_exec(db,sql,callback,NULL,NULL);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3 select data failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_close(db);
	return 0;
}

int get_database(struct pack *packp,char *table_name)
{
	sqlite3			*db;
	int				rv;
	char			sql[128] = {0};

	rv = sqlite3_open("client.db",&db);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_open() failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from %s ORDER BY ROWID ASC limit 1",table_name);
	rv = sqlite3_exec(db,sql,callback,NULL,NULL);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3 select data failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	
	memset(packp,0,sizeof(packp));
	strcpy(packp->device,result[3]);
	strcpy(packp->datime,result[4]);
	packp->temper = atof(result[5]);

	printf("packp->device:%s,packp->datime:%s,packp->temper:%f\n",packp->device,packp->datime,packp->temper);

	sqlite3_close(db);
	return 0;
}

int delete_database(char *table_name)
{
	sqlite3			*db;
	int				rv;
	char			sql[128];

	rv = sqlite3_open("client.db",&db);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3_open() failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"DELETE from %s limit 1;",table_name);
	rv = sqlite3_exec(db,sql,NULL,NULL,NULL);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3 delete failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("delete data successfully!\n");

	sqlite3_close(db);
	return 0;
}

int processing_data(int fd,struct pack packp)
{
	char		buf[1024];
	int			rv = 0;

	memset(buf,0,sizeof(buf));
	sprintf(buf,"%s %s %f\n",packp.device,packp.datime,packp.temper);
	if( (rv = write(fd,buf,sizeof(buf))) < 0 )
	{
		printf("client write data to client failure:%s\n",strerror(errno));
		return -1;
	}
	printf("client write data to client successfully!\n");
	printf("client write [%d] bytes and data is %s\n",rv,buf);

	if( (rv = read(fd,buf,sizeof(buf))) < 0 )
	{
		printf("client read data from client failure:%s\n",strerror(errno));
		return -1;
	}
	printf("client read data from client successfully!\n");

	return 0;
}
int main(int argc,char*argv[])
{
	int					fd = 0;
	int					rv = 0;
	char				datime[64];
	float		       	temp;
	struct pack			pack_1;
	struct pack			pack_2;

	/* 创建socket文件描述符 */
	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("client use socket() create a sockfd failure:%s\n",strerror(errno));
		return -1;
	}
	
	/* 参数解析 */
	if( !(argp = parameter_analysis(argc,argv)) )
	{
		printf("parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}
	printf("parameter analysis successfully!\n");
	
	/* 客户端进行连接 */
	if( (rv = client_connect(fd,argp)) < 0 )
	{
		printf("client connect failure:%s\n",strerror(errno));
		return -1;
	}
	printf("client fd[%d] connect successfully!\n",fd);
	
	printf("Start to get temperature,please wait!\n");
	while(1)
	{
		/* 获取采样时间 */
		if( (rv = get_time(datime)) < 0 )
		{
			printf("client get local time failure:%s\n",strerror(errno));
			return -1;
		}
		printf("client get datime successfully!\n");
		
		/* 获取采样温度值 */
		if( (rv = get_temperature(&temp)) < 0 )
		{
			printf("client get temperature failure:%s\n",strerror(errno));
			return -1;
		}
		printf("client get temperature successfully!\n");

		memset(&pack_1,0,sizeof(pack_1));
		strcpy(pack_1.device,SN);
		strcpy(pack_1.datime,datime);
		pack_1.temper = temp;
		
		/*判断数据库和表是否存在，不存在则创建 */
		if( (rv = create_database()) < 0 )
		{
			printf("create or open database failure:%s\n",strerror(errno));
			return -1;
		}
		printf("create or open database_table successfully!\n");

		/*判断网络状态 */
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
		
		/*网络异常*/
		if( g_status )
		{
			/* 将读取信息写入数据库中 */
			if( (rv = insert_database(&pack_1,"temperature")) < 0 )
			{
				printf("insert data to database failure:%s\n",strerror(errno));
				return -1;
			}
			printf("The data is successfully written to the databases!\n");
		
			/* 断线重连 */
			if( (rv = client_connect(fd,argp)) < 0 )
			{
				printf("reconnection failure:%s\n",strerror(errno));
				return -1;
			}
			printf("reconnection successfully!\n");
			
			continue;
		}
		
		/*网络正常*/
		else
		{
			if( (rv = processing_data(fd,pack_1)) < 0 )
			{
				printf("client processing data failure:%s\n",strerror(errno));
				return -1;
			}
			printf("client processing data successfully!\n");
		

			/* 检测表中是否有数据 */
			while(1)
			{
				/* 表中存在数据 */
				if( check_database("temperature") )
				{
					/* 获取表中数据，并上传至服务端 */
					if( (rv = get_database(&pack_2,"temperature")) < 0 )
					{
						printf("client get data from database failure:%s\n",strerror(errno));
						return -1;
					}

					if( (rv = processing_data(fd,pack_1)) < 0 )
					{
						printf("client processing data failure:%s\n",strerror(errno));
						return -1;
					}

					/* 删除获取到的数据 */
					if( (rv = delete_database("temperature")) < 0 )
					{
						printf("delete_database failure:%s\n",strerror(errno));
						return -1;
					}
					/* 返回，直到表中无数据 */
					continue;
				}

				/* 表中不存在数据 */
				printf("There is no data in the database or all data has been uploaded!\n");
				break;
				
			}
		}	
	}  

	return 0;
}
