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
struct argument *parameter_analysis();

/* This structure is used to store information about parameters parsing */
struct argument{
	char	*ip;
	int		port;
	int		second;
};
#endif
