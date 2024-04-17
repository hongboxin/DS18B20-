/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  time.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 18:05:25"
 *                 
 ********************************************************************************/
#ifndef _TIME_H_

#define _TIME_H_

/* This function is used to obtain the current system time  */
extern int get_time(char *datime,int size);

/*  This function is used for timing sampling */
extern int check_time(time_t *last_time,int interval);

#endif
