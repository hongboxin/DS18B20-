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

#define SN_LENGTH		16
#define DT_LENGTH 		32

/* This structure is used to hold the information that needs to be obtained */
typedef struct pack_info_s
{
	char	device[SN_LENGTH];
	char	datime[DT_LENGTH];
	float	temp;
}pack_info_t;

#endif
