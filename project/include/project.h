/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  project.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 20:06:15"
 *                 
 ********************************************************************************/
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <time.h>
#include "sqlite3.h"

/*This function is used to get the temperature from the ds18b20 temperature sensor */
extern int get_temperature(float *temp);

/*The function is used to print parameters to make relevant information */
extern void print_usage(char *parameter);

/* This structure is used to store information about parameters parsing */
typedef struct arg_ctx_s
{
	char	*ip;
	int		port;
	int		second;
}arg_ctx_t;

static arg_ctx_t		arg_1;
static arg_ctx_t		*argp;

/*  The function is used for parameters parsing */
extern arg_ctx_t *parameter_analysis(int argc,char *argv[]);

/*This function is used to obtain the current system time */
extern int get_time(char *datime);

/* This function is used for timing sampling */
extern int check_time(time_t *last_time,int interval);

/* This function is used to determine the network status*/
extern int net_status(int fd);

/*This function is used for client connection */
extern int client_connect(arg_ctx_t *argp);

/*This function is used for server connection */
extern int server_connect(int argc,char *argv[]);

/* This structure is used to hold the information that needs to be obtained */
typedef struct pack_info_s
{
	char	device[16];
	char	datime[32];
	float	temp;
}pack_info_t;

/*  This function is used to send data */
extern int send_data(int fd,char *buf,pack_info_t pack);

/* This function is used to install default signal */
extern void install_signal();

/* This function is used to handler default signal */
extern void signal_handler(int signal);

/*  */
extern int g_signal;

/*This function is used to create or open database and tables */
extern sqlite3 *open_database();
extern int create_database(char *database_name,char *table_name);

/*This function is used to insert data into the database*/
extern int insert_database(sqlite3 *db,pack_info_t *packp);
extern int write_database(char *database_name,char *table_name,pack_info_t *packp);

/*This function is used to check whether there is data in the database*/
extern int check_database(sqlite3 *db);

/*This function is used to get data from the database */
extern int get_database(sqlite3 *db,pack_info_t *packp);

/*This function is used to delete data from the database */
extern int delete_database(sqlite3 *db);

/*This function is used to set open fd max socket*/
extern void set_rlimit();

#endif
