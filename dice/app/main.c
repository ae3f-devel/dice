#include <stdio.h>
#include <string.h>
#include "dice/tui.h"

#if ae2f_Sys_WIN(!)0
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

int main(void)
{
    int start_x, start_y, i;
    int ch;
    int xpos = 0, ypos = 1;

#if ae2f_Sys_WIN(!)0
    HANDLE hStdin;
    DWORD origMode, rawMode;
#endif
    char *msg;

    if (dice_tui_init() != DICE_TUI_OK) {
        fprintf(stderr, "failed to initialize TUI\n");
        return 1;
    }

    msg = "Hello, world!";

    start_x = ((int)dice_tui_ctx.m_width - (int)strlen(msg)) / 2;
    start_y = ((int)dice_tui_ctx.m_height) / 2;

    for (i = 0; msg[i]; ++i) {
        dice_tui_set_char(start_x + i, start_y, msg[i]);
    }

    dice_tui_render();

#if ae2f_Sys_WIN(!)0
    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (hStdin == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "unable to get stdin handle\n");
        return 1;
    }

    if (!GetConsoleMode(hStdin, &origMode)) {
        fprintf(stderr, "GetConsoleMode failed\n");
        return 1;
    }

    rawMode = origMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

    SetConsoleMode(hStdin, rawMode);
#else
    {
        struct termios orig, raw;

        tcgetattr(STDIN_FILENO, &orig);

        raw = orig;
        raw.c_lflag &= (tcflag_t)~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
#endif

    while (1) {
#if ae2f_Sys_WIN(!)0
        ch = _getch();
#else
        ch = getchar();
#endif
        if (ch == 'q'){
            break;
        }

        if (ch == '\r' || ch == '\n'){
            continue;
        }

        dice_tui_set_char(xpos, ypos, (char)ch);
        dice_tui_render();

        xpos++;
        if (xpos >= dice_tui_ctx.m_width){
            break;
        }
    }

#if ae2f_Sys_WIN(!)0
    SetConsoleMode(hStdin, origMode);
#else
    {
        struct termios orig;

        tcgetattr(STDIN_FILENO, &orig);

        orig.c_lflag |= (ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &orig);
    }
#endif
    printf("\nbye\n");
    return 0;
}
