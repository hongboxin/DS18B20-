/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(15/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "15/03/24 10:51:39"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sqlite3.h"
#include "project.h"

#define SN					"ds18b20"
#define database_name		"client.db"
#define table_name			"temperature"

int		g_status;

int main(int argc,char *argv[])
{
	int					fd = 0;
	int					rv = -1;
	char				datime[128];
	float				temp;
	struct pack			pack_1;
	struct pack			pack_2;

	/* 文件描述符 */
	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		printf("Client create socket fd failure:%s\n",strerror(errno));
		return -1;
	}

	/* 参数解析 */
	if( !(argp = parameter_analysis(argc,argv)) )
	{
		printf("Client parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}
	printf("parameter analysis successfully!\n");

	/*客户端进行连接 */
	if( (rv = client_connect(fd,argp)) < 0 )
	{
		printf("Client connect failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Client socket fd[%d] connect successfully!\n",fd);

	printf("Start to collect temperature,please wait!\n");
	while(1)
	{
		sleep(argp->second);

		/*获取采样时间*/
		if( (rv = get_time(datime)) < 0 )
		{
			printf("The client failed to obtain the system time:%s\n",strerror(errno));
			return -1;
		}
		printf("The client obtain the system time successfully!\n");
		printf("TIME:%s\n",datime);

		/* 获取采样温度值 */
		if( (rv = get_temperature(&temp)) < 0 )
		{
			printf("The client failed to obtain the temperature:%s\n",strerror(errno));
			return -1;
		}
		printf("The client obtain the temperature successfully!\n");
		printf("TEMP:%.2f\n",temp);

		memset(&pack_1,0,sizeof(pack_1));
		strcpy(pack_1.device,SN);
		strcpy(pack_1.datime,datime);
		pack_1.temp = temp;

		/*判断数据库和表是否存在，不存在则创建*/
		if( (rv = create_database(database_name,table_name)) < 0 )
		{
			printf("Client use create_database() failure:%s\n",strerror(errno));
			return -1;
		}

		/*判断网络状态 */
		if( net_status(fd) )
		{
			printf("The TCP network is normal!\n");
			g_status = 0;
		}
		else
		{
			printf("The TCP network is abnormal!\n");
			g_status = 1;
		}

		/*网络异常*/
		if( g_status )
		{
			/* 将读取信息写入数据库中 */		
			if( (rv = insert_database(database_name,table_name,&pack_1)) < 0 )
			{
				printf("Failed to write data to the database:%s\n",strerror(errno));
				return -1;
			}

			/*断线重连*/
			if( (rv = client_connect(fd,argp)) < 0 )
			{
				printf("Client reconnection failure:%s\n",strerror(errno));
				return -1;
			}

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

			/*检测表中是否有数据*/
			while(1)
			{
				/* 表中存在数据 */
				if( check_database(database_name,table_name) )
				{
					/*获取第一行数据 */
					if( (rv = get_database(database_name,table_name,&pack_2)) < 0 )
					{
						printf("Client get data from database failure:%s\n",strerror(errno));
						return -1;
					}

					/*上传获取的数据*/
					if( (rv = processing_data(fd,pack_2)) < 0 )
					{
						printf("Client processing database data failure:%s\n",strerror(errno));
						return -1;
					}

					/* 删除获取到的数据 */
					if( (rv = delete_database(database_name,table_name)) < 0 )
					{
						printf("Client delete data from database failure:%s\n",strerror(errno));
						return -1;
					}

					/*重复循环，直至表中没有数据 */
					continue;
				}
				
				/* 表中不存在数据 */
				else
				{
					printf("There is no data in the database or all data has been uploaded!\n");
					return 0;
				}
			}
		}
	}

	return 0;
}

