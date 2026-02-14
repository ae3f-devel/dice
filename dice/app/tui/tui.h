#ifndef TUI_H
#define TUI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>

#include <ae2f/Sys/Trm.h>

/**
 * @brief Get current terminal size in rows and columns.
 * @details Writes row and column counts to the provided pointers. Returns 0 on
 * */
static void tui_get_size(ae2fsys_trmpos_t *rdwr_row_len, ae2fsys_trmpos_t *rdwr_col_len) {
	if (!rdwr_row_len || !rdwr_col_len) {
		return;
	}
	
	ae2fsys_get_trm_size_simple_imp(*rdwr_row_len, *rdwr_col_len);
}

typedef struct {
	ae2fsys_trmpos_t row_len
	ae2fsys_trmpos_t col_len;
	char *cells;
} tui_frame_t;


/**
 * @brief Convert a character to a printable ASCII fallback.
 * @details Returns the input when it is printable ASCII, otherwise returns '?'.
 */
static ae2f_inline char tui_sanitize_ascii(const char c_c) {
	return ((unsigned char)c_c < 32 || (unsigned char)c_c > 126)? '?' : c_c;
}

/**
 * @brief Allocate a new frame with the given size.
 * @details Returns a newly allocated frame or NULL on failure.
 */
static void tui_frame_new(tui_frame_t *rdwr_frame, const ae2fsys_trmpos_t c_row_len, const ae2fsys_trmpos_t c_col_len) {
	
	if (!c_row_len || !c_col_len || c_col_len > SIZE_MAX / c_row_len || !rdwr_frame) {
		return NULL;
	}
	
	rdwr_frame->cells = (char*)malloc(sizeof(char) * c_row_len * c_col_len);
	if (!rdwr_frame->cells) {
		return NULL;
	}

	rdwr_frame->row_len = c_row_len;
	rdwr_frame->col_len = c_col_len;
}

/**
 * @brief Free a frame and its backing storage.
 * @details Safe to call with NULL.
 */
static void tui_frame_free(tui_frame_t *rdwr_frame) {
	if (!rdwr_frame) {
		return;	
	}

	free(rdwr_frame->cells);
}
/**
 * @brief Set a character in the frame, sanitizing to ASCII.
 * @details Returns 0 on success and -1 on bounds or argument failure.
 */
static int tui_frame_set_char(tui_frame_t *rdwr_frame, const size_t c_row, const size_t c_col, const char c_c) {
    if (!rdwr_frame || c_row >= rdwr_frame->row_len || c_col >= rdwr_frame->col_len) {
	return -1;
    }
    rdwr_frame->cells[c_row * rdwr_frame->col_len + c_col] = tui_sanitize_ascii(ch);

    return 0;
}


/**
 * @brief Get a character from the frame.
 * @details Returns the character at the location or '\0' on failure.
 */
static inline char tui_frame_get_char(tui_frame_t *f, size_t r, size_t c) {
    if (!f || r >= f->rows || c >= f->cols) return '\0';

    return f->cells[r * f->cols + c];
}


/**
 * @brief Fill the entire frame with a character.
 * @details No-op when frame is NULL.
 */
static inline void tui_frame_clear(tui_frame_t *f, char ch) {
    if (!f) return;

    memset(f->cells, (unsigned char)ch, f->rows * f->cols);
}


/**
 * @brief Resize a frame, preserving overlapping contents.
 * @details Returns 0 on success and -1 on failure.
 */
static inline int tui_frame_resize(tui_frame_t *f, size_t rows, size_t cols) {
    if (!f || rows == 0 || cols == 0 || cols > SIZE_MAX / rows) return -1;

    size_t total = rows * cols;
    char *newcells = calloc(total, sizeof(char));

    if (!newcells) return -1;

    size_t minr = rows < f->rows ? rows : f->rows;
    size_t minc = cols < f->cols ? cols : f->cols;

    for (size_t r = 0; r < minr; ++r) {
        memcpy(newcells + r * cols, f->cells + r * f->cols, minc);
    }
    free(f->cells);

    f->cells = newcells;
    f->rows = rows;
    f->cols = cols;

    return 0;
}


/**
 * @brief Draw the frame to an output stream with newlines.
 * @details Writes each row followed by a newline and flushes the stream.
 */
static inline void tui_frame_draw(FILE *out, const tui_frame_t *f) {
    if (!f || !out) return;

    for (size_t r = 0; r < f->rows; ++r) {
        fwrite(f->cells + r * f->cols, 1, f->cols, out);
        fputc('\n', out);
    }

    fflush(out);
}


/**
 * @brief Move cursor to a 0-based row/column position.
 * @details Emits an ANSI cursor move sequence to the output stream.
 */
static inline void tui_ansi_move_cursor(FILE *out, size_t r, size_t c) {
    if (!out) return;

    fprintf(out, "\x1b[%zu;%zuH", r + 1, c + 1);
}

/**
 * @brief Clear the screen and move cursor to home.
 * @details Emits ANSI clear and home sequences and flushes the stream.
 */
static inline void tui_ansi_clear_screen(FILE *out) {
    if (!out) return;

    fprintf(out, "\x1b[2J\x1b[H");
    fflush(out);
}

/**
 * @brief Hide the terminal cursor.
 * @details Emits the ANSI hide-cursor sequence and flushes the stream.
 */
static inline void tui_ansi_hide_cursor(FILE *out) {
    if (!out) return;

    fprintf(out, "\x1b[?25l");
    fflush(out);
}

/**
 * @brief Show the terminal cursor.
 * @details Emits the ANSI show-cursor sequence and flushes the stream.
 */
static inline void tui_ansi_show_cursor(FILE *out) {
    if (!out) return;

    fprintf(out, "\x1b[?25h");
    fflush(out);
}


#ifndef _WIN32
static inline void tui_disable_raw_mode(void);

static struct termios tui_orig_termios;
static int tui_termios_saved = 0;


/**
 * @brief Enable raw input mode (POSIX).
 * @details Saves the original termios settings and registers an atexit
 * handler. Returns 0 on success and -1 on failure.
 */
static inline int tui_enable_raw_mode(void) {
    struct termios raw;

    if (tui_termios_saved) return 0;
    if (!isatty(STDIN_FILENO)) return -1;
    if (tcgetattr(STDIN_FILENO, &tui_orig_termios) == -1) return -1;

    raw = tui_orig_termios;
    raw.c_lflag &= (tcflag_t)~(tcflag_t)(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_iflag &= (tcflag_t)~(tcflag_t)(IXON | ICRNL);
    raw.c_oflag &= (tcflag_t)~(tcflag_t)(OPOST);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return -1;

    tui_termios_saved = 1;

    atexit(tui_disable_raw_mode);

    return 0;
}

/**
 * @brief Restore original terminal mode (POSIX).
 * @details No-op when no state is saved.
 */
static inline void tui_disable_raw_mode(void) {
    if (!tui_termios_saved) return;
    if (!isatty(STDIN_FILENO)) return;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tui_orig_termios);

    tui_termios_saved = 0;
}
#endif

#ifdef _WIN32
static inline void tui_disable_raw_mode(void);

static DWORD tui_orig_mode;
static int tui_mode_saved = 0;


/**
 * @brief Enable raw input mode (Windows).
 * @details Saves the original console mode and registers an atexit handler.
 * Returns 0 on success and -1 on failure.
 */
static inline int tui_enable_raw_mode(void) {
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE) return -1;

    DWORD mode = 0;

    if (!GetConsoleMode(h, &mode)) return -1;
    if (!tui_mode_saved) {
        tui_orig_mode = mode;
        tui_mode_saved = 1;
        atexit(tui_disable_raw_mode);
    }

    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);

    if (!SetConsoleMode(h, mode)) return -1;
    return 0;
}

/**
 * @brief Restore original console mode (Windows).
 * @details No-op when no state is saved.
 */
static inline void tui_disable_raw_mode(void) {
    if (!tui_mode_saved) return;

    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE) return;

    SetConsoleMode(h, tui_orig_mode);

    tui_mode_saved = 0;
}
#endif


/**
 * @brief Poll for a single key press.
 * @details Waits up to timeout_ms milliseconds. Returns the key code as an
 * unsigned char, or -1 on timeout or failure.
 */
static inline int tui_poll_key(int timeout_ms) {
#ifndef _WIN32
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int rv = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);

    if (rv <= 0) return -1;
    char ch = 0;
    ssize_t n = read(STDIN_FILENO, &ch, 1);

    if (n <= 0) return -1;
    return (unsigned char)ch;
#else
    if (timeout_ms < 0) timeout_ms = INFINITE;

    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE) return -1;

    DWORD wait = WaitForSingleObject(h, (DWORD)timeout_ms);

    if (wait != WAIT_OBJECT_0) return -1;
    if (!_kbhit()) return -1;

    return (unsigned char)_getch();
#endif
}


static volatile sig_atomic_t tui_resize_flag = 0;
/**
 * @brief Signal handler to mark a pending resize.
 * @details Sets an internal flag used by the resize handler.
 */
static inline void tui_sigwinch_handler(int unused) { (void)unused; tui_resize_flag = 1; }

/**
 * @brief Install resize signal handler when supported.
 * @details No-op on platforms without SIGWINCH.
 */
static inline void tui_install_resize_handler(void) {
#ifndef _WIN32
#ifdef SIGWINCH
    signal(SIGWINCH, tui_sigwinch_handler);
#endif
#endif
}

typedef struct {
    FILE *out;
    tui_frame_t *front;
    tui_frame_t *back;
} tui_ctx_t;

typedef enum {
    TUI_CTX_OK = 0,
    TUI_CTX_ERR_ARG,
    TUI_CTX_ERR_TERM_SIZE,
    TUI_CTX_ERR_ALLOC,
    TUI_CTX_ERR_RAW_MODE
} tui_ctx_status_t;


/**
 * @brief Create a new TUI context with front/back buffers.
 * @details Uses the current terminal size or a 24x80 fallback. Returns a new
 * context or NULL on failure.
 */
static inline tui_ctx_status_t tui_ctx_new_ex(tui_ctx_t **out_ctx, FILE *out) {
    unsigned short rows = 0, cols = 0;

    if (!out_ctx) return TUI_CTX_ERR_ARG;

    *out_ctx = NULL;

    if (tui_get_size(&rows, &cols) == -1) {
        rows = 24;
        cols = 80;
    }
    if (rows == 0 || cols == 0) { rows = 24; cols = 80; }

    tui_ctx_t *ctx = malloc(sizeof *ctx);

    if (!ctx) return TUI_CTX_ERR_ALLOC;

    ctx->out = out ? out : stdout;
    ctx->front = tui_frame_new(rows, cols);
    ctx->back = tui_frame_new(rows, cols);

    if (!ctx->front || !ctx->back) {
        tui_frame_free(ctx->front);
        tui_frame_free(ctx->back);
        free(ctx);

        return TUI_CTX_ERR_ALLOC;
    }

    tui_frame_clear(ctx->front, ' ');
    tui_frame_clear(ctx->back, ' ');
    if (tui_enable_raw_mode() == -1) {
        tui_frame_free(ctx->front);
        tui_frame_free(ctx->back);
        free(ctx);
        return TUI_CTX_ERR_RAW_MODE;
    }
    tui_install_resize_handler();

    *out_ctx = ctx;
    return TUI_CTX_OK;
}

/**
 * @brief Create a new TUI context with front/back buffers.
 * @details Uses the current terminal size or a 24x80 fallback. Returns a new
 * context or NULL on failure.
 */
static inline tui_ctx_t *tui_ctx_new(FILE *out) {
    tui_ctx_t *ctx = NULL;

    if (tui_ctx_new_ex(&ctx, out) != TUI_CTX_OK) return NULL;
    return ctx;
}




/**
 * @brief Free a TUI context and its frames.
 * @details Safe to call with NULL.
 */
static inline void tui_ctx_free(tui_ctx_t *ctx) {
    if (!ctx) return;

    tui_frame_free(ctx->front);
    tui_frame_free(ctx->back);
    free(ctx);
}


/**
 * @brief Handle a pending resize by updating frame sizes.
 * @details Returns 1 if resized, 0 if no resize, and -1 on failure.
 */
static inline int tui_ctx_handle_resize(tui_ctx_t *ctx) {
    if (!ctx) return -1;
    if (!tui_resize_flag) return 0;

    tui_resize_flag = 0;
    unsigned short rows = 0, cols = 0;

    if (tui_get_size(&rows, &cols) == -1) return -1;
    if (rows == 0 || cols == 0) return -1;
    if (tui_frame_resize(ctx->front, rows, cols) == -1) return -1;
    if (tui_frame_resize(ctx->back, rows, cols) == -1) return -1;

    tui_frame_clear(ctx->back, ' ');
    memset(ctx->front->cells, 0, ctx->front->rows * ctx->front->cols);

    return 1;
}


/**
 * @brief Present back buffer differences to the output.
 * @details Diffs back against front and writes only changed spans.
 */
static inline void tui_present(tui_ctx_t *ctx) {
    if (!ctx || !ctx->out) return;

    tui_frame_t *f = ctx->back;
    tui_frame_t *g = ctx->front;

    if (!f || !g) return;

    size_t rows = f->rows, cols = f->cols;

    for (size_t r = 0; r < rows; ++r) {
        size_t c = 0;
        
        while (c < cols) {
            char b = g->cells[r * cols + c];

            if (f->cells[r * cols + c] == b) {
                ++c;
                continue;
            }
            size_t start = c;

            while (c < cols && f->cells[r * cols + c] != g->cells[r * cols + c]) ++c;

            size_t len = c - start;

            tui_ansi_move_cursor(ctx->out, r, start);

            for (size_t i = 0; i < len; ++i) {
                char ch = tui_sanitize_ascii(f->cells[r * cols + start + i]);
                fputc(ch, ctx->out);
                g->cells[r * cols + start + i] = ch;
            }
        }
    }
    
    fflush(ctx->out);
}

#ifdef __cplusplus
}
#endif

#endif