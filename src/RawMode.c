/*
 * =====================================================================================
 *
 *       Filename:  RawMode.c
 *
 *    Description: Initializes Raw mode 
 *
 *        Version:  1.0
 *        Created:  04.09.2021 09:18:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Filip Ivanuse, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "RawMode.h"


struct termios orig_termios;

void disabe_raw_mode()
{
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        exit(1);
    }
}

void init_raw_mode()
{
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) exit(-1);
  atexit(disabe_raw_mode);  

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ISIG);

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) exit(-1);
}
