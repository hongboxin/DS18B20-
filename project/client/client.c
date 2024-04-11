/*********************************************************************************
 *		Copyright:	(C) 2024 linuxer<linuxer@email.com>
 *					All rights reserved.
 *
 *		 Filename:	client.c
 *	  Description:	This file 
 *				   
 *		  Version:	1.0.0(15/03/24)
 *		   Author:	linuxer <linuxer@email.com>
 *		ChangeLog:	1, Release initial version on "15/03/24 10:51:39"
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
#include "database.h"
#include "mysignal.h"
#include "mytime.h"
#include "parameter.h"
#include "socket.h"
#include "temperature.h"

#define SN			"rpi#0001"
#define table_name	"temperature"

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
	char				*logfile="client.log";
	int					loglevel=LOG_LEVEL_INFO;
	int					logsize=10;
	arg_ctx_t			args;
	sqlite3				*db;
	
	/* 参数解析 */
	if( (rv = parameter_analysis(argc,argv,&args,&logfile,&loglevel)) < 0 )
	{
		printf("Client parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}

	/*判断是否以守护进程形式运行程序 */
	if( args.daemon )
	{
		if( daemon(1,0) < 0 )
		{
			printf("Running daemon failure:%s\n", strerror(errno));
			return 0;
		}
	}

	/* 安装信号 */
	install_signal();

	/*打开日志系统 */
	if( log_open(logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		printf("Initial log system failed:%s\n",strerror(errno));
		return -1;
	}

	/* 打开数据库 */
	if( !(db = open_database(table_name)) )
	{
		log_error("Client open database failure:%s\n",strerror(errno));
		return -1;
	}

	while( !g_signal )
	{
		flag = 0;
		/* 判断是否进行采样 */
		if( check_time(&last_time,args.second) )
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

			if(flag)
			{
				if( (rv = send_data(fd,buf,pack)) < 0 )
				{
					insert_database(db,table_name,&pack);
					log_warn("Client send data failure and write it to database:%s\n",buf);
				}
				else
				{
					log_info("Client send data:%s\n",buf);
				}
			}

			/*	查看数据库中是否存在数据 */
			if( check_database(db,table_name) )
			{
				memset(&pack,0,sizeof(pack));
				if( (rv = get_database(db,table_name,&pack)) < 0 )
				{
					log_error("Client get data from database failure:%s\n",strerror(errno));
					continue;
				}
				else
				{
					if( (rv = send_data(fd,buf,pack)) < 0 )
					{
						insert_database(db,table_name,&pack);
						log_warn("Client send data failure and write it to database:%s\n",buf);
						continue;
					}
					else
					{
						log_info("Client send data:%s\n",buf);
						if( (rv = delete_database(db,table_name)) < 0 )
						{
							log_error("Client delete data from database failure:%s\n",strerror(errno));
							continue;
						}
					}
				}
			}
			
		}
		/*	网络异常 */
		else
		{
			log_debug("The TCP network is abnormal!\n");
			close(fd);

			/* 重连 */
			if( (fd = client_connect(&args)) < 0 )
			{
				/*	重连失败 */
				log_debug("Client reconnect failure:%s\n",strerror(errno));
				if( flag )
				{
					insert_database(db,table_name,&pack);
					log_warn("Failed to write data to the database:%s\n",strerror(errno));
				}
			}
			else
			{
				/* 重连成功 */
				log_debug("Client reconnect successfully!\n");
				
				if( flag )
				{
					if( (rv = send_data(fd,buf,pack)) < 0 )
					{
						insert_database(db,table_name,&pack);
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

	}
	
	close(fd);
	sqlite3_close(db);

	return 0;
}

