#include "tui.h"

#if ae2f_Sys_WIN(!)0
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

ctx_t tui_ctx = {0};

tui_status_t tui_get_size(void)
{
#if ae2f_Sys_WIN(!)0
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.m_width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    tui_ctx.m_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.m_width  = w.ws_col;
    tui_ctx.m_height = w.ws_row;
#endif
    return TUI_OK;
}

tui_status_t tui_init(void)
{
    if (tui_ctx.m_front){
        return TUI_INIT_RECALL;
    }

    if (tui_get_size() != TUI_OK){
        return TUI_ERR_WINDOW_SIZE;
    }

    if (tui_ctx.m_width == 0 || tui_ctx.m_height == 0){
        return TUI_ERR_WINDOW_SIZE;
    }

    tui_ctx.m_front = stdout;

    fprintf(tui_ctx.m_front, "\x1b[2J\x1b[H");
    fflush(tui_ctx.m_front);

    size_t total = (size_t)tui_ctx.m_width * (size_t)tui_ctx.m_height;

    tui_ctx.m_back  = malloc(total);
    tui_ctx.m_prev  = malloc(total);

    if (!tui_ctx.m_back || !tui_ctx.m_prev){
        free(tui_ctx.m_back);
        free(tui_ctx.m_prev);

        return TUI_ERR_MEMORY_ALLOC;
    }
    
    memset(tui_ctx.m_back, ' ', total);
    memset(tui_ctx.m_prev, ' ', total);

    return TUI_OK;
}


tui_status_t tui_set_char(int x, int y, char c)
{
    if (!tui_ctx.m_back){
        return TUI_ERR_NULL_POINTER;
    }

    if (x < 0 || y < 0){
        return TUI_ERR_INVALID_INPUT;
    }

    if ((ae2fsys_trmpos_t)x >= tui_ctx.m_width || (ae2fsys_trmpos_t)y >= tui_ctx.m_height){
        return TUI_ERR_OUT_OF_BOUNDS;
    }

    size_t idx = (size_t)y * (size_t)tui_ctx.m_width + (size_t)x;
    tui_ctx.m_back[idx] = c;

    return TUI_OK;
}


tui_status_t tui_render(void)
{
    if (!tui_ctx.m_back || !tui_ctx.m_prev){
        return TUI_ERR_NULL_POINTER;
    }

    size_t total = (size_t)tui_ctx.m_width * (size_t)tui_ctx.m_height;

    for (size_t i = 0; i < total; ++i) {
        char nb = tui_ctx.m_back[i];

        if (nb != tui_ctx.m_prev[i]) {
            size_t y = i / (size_t)tui_ctx.m_width;
            size_t x = i % (size_t)tui_ctx.m_width;
            
            fprintf(tui_ctx.m_front, "\x1b[%zu;%zuH%c", y + 1, x + 1, nb);

            tui_ctx.m_prev[i] = nb;
        }
    }

    fflush(tui_ctx.m_front);

    return TUI_OK;
}