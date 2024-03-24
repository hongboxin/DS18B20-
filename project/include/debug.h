/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  debug.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(23/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "23/03/24 15:51:27"
 *                 
 ********************************************************************************/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

//#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(format,...) printf("FILE: "__FILE__",FUNC: %s,LINE: %d: "format"\n",__func__,__LINE__,##__VA_ARGS__)

#else
#define DEBUG(format,...)

#endif

#endif

