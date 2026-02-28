#ifndef dice_tui_h
#define dice_tui_h

#include <ae2f/Sys/Trm.h>
#include <stdio.h>
#include "./tui/err.h"

typedef struct {
    ae2fsys_trmpos_t m_width;
    ae2fsys_trmpos_t m_height;
    int m_init_flag;
    char* m_back;
    char* m_prev;
} dice_tui_ctx_t;

extern dice_tui_ctx_t dice_tui_ctx;

dice_tui_status_t dice_tui_get_size(void);
dice_tui_status_t dice_tui_init(void);
dice_tui_status_t dice_tui_resize(void);
ae2f_inline dice_tui_status_t dice_tui_set_char(ae2fsys_trmpos_t x, ae2fsys_trmpos_t y, char c);
ae2f_inline dice_tui_status_t dice_tui_render(void);
ae2f_inline dice_tui_status_t dice_tui_clear(void);

void dice_tui_shutdown(void);
#endif