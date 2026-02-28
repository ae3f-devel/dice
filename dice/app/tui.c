#include "dice/tui.h"

#include <ae2f/Sys/Trm.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

dice_tui_ctx_t dice_tui_ctx = {0};

static dice_tui_status_t dice_tui_realloc_buffers(size_t total) {
    char *new_back = NULL;
    char *new_prev = NULL;

    new_back = malloc(total);
    new_prev = malloc(total);
    if (!(new_back && new_prev)) {
        free(new_back);
        free(new_prev);
        return DICE_TUI_ERR_MEMORY_ALLOC;
    }

    memset(new_back, ' ', total);
    memset(new_prev, '\0', total);

    free(dice_tui_ctx.m_back);
    free(dice_tui_ctx.m_prev);
    dice_tui_ctx.m_back = new_back;
    dice_tui_ctx.m_prev = new_prev;

    return DICE_TUI_OK;
}

dice_tui_status_t dice_tui_get_size(void) {
    size_t w, h;

    _ae2fsys_get_trm_size_simple_imp(L, dice_tui_ctx.m_width, dice_tui_ctx.m_height);

    w = (size_t)dice_tui_ctx.m_width;
    h = (size_t)dice_tui_ctx.m_height;

    if (w <= 0 || h <= 0) {
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    if(w * h < w || w * h < h){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    return DICE_TUI_OK;
}

dice_tui_status_t dice_tui_init(void) {
    dice_tui_status_t status;
    size_t total;
    size_t w = (size_t)dice_tui_ctx.m_width, h = (size_t)dice_tui_ctx.m_height;

    if (dice_tui_ctx.m_init_flag) {
        return DICE_TUI_INIT_RECALL;
    }

    if(w * h < w || w * h < h){
        return DICE_TUI_ERR_WINDOW_SIZE;
    }

    status = dice_tui_get_size();

    if (status != DICE_TUI_OK) {
        return status;
    }

    total = (size_t)w * (size_t)h;

    _ae2fsys_clear_trm_simple_imp(L);

    status = dice_tui_realloc_buffers(total);

    if (status != DICE_TUI_OK) {
        memset(&dice_tui_ctx, 0, sizeof(dice_tui_ctx));
        return status;
    }

    return DICE_TUI_OK;
}

dice_tui_status_t dice_tui_resize(void) {
    dice_tui_status_t status;
    size_t total;

    status = dice_tui_get_size();
    if (status != DICE_TUI_OK) {
        return status;
    }

    total = (size_t)dice_tui_ctx.m_width * (size_t)dice_tui_ctx.m_height;
    
    status = dice_tui_realloc_buffers(total);
    if (status != DICE_TUI_OK) {
        memset(&dice_tui_ctx, 0, sizeof(dice_tui_ctx));
        return status;
    }

    _ae2fsys_clear_trm_simple_imp(L);

    return DICE_TUI_OK;
}

ae2f_inline dice_tui_status_t dice_tui_set_char(ae2fsys_trmpos_t x, ae2fsys_trmpos_t y, char c) {
    size_t idx;

    if (!dice_tui_ctx.m_back) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    if (x < 0 || y < 0) {
        return DICE_TUI_ERR_INVALID_INPUT;
    }

    if ((ae2fsys_trmpos_t)x >= dice_tui_ctx.m_width || (ae2fsys_trmpos_t)y >= dice_tui_ctx.m_height) {
        return DICE_TUI_ERR_OUT_OF_BOUNDS;
    }

    idx = (size_t)y * (size_t)dice_tui_ctx.m_width + (size_t)x;
    dice_tui_ctx.m_back[idx] = c;

    return DICE_TUI_OK;
}

ae2f_inline dice_tui_status_t dice_tui_render(void) {
    size_t x, y, width, height;

    if (!(dice_tui_ctx.m_init_flag && dice_tui_ctx.m_back && dice_tui_ctx.m_prev)) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    width  = (size_t)dice_tui_ctx.m_width;
    height = (size_t)dice_tui_ctx.m_height;

    for (y = 0; y < height; ++y) {
        size_t row_offset = y * width;
        int cursor_set = 0; 

        for (x = 0; x < width; ++x) {
            size_t i = row_offset + x;
            char nb = dice_tui_ctx.m_back[i];

            if (nb != dice_tui_ctx.m_prev[i]) {
                if (!cursor_set) {
                    _ae2fsys_trm_goto_simple_imp(L, (ae2fsys_trmpos_t)(x + 1), (ae2fsys_trmpos_t)(y + 1));
                    cursor_set = 1;
                }
                fputc(nb, stdout);
                dice_tui_ctx.m_prev[i] = nb;
            } else {
                cursor_set = 0;
            }
        }
    }
    fflush(stdout);

    return DICE_TUI_OK;
}

ae2f_inline dice_tui_status_t dice_tui_clear(void) {
    size_t total;

    if (!(dice_tui_ctx.m_back && dice_tui_ctx.m_prev)) {
        return DICE_TUI_ERR_NULL_POINTER;
    }

    total = (size_t)dice_tui_ctx.m_width * (size_t)dice_tui_ctx.m_height;
    memset(dice_tui_ctx.m_back, ' ', total);
    memset(dice_tui_ctx.m_prev, '\0', total);

    return DICE_TUI_OK;
}

void dice_tui_shutdown(void) {
    free(dice_tui_ctx.m_back);
    free(dice_tui_ctx.m_prev);

    memset(&dice_tui_ctx, 0, sizeof(dice_tui_ctx));
}