#ifndef dice_tui_err_h
#define dice_tui_err_h

typedef enum DICE_TUI_ {
    DICE_TUI_OK = 0,
    DICE_TUI_INIT_RECALL,
    DICE_TUI_ERR_MEMORY_ALLOC,
    DICE_TUI_ERR_NULL_POINTER,
    DICE_TUI_ERR_OUT_OF_BOUNDS,
    DICE_TUI_ERR_INVALID_INPUT,
    DICE_TUI_ERR_WINDOW_SIZE,
} dice_tui_status_t;
#endif