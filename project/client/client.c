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
#include "logger.h"

#define SN			"rpi#0001"

int					g_signal;
static int			g_status;
static sqlite3		*db;

int main(int argc,char *argv[])
{
	int					fd = 0;
	int					rv = -1;
	char				datime[128];
	float				temp;
	pack_info_t			pack;
	char				buf[1024];
	int					flag = 0;
	time_t				last_time = 0;
	char                *logfile="client.log";
	int                 loglevel=LOG_LEVEL_INFO;
	int                 logsize=10;

	/* 参数解析 */
	if( !(argp = parameter_analysis(argc,argv)) )
	{
		printf("Client parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}

	/*客户端进行连接 */
	if( (fd = client_connect(argp)) < 0 )
	{
		printf("Client connect failure:%s\n",strerror(errno));
		return -1;
	}

	/*  安装信号 */ 
	install_signal();

	/* 打开数据库 */
	db = open_database();
	if( !db )
	{
		printf("Client open database failure:%s\n",strerror(errno));
		return -1;
	}

	if( log_open(logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		printf("Initial log system failed:%s\n",strerror(errno));
		return -1;
	}

	while( !g_signal )
	{
		flag = 0;
		/* 判断是否进行采样 */
		if( check_time(&last_time,argp->second) )
		{
			/*获取采样时间*/
			if( (rv = get_time(datime)) < 0 )
			{
				log_error("The client failed to obtain the system time:%s\n",strerror(errno));
			}
			log_debug("The client obtain the system time successfully!\n");

			/* 获取采样温度值 */
			if( (rv = get_temperature(&temp)) < 0 )
			{
				log_error("The client failed to obtain the temperature:%s\n",strerror(errno));
			}
			log_debug("The client obtain the temperature successfully!\n");

			memset(&pack,0,sizeof(pack));
			strcpy(pack.device,SN);
			strcpy(pack.datime,datime);
			pack.temp = temp;

			flag = 1;
		}

		/*判断网络状态 */
		if( net_status(fd) == 1 )
		{
			log_debug("The TCP network is normal!\n");
			g_status = 0;
		}
		else
		{
			log_debug("The TCP network is abnormal!\n");
			g_status = 1;
		}

		/*网络异常*/
		if( g_status )
		{		
			close(fd);
			
			/*断线重连*/
			if( (fd = client_connect(argp)) < 0 )
			{
				/* 重连失败*/
				log_debug("Client reconnect failure:%s\n",strerror(errno));
				if( (rv = insert_database(db,&pack)) < 0 )
				{
					log_warn("Failed to write data to the database:%s\n",strerror(errno));
				}
				log_debug("Start to continue to sampling!\n");
			}
			else
			{
				/*重连成功*/
				log_debug("Client reconnect successfully!\n");
				
				if( flag )
				{
					if( (rv = send_data(fd,buf,pack)) < 0 )
					{
						insert_database(db,&pack);
						log_warn("Client send data failure and write it to database:%s\n",buf);
					}
					else
					{
						log_info("Client send data:%s\n",buf);
					}
				}
				log_debug("Start to continue to sampling!\n");
			}

			continue;
		}

		/*网络正常*/
		else
		{
			if( flag )
			{
				if( (rv = send_data(fd,buf,pack)) < 0 )
				{
					insert_database(db,&pack);
					log_warn("Client send data failure and write it to database:%s\n",buf);
				}
				else
				{
					log_info("Client send data:%s\n",buf);
				}
			}
			
			/* 查看数据库中是否存在数据 */
			if( check_database(db) )
			{
				memset(&pack,0,sizeof(pack));
				if( (rv = get_database(db,&pack)) < 0 )
				{
					log_error("Client get data from database failure:%s\n",strerror(errno));
				}
				
				if( (rv = send_data(fd,buf,pack)) < 0 )
				{
					insert_database(db,&pack);
					log_warn("Client send data failure and write it to database:%s\n",buf);
				}
				else
				{
					log_info("Client send data:%s\n",buf);
				}

				if( (rv = delete_database(db)) < 0 )
				{
					log_error("Client delete data from database failure:%s\n",strerror(errno));
				}
			}
		}
	
	}
	
	close(fd);
	sqlite3_close(db);

	return 0;
}

