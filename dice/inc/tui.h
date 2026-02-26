#ifndef DICE_TUI_H
#define DICE_TUI_H

#include <ae2f/Sys/Trm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "tui/err.h"

#if ae2f_Sys_WIN(!)0
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

typedef struct {
    ae2fsys_trmpos_t width;
    ae2fsys_trmpos_t height;
    FILE* front;
    char* back;
    char* prev;
} ctx_t;

ctx_t tui_ctx = {0};

static ae2f_inline tui_status_t tui_get_size(void)
{
#if ae2f_Sys_WIN(!)0
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    tui_ctx.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.width  = w.ws_col;
    tui_ctx.height = w.ws_row;
#endif
    return TUI_OK;
}

static ae2f_inline tui_status_t tui_init(void)
{
    if (tui_ctx.front){
        return TUI_INIT_RECALL;
    }

    if (tui_get_size() != TUI_OK){
        return TUI_ERR_WINDOW_SIZE;
    }

    if (tui_ctx.width == 0 || tui_ctx.height == 0){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.front = stdout;

    fprintf(tui_ctx.front, "\x1b[2J\x1b[H");
    fflush(tui_ctx.front);

    size_t total = (size_t)tui_ctx.width * (size_t)tui_ctx.height;

    tui_ctx.back  = malloc(total);
    tui_ctx.prev  = malloc(total);

    if (!tui_ctx.back || !tui_ctx.prev){
        free(tui_ctx.back);
        free(tui_ctx.prev);

        return TUI_ERR_MEMORY_ALLOC;
    }
    
    memset(tui_ctx.back, ' ', total);
    memset(tui_ctx.prev, ' ', total);

    return TUI_OK;
}

static ae2f_inline tui_status_t tui_set_char(int x, int y, char c)
{
    if (!tui_ctx.back){
        return TUI_ERR_NULL_POINTER;
    }

    if (x < 0 || y < 0){
        return TUI_ERR_INVALID_INPUT;
    }

    if ((ae2fsys_trmpos_t)x >= tui_ctx.width || (ae2fsys_trmpos_t)y >= tui_ctx.height){
        return TUI_ERR_OUT_OF_BOUNDS;
    }

    size_t idx = (size_t)y * (size_t)tui_ctx.width + (size_t)x;
    tui_ctx.back[idx] = c;

    return TUI_OK;
}

static ae2f_inline tui_status_t tui_render(void)
{
    if (!tui_ctx.back || !tui_ctx.prev){
        return TUI_ERR_NULL_POINTER;
    }

    size_t total = (size_t)tui_ctx.width * (size_t)tui_ctx.height;

    for (size_t i = 0; i < total; ++i) {
        char nb = tui_ctx.back[i];

        if (nb != tui_ctx.prev[i]) {
            size_t y = i / (size_t)tui_ctx.width;
            size_t x = i % (size_t)tui_ctx.width;
            
            fprintf(tui_ctx.front, "\x1b[%zu;%zuH%c", y + 1, x + 1, nb);

            tui_ctx.prev[i] = nb;
        }
    }

    fflush(tui_ctx.front);

    return TUI_OK;
}
#endif