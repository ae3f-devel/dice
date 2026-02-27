#include <stdio.h>
#include <stdlib.h>
#include "dice/tui.h"

#if ae2f_Sys_WIN(!)0
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

static struct termios orig_term;

static void restore_term(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}
#endif

int main(void)
{
    char text[] = "Hello, World";
    int i = 0;
    int ch;

    if (dice_tui_init() != DICE_TUI_OK) {
        return 1;
    }

    for(i = 0;i<12;i++){
        dice_tui_set_char(dice_tui_ctx.m_width / 2 - 6 + i, dice_tui_ctx.m_height / 2, text[i]);
    }

    dice_tui_render();

#if defined(_WIN32)
    {
        HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;

        GetConsoleMode(h, &mode);
        SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    }
#else
    atexit(restore_term);
    tcgetattr(STDIN_FILENO, &orig_term);
    {
        struct termios raw = orig_term;
        raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
#endif

    while (1) {
#if ae2f_Sys_WIN(!)0
        ch = _getch();
#else
        ch = getchar();

        if (ch == EOF) {
            break;
        }
#endif
        if (ch == 'q') {
            dice_tui_clear();
            dice_tui_render();
            break;
        }

        if (ch == '\r' || ch == '\n') {
            continue;
        }
    }
    
    dice_tui_shutdown();

    return 0;
}