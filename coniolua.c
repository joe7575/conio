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
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include "lua.h"
#include "lauxlib.h"
#include "conio.h"

static const char *colors[] = {
    "BLACK",
    "BLUE",
    "GREEN",
    "CYAN",
    "RED",
    "MAGENTA",
    "BROWN",
    "LIGHTGRAY",
    "DARKGRAY",
    "LIGHTBLUE", 
    "LIGHTGREEN",
    "LIGHTCYAN",
    "LIGHTRED",
    "LIGHTMAGENTA",
    "YELLOW",
    "WHITE",
    "BLINK",
     NULL
};

static const char *cursortype[] = {
    "NOCURSOR",
    "SOLIDCURSOR",
    "NORMALCURSOR"
};

static int version(lua_State *L) {
    lua_pushstring(L, SVERSION);
    return 1;
}

static int getch(lua_State *L) {
    int ch = c_getch();
    lua_pushinteger(L, ch);
    return 1;
}

static int kbhit(lua_State *L) {
    int ch = c_kbhit();
    lua_pushboolean(L, ch == 0 ? 0 : 1);
    return 1;
}

static int clrscr(lua_State *L) {
    c_clrscr();
    return 0;
}

static int gotoxy(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    c_gotoxy(x, y);
    return 0;
}

static int setcursortype(lua_State *L) {
    int ctype = luaL_checkoption(L, 1, NULL, cursortype);
    c_setcursortype(ctype);
    return 0;
}

static int textbackground(lua_State *L) {
    int newcolor = luaL_checkoption(L, 1, NULL, colors);
    c_textbackground(newcolor);
    return 0;
}

static int textcolor(lua_State *L) {
    int newcolor = luaL_checkoption(L, 1, NULL, colors);
    c_textcolor(newcolor);
    return 0;
}

static int wherex(lua_State *L) {
    int x = c_wherex();
    lua_pushinteger(L, x);
    return 1;
}

static int wherey(lua_State *L) {
    int y = c_wherey();
    lua_pushinteger(L, y);
    return 1;
}

static int gettextinfo(lua_State *L) {
    struct text_info r;
    c_gettextinfo(&r);
    lua_newtable(L);
    lua_pushstring(L, "attribute");
    lua_pushinteger(L, r.attribute);
    lua_settable(L, -3);
    lua_pushstring(L, "normattr");
    lua_pushinteger(L, r.normattr);
    lua_settable(L, -3);
    lua_pushstring(L, "screenheight");
    lua_pushinteger(L, r.screenheight);
    lua_settable(L, -3);
    lua_pushstring(L, "screenwidth");
    lua_pushinteger(L, r.screenwidth);
    lua_settable(L, -3);
    lua_pushstring(L, "curx");
    lua_pushinteger(L, r.curx);
    lua_settable(L, -3);
    lua_pushstring(L, "cury");
    lua_pushinteger(L, r.cury);
    lua_settable(L, -3);
    return 1;
}

static int textattr(lua_State *L) {
    int newattr = luaL_checkinteger(L, 1);
    c_textattr(newattr);
    return 0;
}

static int reset(lua_State *L) {
    printf("\x1b[m");
    return 0;
}

static int disable_raw_mode(lua_State *L)
{
    c_disable_raw_mode();
    return 0;
}

static int enable_raw_mode(lua_State *L)
{
    c_enable_raw_mode();
    return 0;
}

static int msleep(lua_State *L)
{
    int msec = luaL_checkinteger(L, 1);
    c_msleep(msec);
    return 0;
}

static const luaL_Reg R[] = {
    {"version",                 version},
    {"getch",                   getch},
    {"kbhit",                   kbhit},
    {"clrscr",                  clrscr},
    {"gotoxy",                  gotoxy},
    {"setcursortype",           setcursortype},
    {"textbackground",          textbackground},
    {"textcolor",               textcolor},
    {"wherex",                  wherex},
    {"wherey",                  wherey},
    {"gettextinfo",             gettextinfo},
    {"textattr",                textattr},
    {"reset",                   reset},
    {"msleep",                  msleep},
    {"enable_raw_mode",         enable_raw_mode},
    {"disable_raw_mode",        disable_raw_mode},
    {NULL, NULL}
};

/* }====================================================== */



LUALIB_API int luaopen_conio(lua_State *L) {
    lua_newtable(L);
    luaL_register(L, NULL, R);
    return 1;
}
