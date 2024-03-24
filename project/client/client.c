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
#include "debug.h"

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
	char				buf[1024];

	/* 文件描述符 */
	fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd < 0 )
	{
		DEBUG("Client create socket fd failure:%s\n",strerror(errno));
		return -1;
	}

	/* 参数解析 */
	if( !(argp = parameter_analysis(argc,argv)) )
	{
		DEBUG("Client parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}
	DEBUG("parameter analysis successfully!\n");

	/*客户端进行连接 */
	if( (rv = client_connect(fd,argp)) < 0 )
	{
		DEBUG("Client connect failure:%s\n",strerror(errno));
		return -1;
	}
	DEBUG("Client sockfd[%d] connect successfully!\n",fd);

	printf("Start to collect temperature,please wait!\n");
	while(1)
	{
		sleep(argp->second);

		/*获取采样时间*/
		if( (rv = get_time(datime)) < 0 )
		{
			DEBUG("The client failed to obtain the system time:%s\n",strerror(errno));
			return -1;
		}
		DEBUG("The client obtain the system time successfully!\n");

		/* 获取采样温度值 */
		if( (rv = get_temperature(&temp)) < 0 )
		{
			DEBUG("The client failed to obtain the temperature:%s\n",strerror(errno));
			return -1;
		}
		DEBUG("The client obtain the temperature successfully!\n");

		memset(&pack_1,0,sizeof(pack_1));
		strcpy(pack_1.device,SN);
		strcpy(pack_1.datime,datime);
		pack_1.temp = temp;

		/*判断数据库和表是否存在，不存在则创建*/
		if( (rv = create_database(database_name,table_name)) < 0 )
		{
			DEBUG("Client use create_database() failure:%s\n",strerror(errno));
			return -1;
		}

		/*判断网络状态 */
		if( net_status(fd) == 1 )
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
				DEBUG("Failed to write data to the database:%s\n",strerror(errno));
				return -1;
			}
			
			close(fd);
			fd = socket(AF_INET,SOCK_STREAM,0);
			if( fd < 0 )
			{
				DEBUG("Server recreate sockfd failure:%s\n",strerror(errno));
				return -1;
			}
			
			/*断线重连*/
			if( (rv = client_connect(fd,argp)) < 0 )
			{
				printf("Client reconnect failure:%s\n",strerror(errno));
				printf("Start to continue to sampling!\n");
			}
			else
			{
				printf("Client reconnect successfully!\n");
				while( check_database(database_name,table_name) )
				{
					if( (rv = get_database(database_name,table_name,&pack_2)) < 0 )
					{
						DEBUG("Client get data from database failure:%s\n",strerror(errno));
						return -1;
					}

					memset(buf,0,sizeof(buf));
					sprintf(buf,"%s/%s/%.2f\n",pack_2.device,pack_2.datime,pack_2.temp);
					if( (rv = write(fd,buf,strlen(buf))) < 0 )
					{
						DEBUG("Client upload the database data failure:%s\n",strerror(errno));
						return -1;
					}
					printf("Client upload the database data\n");
					printf("DEVICE\t\t\tDATIME\t\t\tTEMP\n");
					printf("%s\t\t\t%s\t%.2f\n",pack_2.device,pack_2.datime,pack_2.temp);
					printf("\n");

					if( (rv = delete_database(database_name,table_name)) < 0 )
					{
						DEBUG("Client delete data from database failure:%s\n",strerror(errno));
						return -1;
					}
					DEBUG("Delete database data successfully!\n");
					DEBUG("Continue to check database!\n");
				}
				printf("Start to continue to sampling!\n");
			}

			continue;
		}

		/*网络正常*/
		else
		{
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%s/%s/%.2f\n",pack_1.device,pack_1.datime,pack_1.temp);
			if( (rv = write(fd,buf,strlen(buf))) < 0 )
			{
				DEBUG("Client write failure:%s\n",strerror(errno));
				return -1;
			}
			printf("Client write to server successfully and [%d] data is %s\n",rv,buf);	
		}
	
	}

	return 0;
}

