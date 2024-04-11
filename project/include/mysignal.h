/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  signal.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 18:07:52"
 *                 
 ********************************************************************************/

#ifndef _SIGNAL_H_

#define _SIGNAL_H_

/*  This function is used to install default signal  */
extern void install_signal();

/*  This function is used to handler default signal  */
extern void signal_handler(int signal);

extern int g_signal;

#endif
