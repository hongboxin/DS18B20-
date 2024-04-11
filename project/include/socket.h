/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 18:39:26"
 *                 
 ********************************************************************************/
#ifndef _SOCKET_H_

#define _SOCKET_H_

/*  This function is used to determine the network status */
extern int net_status(int fd);

/* This function is used for client connection */
extern int client_connect(arg_ctx_t *argp);

/* This function is used for server connection  */
extern int server_connect(char *ip,int port);

/*   This function is used to send data  */
extern int send_data(int fd,char *buf,pack_info_t pack);

#endif
