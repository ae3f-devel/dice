#include "dice/tui.h"

#include <ae2f/Sys/Trm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

dice_tui_ctx_t dice_tui_ctx = {0};

dice_tui_status_t dice_tui_get_size(void)
{
    _ae2fsys_get_trm_size_simple_imp(L, dice_tui_ctx.m_width, dice_tui_ctx.m_height);

    ae2f_unexpected_but_if(dice_tui_ctx.m_width == 0 || dice_tui_ctx.m_height == 0) {
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    {
        const size_t w = (size_t)dice_tui_ctx.m_width;
        const size_t h = (size_t)dice_tui_ctx.m_height;

        ae2f_unexpected_but_if(h != 0 && w > SIZE_MAX / h) {
            return DICE_TUI_ERR_WINDOW_SIZE;
        }
    }
    return DICE_TUI_OK;
}

dice_tui_status_t dice_tui_init(void)
{
    size_t total, width, height;

    ae2f_unexpected_but_if(dice_tui_ctx.m_front) {
        return DICE_TUI_INIT_RECALL;
    }

    ae2f_unexpected_but_if(dice_tui_get_size() != DICE_TUI_OK) {
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    width  = (size_t)dice_tui_ctx.m_width;
    height = (size_t)dice_tui_ctx.m_height;

    total = width * height;
    dice_tui_ctx.m_front = stdout;

    _ae2fsys_clear_trm_simple_imp(L);
    fflush(dice_tui_ctx.m_front);

    dice_tui_ctx.m_back = malloc(total);
    dice_tui_ctx.m_prev = malloc(total);

    ae2f_unexpected_but_if(!dice_tui_ctx.m_back || !dice_tui_ctx.m_prev) {
        free(dice_tui_ctx.m_back);
        free(dice_tui_ctx.m_prev);

        memset(&dice_tui_ctx, 0, sizeof (dice_tui_ctx));
        return DICE_TUI_ERR_MEMORY_ALLOC;
    }
    
    memset(dice_tui_ctx.m_back, ' ', total);
    memset(dice_tui_ctx.m_prev, ' ', total);

    return DICE_TUI_OK;
}


ae2f_inline dice_tui_status_t dice_tui_set_char(int x, int y, char c)
{
    size_t idx;

    ae2f_unexpected_but_if(!dice_tui_ctx.m_back) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    ae2f_unexpected_but_if(x < 0 || y < 0) {
        return DICE_TUI_ERR_INVALID_INPUT;
    }

    ae2f_unexpected_but_if((ae2fsys_trmpos_t)x >= dice_tui_ctx.m_width || (ae2fsys_trmpos_t)y >= dice_tui_ctx.m_height) {
        return DICE_TUI_ERR_OUT_OF_BOUNDS;
    }

    idx = (size_t)y * (size_t)dice_tui_ctx.m_width + (size_t)x;
    dice_tui_ctx.m_back[idx] = c;

    return DICE_TUI_OK;
}

ae2f_inline dice_tui_status_t dice_tui_render(void)
{
    const size_t width  = (size_t)dice_tui_ctx.m_width;
    const size_t height = (size_t)dice_tui_ctx.m_height;
    size_t i;

    ae2f_unexpected_but_if(!dice_tui_ctx.m_front || !dice_tui_ctx.m_back || !dice_tui_ctx.m_prev) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    for (i = 0; i < width * height; ++i) {
        char nb = dice_tui_ctx.m_back[i];

        if (nb != dice_tui_ctx.m_prev[i]) {
            _ae2fsys_trm_goto_simple_imp(L, (ae2fsys_trmpos_t)(i % width + 1), (ae2fsys_trmpos_t)(i / width + 1));

            fputc(nb, dice_tui_ctx.m_front);

            dice_tui_ctx.m_prev[i] = nb;
        }
    }

    fflush(dice_tui_ctx.m_front);

    return DICE_TUI_OK;
}

ae2f_inline dice_tui_status_t dice_tui_clear(void)
{
    size_t total = (size_t)dice_tui_ctx.m_width * (size_t)dice_tui_ctx.m_height;

    ae2f_unexpected_but_if(!dice_tui_ctx.m_back || !dice_tui_ctx.m_prev) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    memset(dice_tui_ctx.m_back, ' ', total);
    memset(dice_tui_ctx.m_prev, '\0', total);

    return DICE_TUI_OK;
}

void dice_tui_shutdown(void)
{
    free(dice_tui_ctx.m_back);
    free(dice_tui_ctx.m_prev);

    memset(&dice_tui_ctx, 0, sizeof(dice_tui_ctx));
}