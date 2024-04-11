/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  parameter.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 15:20:07"
 *                 
 ********************************************************************************/
#ifndef _PARAMETER_H_

#define _PARAMETER_H_

/* The function is used to print parameters to make relevant information */
extern void print_usage(char *parameter);

/*  This structure is used to store information about parameters parsing */
typedef struct arg_ctx_s
{
	char	*ip;
	int		port;
	int		second;
	int		daemon;
}arg_ctx_t;

/*   The function is used for parameters parsing */
extern int parameter_analysis(int argc,char *argv[],arg_ctx_t *argp,char **logfile,int *loglevel);

#endif
