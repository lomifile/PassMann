/*
 * =====================================================================================
 *
 *       Filename:  RawMode.h
 *
 *    Description: Header file for initi raw mode 
 *
 *        Version:  1.0
 *        Created:  04.09.2021 09:22:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef PASSMANN_RAW_MODE_H
#define PASSMANN_RAW_MODE_H

#include <stdio.h>
#include <stdarg.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

extern struct termios orig_termios;

void init_raw_mode();

#endif
