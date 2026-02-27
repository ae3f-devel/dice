#ifndef dice_tui_h
#define dice_tui_h

#include <ae2f/Sys/Trm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "dice/tui/err.h"

typedef struct {
    ae2fsys_trmpos_t m_width;
    ae2fsys_trmpos_t m_height;
    FILE* m_front;
    char* m_back;
    char* m_prev;
} dice_tui_ctx_t;

extern dice_tui_ctx_t dice_tui_ctx;

dice_tui_status_t dice_tui_get_size(void);
dice_tui_status_t dice_tui_init(void);
dice_tui_status_t dice_tui_set_char(int x, int y, char c);
dice_tui_status_t dice_tui_render(void);
dice_tui_status_t dice_tui_clear(void);

void dice_tui_shutdown(void);
#endif