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
	int					fd = -1;
	int					rv = -1;
	pack_info_t			pack;
	char				buf[1024];
	int					flag = 0;
	time_t				last_time = 0;
	char				*logfile="client.log";
	int					loglevel=LOG_LEVEL_INFO;
	int					logsize=10;
	arg_ctx_t			args;
	sqlite3				*db;
	
	/* Parameter analysis */
	if( (rv = parameter_analysis(argc,argv,&args,&logfile,&loglevel)) < 0 )
	{
		printf("Client parameter analysis failure:%s\n",strerror(errno));
		return -1;
	}

	/*Determines whether to run the program as a daemon */
	if( args.daemon )
	{
		if( daemon(1,0) < 0 )
		{
			printf("Running daemon failure:%s\n", strerror(errno));
			return 0;
		}
	}

	/* Install default signal */
	install_signal();
	
	/*Open the log system */
	if( log_open(logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		printf("Initial log system failed:%s\n",strerror(errno));
		return -1;
	}

	/* open database */
	if( !(db = open_database(table_name)) )
	{
		log_error("Client open database failure:%s\n",strerror(errno));
		return -1;
	}

	while( !g_signal )
	{
		flag = 0;
		/* Determine whether to sample */
		if( check_time(&last_time,args.second) )
		{
			memset(&pack,0,sizeof(pack));
			/* Sampling acquisition time*/
			if( (rv = get_time(pack.datime,DT_LENGTH)) < 0 )
			{
				log_error("The client failed to obtain the system time:%s\n",strerror(errno));
			}
			log_debug("The client obtain the system time successfully!\n");

			/* Get sampling temperature */
			if( (rv = get_temperature(&pack.temp)) < 0 )
			{
				log_error("The client failed to obtain the temperature:%s\n",strerror(errno));
			}
			log_debug("The client obtain the temperature successfully!\n");

			strncpy(pack.device,SN,SN_LENGTH);

			flag = 1;
		}
		/* Net is abnormal */
		if( net_status(fd) == 0 )
		{
			/* Close the file descriptor */
		    if( fd >= 0 )
			{
				if(	socket_close(&fd) < 0 )
				{
					log_error("Close fd failure:%s\n",strerror(errno));
					continue;
				}
			
			}
			/* Connect failure */
			if( (fd = client_connect(&args)) < 0 )
			{
				log_debug("Client reconnect failure:%s\n",strerror(errno));
				if( flag )
				{
					insert_database(db,table_name,&pack);
					log_warn("Net is abnormal and write data to the database:%s\n",strerror(errno));
				}
				continue;
			}
		}

		/* Net is normal or conncet successfully */
		log_debug("The TCP network is normal!\n");

		if( flag )
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
			}
		}

		/* Check whether data exists in the database */
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
				if( (rv = delete_database(db,table_name)) < 0 )
				{
					log_error("Client delete data from database failure:%s\n",strerror(errno));
					continue;
				}
			}

			if( (rv = send_data(fd,buf,pack)) < 0 )
			{
				insert_database(db,table_name,&pack);
				log_warn("Client send data failure and write it to database:%s\n",buf);
				continue;
			}
			else
			{
				log_info("Client send data:%s\n",buf);
			}
			
		}
			
	}
	
	close(fd);
	sqlite3_close(db);

	return 0;
}

