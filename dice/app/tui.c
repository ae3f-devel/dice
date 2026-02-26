#include "dice/tui.h"

#if ae2f_Sys_WIN(!)0
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

ctx_t dice_tui_ctx = {0};

dice_tui_status_t dice_tui_get_size(void)
{
#if ae2f_Sys_WIN(!)0
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    dice_tui_ctx.m_width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    dice_tui_ctx.m_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    dice_tui_ctx.m_width  = w.ws_col;
    dice_tui_ctx.m_height = w.ws_row;
#endif
    return DICE_TUI_OK;
}

dice_tui_status_t dice_tui_init(void)
{
    if (dice_tui_ctx.m_front){
        return DICE_TUI_INIT_RECALL;
    }

    if (tui_get_size() != DICE_TUI_OK){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    if (dice_tui_ctx.m_width == 0 || dice_tui_ctx.m_height == 0){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    dice_tui_ctx.m_front = stdout;

    fprintf(dice_tui_ctx.m_front, "\x1b[2J\x1b[H");
    fflush(dice_tui_ctx.m_front);

    size_t total = (size_t)dice_tui_ctx.m_width * (size_t)dice_tui_ctx.m_height;

    dice_tui_ctx.m_back  = malloc(total);
    dice_tui_ctx.m_prev  = malloc(total);

    if (!dice_tui_ctx.m_back || !dice_tui_ctx.m_prev){
        free(dice_tui_ctx.m_back);
        free(dice_tui_ctx.m_prev);

        return DICE_DICE_TUI_ERR_MEMORY_ALLOC;
    }
    
    memset(dice_tui_ctx.m_back, ' ', total);
    memset(dice_tui_ctx.m_prev, ' ', total);

    return DICE_TUI_OK;
}


dice_tui_status_t dice_tui_set_char(int x, int y, char c)
{
    if (!dice_tui_ctx.m_back){
        return DICE_TUI_ERR_NULL_POINTER;
    }

    if (x < 0 || y < 0){
        return DICE_TUI_ERR_INVALID_INPUT;
    }

    if ((ae2fsys_trmpos_t)x >= dice_tui_ctx.m_width || (ae2fsys_trmpos_t)y >= dice_tui_ctx.m_height){
        return DICE_TUI_ERR_OUT_OF_BOUNDS;
    }

    size_t idx = (size_t)y * (size_t)dice_tui_ctx.m_width + (size_t)x;
    dice_tui_ctx.m_back[idx] = c;

    return DICE_TUI_OK;
}


dice_tui_status_t dice_tui_render(void)
{
    if (!dice_tui_ctx.m_back || !dice_tui_ctx.m_prev){
        return DICE_TUI_ERR_NULL_POINTER;
    }

    size_t total = (size_t)dice_tui_ctx.m_width * (size_t)dice_tui_ctx.m_height;

    for (size_t i = 0; i < total; ++i) {
        char nb = dice_tui_ctx.m_back[i];

        if (nb != dice_tui_ctx.m_prev[i]) {
            size_t y = i / (size_t)dice_tui_ctx.m_width;
            size_t x = i % (size_t)dice_tui_ctx.m_width;
            
            fprintf(dice_tui_ctx.m_front, "\x1b[%zu;%zuH%c", y + 1, x + 1, nb);

            dice_tui_ctx.m_prev[i] = nb;
        }
    }

    fflush(dice_tui_ctx.m_front);

    return DICE_TUI_OK;
}