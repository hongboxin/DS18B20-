/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  database.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 18:30:17"
 *                 
 ********************************************************************************/
#ifndef _DATABASE_H_

#define _DATABASE_H_

/* This function is used to create or open database and tables*/
extern sqlite3 *open_database(char *table_name);

/*  This function is used to create or open database and tables for server */
extern int create_database(char *database_name,char *table_name);

/* This function is used to insert data into the database*/
extern int insert_database(sqlite3 *db,char *table_name,pack_info_t *packp);

/*  This function is used to insert data into the database */
extern int write_database(char *database_name,char *table_name,pack_info_t *packp);

/* This function is used to check whether there is data in the database*/
extern int check_database(sqlite3 *db,char *table_name);

/* This function is used to get data from the database  */
extern int get_database(sqlite3 *db,char *table_name,pack_info_t *packp);

/* This function is used to delete data from the database */
extern int delete_database(sqlite3 *db,char *table_name);

#endif
