// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef ADD_H
#define ADD_H
#include <termios.h>
#include <stdio.h>
// This is the content of the .h file, which is where the declarations go
static struct termios old, newt;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  newt = old; /* make new settings same as old settings */
  newt.c_lflag &= ~ICANON; /* disable buffered i/o */
  newt.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newt); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
} 
// This is the end of the header guard
#endif
