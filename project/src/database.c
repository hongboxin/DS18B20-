/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 12:03:54"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "project.h"

sqlite3		*db;
char		sql[128];
char		*errmsg = NULL;

int	create_database(char *database_name,char *table_name)
{
	int			rv = -1;

	rv = sqlite3_open(database_name,&db);
	if( rv != SQLITE_OK )
	{
		printf("Create or open database failure:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("Create or open database successfully!\n");

	memset(sql,0,sizeof(sql));
	sprintf(sql,"create table if not exists %s(Device_name text,Sampling_time text,Sampling_temperature real)",table_name);
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Create table failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}
	printf("Create table successfully!\n");

	sqlite3_close(db);
	return 0;
}

int insert_database(char *database_name,char *table_name,struct pack *packp)
{
	int			rv = -1;

	rv = sqlite3_open(database_name,&db);
	if( rv != SQLITE_OK )
	{
		printf("Open database failure during inserting:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into %s values('%s','%s','%f')",table_name,packp->device,packp->datime,packp->temp);
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Insert into table failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	sqlite3_close(db);
	return 0;
}

int check_database(char *database_name,char *table_name)
{
	int			rv = -1;
	char		**result;
	int			row = 0;
	int			column = 0;

	rv = sqlite3_open(database_name,&db);
	if( rv != SQLITE_OK )
	{
		printf("Open database failure during checking:%s\n",sqlite3_close(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from %s",table_name);
	rv = sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Check database failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}
	else
	{
		if( row > 0 )
		{
			printf("There is data in the database!\n");
			return row;
		}
		else
		{
			printf("There is not data in the database!\n");
			return 0;
		}
	}

}

int get_database(char *database_name,char *table_name,struct pack *packp)
{
	int		rv = -1;
	char	**result;
	int		row = 0;
	int		column = 0;

	rv = sqlite3_open(database_name,&db);
	if( rv != SQLITE_OK )
	{
		printf("Open database failure during getting:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from %s ORDER BY POWID ASC limit 1",table_name);
	rv = sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Check database failure during getting:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	memset(packp,0,sizeof(packp));
	strcpy(packp->device,result[3]);
	strcpy(packp->datime,result[4]);
	packp->temp = atof(result[5]);
	printf("packp->device:%s,packp->datime:%s,packp->temper:%f\n",packp->device,packp->datime,packp->temp);

	sqlite3_close(db);
	return 0;
}

int delete_database(char *database_name,char *table_name)
{
	int		rv = -1;

	rv = sqlite3_open(database_name,&db);
	if( rv != SQLITE_OK )
	{
		printf("Open database failure during deleting:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from %s limit 1",table_name);
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3 delete failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}
	printf("Delete data successfully!\n");

	return 0;
}

