/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temperature.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 18:01:17"
 *                 
 ********************************************************************************/
#ifndef _TEMPERATURE_H_

#define _TEMPERATURE_H_

/* This function is used to get the temperature from the ds18b20 temperature sensor */
extern int get_temperature(float *temp);

#endif

