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

static sqlite3		*db;

sqlite3 *open_database(char *table_name)
{
	int			rv = -1;
	char 		sql[128];
	char		*errmsg = NULL;

	if( (rv = sqlite3_open("client.db",&db)) < 0 )
	{
		printf("Open database failure:%s\n",sqlite3_errmsg(db));
		return NULL;
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"create table if not exists %s(Device_name text, Sampling_time text,Sampling_temperature real)",table_name);
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Create table failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return NULL;
	}

	return db;
}

int insert_database(sqlite3 *db,char *table_name,pack_info_t *packp)
{
	int			rv = -1;
	char		sql[128];
	char		*errmsg = NULL;

	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into %s values('%s','%s','%f')",table_name,packp->device,packp->datime,packp->temp);
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("Insert into table failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	return 0;
}

int check_database(sqlite3 *db,char *table_name)
{
	int			rv = -1;
	char		**result;
	int			row = 0;
	int			column = 0;
	char		sql[128];
	char		*errmsg = NULL;

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from temperature");
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
			return row;
		}
		else
		{
			return 0;
		}
	}

}

int get_database(sqlite3 *db,char *table_name,pack_info_t *packp)
{
	int		rv = -1;
	char	**result;
	int		row = 0;
	int		column = 0;
	char	sql[128];
	char	*errmsg = NULL;

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from temperature ORDER BY ROWID ASC limit 1");
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

	return 0;
}

int delete_database(sqlite3 *db,char *table_name)
{
	int		rv = -1;
	char    **result;
	int     row = 0;
	int     column = 0;
	char	sql[128];
	char	*errmsg = NULL;

	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from temperature where ROWID IN (SELECT ROWID FROM temperature limit 1);");
	rv = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if( rv != SQLITE_OK )
	{
		printf("sqlite3 delete failure:%s\n",errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	return 0;
}

