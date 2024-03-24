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

/*This function is used to get the temperature from the ds18b20 temperature sensor */
int get_temperature(float *temp);

/*The function is used to print parameters to make relevant information */
void print_usage(char *parameter);

/* The function is used for parameters parsing */
struct argument *parameter_analysis(int argc,char *argv[]);

/* This structure is used to store information about parameters parsing */
struct argument
{
	char	*ip;
	int		port;
	int		second;
};

static struct argument		arg_1;
static struct argument		*argp;

/*This function is used to obtain the current system time */
int get_time(char *datime);

/* This function is used to determine the network status*/
int net_status(int fd);

/*This function is used for client connection */
int client_connect(int fd,struct argument *argp);

/*This function is used for server connection */
int server_connect(int argc,char *argv[]);

/* This structure is used to hold the information that needs to be obtained */
struct pack
{
	char	device[16];
	char	datime[32];
	float	temp;
};

/*This function is used to create or open database and tables */
int create_database(char *database_name,char *table_name);

/*This function is used to insert data into the database*/
int insert_database(char *database_name,char *table_name,struct pack *packp);

/*This function is used to check whether there is data in the database*/
int check_database(char *database_name,char *table_name);

/*This function is used to get data from the database */
int get_database(char *database_name,char *table_name,struct pack *packp);

/*This function is used to delete data from the database */
int delete_database(char *database_name,char *table_name);

/*This function is used to set open fd max socket*/
void set_rlimit();

#endif
