/*
MIT License

Copyright (c) 2015-2024 Thiago Adams [thradams]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifdef _WIN32

#include <windows.h>
#include <conio.h>
#include "conio.h"
#include <limits.h>
#include <stdbool.h>

/*windows 10 can use VT100*/

bool EnableVTMode() {
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}

static void clearbits(unsigned char * v,
  int bit_index,
  int nbits)
{
  unsigned mask = ~((unsigned char)(0)) << (sizeof(v) * CHAR_BIT - (unsigned char)(nbits));
  mask = mask >> (sizeof(v) * CHAR_BIT - (unsigned char)(bit_index)-(unsigned char)(nbits));
  *v &= ~mask;
}


static void setbits(unsigned char *v,
  int bit_index,
  int nbits,
  unsigned char number)
{
  clearbits(&number, nbits, sizeof(number) * CHAR_BIT - nbits);

  unsigned char big = number;
  big = (big << bit_index);

  clearbits(v, bit_index, nbits);
  *v |= big;
}


static unsigned char getbits(unsigned char v, int bit_index, int nbits)
{
  unsigned char r = v >> bit_index;
  clearbits(&r, nbits, sizeof(unsigned char) * CHAR_BIT - nbits);
  return r;
}


void c_gettextinfo(struct text_info *r)
{
  if (r == 0)
    return;

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  r->attribute = (unsigned char)csbi.wAttributes;
  r->curx = (unsigned char)csbi.dwCursorPosition.X;
  r->cury = (unsigned char)csbi.dwCursorPosition.Y;
  r->screenwidth = (unsigned char)csbi.dwMaximumWindowSize.X;
  r->screenheight = (unsigned char)csbi.dwMaximumWindowSize.X;
  r->normattr = 0;
}

int c_kbhit(void)
{
  return _kbhit();
}
int c_getch(void)
{
  return _getch();
}

int c_getche(void)
{
  return _getche();
}

void c_setcursortype(int cur_t)
{
  CONSOLE_CURSOR_INFO ci;

  switch (cur_t)
  {
  case _NOCURSOR://     (turns off the cursor)
    ci.bVisible = FALSE;
    ci.dwSize = 1;
    break;

  case _SOLIDCURSOR://  (solid block cursor)
    ci.bVisible = TRUE;
    ci.dwSize = 100;
    break;

  default:
  case _NORMALCURSOR: // (normal underscore cursor)
    ci.bVisible = TRUE;
    ci.dwSize = 50;
    break;
  }

  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

void c_textattr(int newattr)
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), newattr);
}

void c_textbackground(int newcolor)
{
  struct text_info ti;
  c_gettextinfo(&ti);
  unsigned char wColor = ti.attribute;
  unsigned char old = getbits(wColor, 4, 4);
  setbits(&wColor, 4, 4, newcolor);
  c_textattr(wColor);
}

void c_textcolor(int newcolor)
{
  struct text_info ti;
  c_gettextinfo(&ti);
  unsigned char wColor = ti.attribute;
  int old = getbits(wColor, 0, 4);
  setbits(&wColor, 0, 4, newcolor);
  c_textattr(wColor);
}


int c_wherex()
{
  CONSOLE_SCREEN_BUFFER_INFO cbsi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi))
  {
    return cbsi.dwCursorPosition.X + 1;
  }
  return -1;
}

int c_wherey()
{
  CONSOLE_SCREEN_BUFFER_INFO cbsi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi))
  {
    return cbsi.dwCursorPosition.Y;
  }
  return -1;
}

void c_gotoxy(int x, int y)
{
  COORD point;
  point.X = x - (short)1;
  point.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
}

void c_clrscr()
{
  COORD coordScreen = { 0, 0 };
  unsigned long cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  unsigned long dwConSize;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
  SetConsoleCursorPosition(hConsole, coordScreen);
}



#elif __linux__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "conio.h"

/*
** FIFO buffer for accumulated keystrokes
*/
static char stdin_buf[32];
static int stdin_head = 0;
static int stdin_tail = 0;

// getch version which handles accumulated keystrokes
int c_getch(void)
{
  int ch = EOF;

  if (stdin_head != stdin_tail)
  {
    ch = stdin_buf[stdin_head];
    stdin_head = (stdin_head + 1) % sizeof(stdin_buf);
  }
  else
  {
    ch = getchar();
  }
  return ch;
}

static void pushstdin(int ch)
{
  if(((stdin_head + 1) % sizeof(stdin_buf)) != stdin_tail)
  {
    stdin_buf[stdin_tail] = ch;
    stdin_tail = (stdin_tail + 1) % sizeof(stdin_buf);
  }
}

void c_disable_raw_mode(void)
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

void c_enable_raw_mode(void)
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
    atexit(c_disable_raw_mode);
}

// Internal function to check if a key has been pressed
static int _kbhit(void)
{
    int ch;
    int oldf;

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

// Export function with takes into account the accumulated keystrokes
int c_kbhit(void)
{
    int ch;
    int oldf;

    if (stdin_head != stdin_tail)
    {
        return 1;
    }
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

static int get_cursor_position(int *x, int *y)
{
  *x = -1;
  *y = -1;

  char buf[32];
  unsigned int i = 0;
  int ch;
  char *ptr;

  // Handle accumulated keystrokes
  while(_kbhit() == 1)
  {
    pushstdin(getchar());
  }
  
  // Request cursor position
  printf("\x1B[6n");

  // Read all chars until 'R' ("^[<v>;<h>R" expected)
  while(i < sizeof(buf) - 1)
  {
    // Use getchar without "_kbhit" to wait for the response
    ch = getchar();
    if (ch == EOF || ch == 'R') break;
    buf[i++] = ch;
  }
  buf[i] = '\0';
  
  // We have to take into account the keyboard input that has occurred in the meantime.
  // Therefore, we have to search for the start ESC sequence.
  ptr = strchr(buf, '\x1b');
  if (ptr == NULL) {
    pushstdin('X');
    return -1;
  }
  *ptr++ = 0;

  // Push keyboard inputs back into the stdin buffer
  i = 0;
  while(buf[i] != 0) {
    pushstdin(buf[i]);
    i++;
  }

  // Finally, parse cursor position
  if (*ptr != '[') {
    pushstdin(*ptr);
    return -1;
  }

  if (sscanf(++ptr, "%d;%d", y, x) != 2) {
    pushstdin('Z');
    return -1;
  }
  return 0;
}

int c_wherex(void)
{
  int x, y;
  get_cursor_position(&x, &y);
  return x;
}

int c_wherey(void)
{
  int x, y;
  get_cursor_position(&x, &y);
  return y;
}

void c_gotoxy(int x, int y)
{
  printf("\x1b[%d;%dH", y, x);
  fflush(stdout);
}

void c_clrscr()
{
  puts("\x1b[2J\x1b[1;1H");
  fflush(stdout);
}

void c_textcolor(int newcolor)
{
  //https://en.wikipedia.org/wiki/ANSI_escape_code

  const char * s = "\x1b[30m";

  switch (newcolor)
  {
  case BLACK:
    s = "\x1b[30m";
    break;

  case BLUE:
    s = "\x1b[34m";
    break;

  case GREEN:
    s = "\x1b[32m";
    break;

  case CYAN:
    s = "\x1b[36m";
    break;

  case RED:
    s = "\x1b[31;1m";
    break;

  case MAGENTA:
    s = "\x1b[35m";
    break;

  case BROWN:
    s = "\x1b[31m";
    break;

  case LIGHTGRAY:
    s = "\x1b[30;1m";
    break;

  case DARKGRAY:
    s = "\x1b[30m";
    break;

  case LIGHTBLUE:
    s = "\x1b[34;1m";
    break;

  case LIGHTGREEN:
    s = "\x1b[32,1m";;
    break;

  case LIGHTCYAN:
    s = "\x1b[36;1m";
    break;

  case LIGHTRED:
    s = "\x1b[31;1m";
    break;

  case LIGHTMAGENTA:
    s = "\x1b[35;1m";
    break;

  case YELLOW:
    s = "\x1b[33;1m";
    break;

  case WHITE:
    s = "\x1b[37;1m";
    break;

  case BLINK:
    s = "\x1b[30m";
    break;
  };

  printf("%s", s);
}

void c_textbackground(int newcolor)
{
  //https://en.wikipedia.org/wiki/ANSI_escape_code

  const char * s = "\x1b[40m";

  switch (newcolor)
  {
  case BLACK:
    s = "\x1b[40m";
    break;

  case BLUE:
    s = "\x1b[44m";
    break;

  case GREEN:
    s = "\x1b[42m";
    break;

  case CYAN:
    s = "\x1b[46m";
    break;

  case RED:
    s = "\x1b[41;1m";
    break;

  case MAGENTA:
    s = "\x1b[45m";
    break;

  case BROWN:
    s = "\x1b[41m";
    break;

  case LIGHTGRAY:
    s = "\x1b[40;1m";
    break;

  case DARKGRAY:
    s = "\x1b[40m";
    break;

  case LIGHTBLUE:
    s = "\x1b[44;1m";
    break;

  case LIGHTGREEN:
    s = "\x1b[42,1m";;
    break;

  case LIGHTCYAN:
    s = "\x1b[46;1m";
    break;

  case LIGHTRED:
    s = "\x1b[41;1m";
    break;

  case LIGHTMAGENTA:
    s = "\x1b[45;1m";
    break;

  case YELLOW:
    s = "\x1b[43;1m";
    break;

  case WHITE:
    s = "\x1b[47;1m";
    break;

  case BLINK:
    s = "\x1b[40m";
    break;
  };

  puts(s);
}

void c_setcursortype(int cur_t)
{
  switch (cur_t)
  {
  case _NOCURSOR:
    printf("\x1b[?25l");
    break;

  case _NORMALCURSOR:
    printf("\x1b[?25h");
    break;

  case _SOLIDCURSOR://TODO
    printf("\x1b[?25h");
    break;
  }
}

void c_gettextinfo(struct text_info *r)
{

  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  r->screenheight = w.ws_row;
  r->screenwidth = w.ws_col;

  int x, y;
  get_cursor_position(&x, &y);


  r->curx = x;
  r->cury = y;

}

void c_textattr(int newattr)
{
  //tODO
}

/* msleep(): Sleep for the requested number of milliseconds. */
int c_msleep(int msec)
{
    struct timespec ts;
    int res;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

#endif //linux
