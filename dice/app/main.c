#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "tui/tui.h"

int main(int argc, char **argv);

enum { CHAT_MAX_LINES = 128, CHAT_MAX_TEXT = 256, CHAT_MAX_USER = 32 };

static void chat_add_message(char lines[CHAT_MAX_LINES][CHAT_MAX_TEXT], size_t *line_count, const char *nickname, const char *msg);

/* parse server argument 'host[:port]' or '[ipv6]:port' -> resolve host to numeric IP
 * returns detailed parse_server_status_t for better error handling */
typedef enum {
    PARSE_SERVER_OK = 0,
    PARSE_SERVER_ERR_ARG,
    PARSE_SERVER_ERR_SYNTAX,
    PARSE_SERVER_ERR_NAME_RESOLUTION,
    PARSE_SERVER_ERR_NO_ADDRESS,
    PARSE_SERVER_ERR_PORT,
    PARSE_SERVER_ERR_TOOLONG
} parse_server_status_t;

static const char *parse_server_status_msg(parse_server_status_t s) {
    switch (s) {
        case PARSE_SERVER_OK: return "ok";
        case PARSE_SERVER_ERR_ARG: return "invalid argument";
        case PARSE_SERVER_ERR_SYNTAX: return "syntax error";
        case PARSE_SERVER_ERR_NAME_RESOLUTION: return "name resolution failed";
        case PARSE_SERVER_ERR_NO_ADDRESS: return "no numeric address found";
        case PARSE_SERVER_ERR_PORT: return "invalid port";
        case PARSE_SERVER_ERR_TOOLONG: return "host too long";
        default: return "unknown error";
    }
}

static parse_server_status_t parse_server_arg(
	const char *arg,
	char *out_host,
	size_t out_host_len,
	char *out_ip, size_t out_ip_len,
	unsigned short *out_port
	) {
    if (!arg || !out_host || !out_ip) return PARSE_SERVER_ERR_ARG;
    if (out_host_len < 2 || out_ip_len < 16) return PARSE_SERVER_ERR_TOOLONG;

    char hostbuf[256] = {0};
    char portbuf[32] = {0};

    const char *hoststart = arg;
    const char *hostend = NULL;
    const char *portstart = NULL;

    /* parse host / optional port */
    if (arg[0] == '[') {
        /* bracketed IPv6: [addr]:port */
        hoststart = arg + 1;
        hostend = strchr(hoststart, ']');

        if (!hostend) return PARSE_SERVER_ERR_SYNTAX;
        if (hostend[1] == ':') portstart = hostend + 2;
    } else {
        /* look for a single trailing ':' (host:port). multiple ':' likely indicates an IPv6 literal without brackets. */
        const char *lastc = strrchr(arg, ':');

        if (lastc && strchr(arg, ':') == lastc) {
            hostend = lastc;
            portstart = lastc + 1;
        } else {
            hostend = NULL; /* no explicit port */
        }
    }

    if (!hostend) hostend = arg + strlen(arg);

    size_t hlen = (size_t)(hostend - hoststart);

    if (hlen == 0) return PARSE_SERVER_ERR_SYNTAX;
    if (hlen >= sizeof(hostbuf) || hlen >= out_host_len) return PARSE_SERVER_ERR_TOOLONG;

    memcpy(hostbuf, hoststart, hlen);
    hostbuf[hlen] = '\0';

    if (portstart && *portstart) {
        strncpy(portbuf, portstart, sizeof(portbuf) - 1);
        portbuf[sizeof(portbuf) - 1] = '\0';
    }

    /* resolve host to numeric IP (prefer IPv4 when available) */
    struct addrinfo hints = {0}, *res = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int gai = getaddrinfo(hostbuf, NULL, &hints, &res);

    if (gai != 0) return PARSE_SERVER_ERR_NAME_RESOLUTION;

    char ipstr[INET6_ADDRSTRLEN] = {0};

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family != AF_INET && p->ai_family != AF_INET6) continue;

        if (getnameinfo(p->ai_addr, p->ai_addrlen, ipstr, sizeof(ipstr), NULL, 0, NI_NUMERICHOST) != 0) continue;

        if (p->ai_family == AF_INET) break; /* prefer IPv4 */

        if (ipstr[0] != '\0') break; /* accept IPv6 */
    }

    freeaddrinfo(res);

    if (ipstr[0] == '\0') return PARSE_SERVER_ERR_NO_ADDRESS;

    /* copy outputs safely */
    strncpy(out_host, hostbuf, out_host_len - 1);

    out_host[out_host_len - 1] = '\0';

    strncpy(out_ip, ipstr, out_ip_len - 1);

    out_ip[out_ip_len - 1] = '\0';

    if (out_port) {
        if (portbuf[0]) {
            char *endptr = NULL;
            long val = strtol(portbuf, &endptr, 10);

            if (!endptr || *endptr != '\0' || val < 1 || val > 65535) return PARSE_SERVER_ERR_PORT;

            *out_port = (unsigned short)val;
        } else {
            *out_port = 0;
        }
    }

    return PARSE_SERVER_OK;
}

static void tui_write_text(tui_frame_t *f, size_t row, size_t col, const char *text) {
	if (!f || !text) return;

	if (row >= f->rows || col >= f->cols) return;

	size_t c = col;

	for (size_t i = 0; text[i] != '\0' && c < f->cols; ++i, ++c) {
		tui_frame_set_char(f, row, c, text[i]);
	}
}

static void tui_draw_hline(tui_frame_t *f, size_t row, char ch) {
	if (!f) return;

	for (size_t c = 0; c < f->cols; ++c) {
		tui_frame_set_char(f, row, c, ch);
	}
}

int main(int argc, char **argv) {
	/* parse optional server address from argv[1] */
	char server_host[256] = "";
	char server_ip[INET6_ADDRSTRLEN] = "";

	unsigned short server_port = 0;
	int server_specified = 0;

	if (argc > 1) {
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			printf("Usage: %s [host[:port]]\n", argv[0]);

			return 0;
		}

		parse_server_status_t pst = parse_server_arg(argv[1], server_host, sizeof(server_host), server_ip, sizeof(server_ip), &server_port);
		if (pst != PARSE_SERVER_OK) {
			fprintf(stderr, "Invalid server address '%s': %s\n", argv[1], parse_server_status_msg(pst));

			return 1;
		}
		server_specified = 1;
	}

	/* ask nickname before enabling raw mode / creating TUI context */
	char current_user[CHAT_MAX_USER] = "User";

	printf("Enter your nickname (max %d chars): ", CHAT_MAX_USER - 1);

	fflush(stdout);

	if (fgets(current_user, CHAT_MAX_USER, stdin)) {
		size_t len = strlen(current_user);

		if (len > 0 && current_user[len - 1] == '\n') current_user[len - 1] = '\0';
		if (current_user[0] == '\0') strcpy(current_user, "User");
	} else {
		strcpy(current_user, "User");
	}

	/* now create TUI context (this will enable raw mode) */
	tui_ctx_t *ctx = NULL;
	tui_ctx_status_t st = tui_ctx_new_ex(&ctx, stdout);

	if (st != TUI_CTX_OK || !ctx || !ctx->back || !ctx->front || !ctx->out) return 1;

	char lines[CHAT_MAX_LINES][CHAT_MAX_TEXT];

	size_t line_count = 0;
	size_t scroll = 0;

	char input[CHAT_MAX_TEXT];

	size_t input_len = 0;

	input[0] = '\0';

	/* clear screen and hide cursor after raw mode enabled */
	tui_ansi_clear_screen(ctx->out);
	tui_ansi_hide_cursor(ctx->out);

	int running = 1;
	while (running) {
		int resized = tui_ctx_handle_resize(ctx);

		if (resized == -1) break;

		tui_frame_t *f = ctx->back;

		if (!f) break;

		tui_frame_clear(f, ' ');

		if (f->rows < 4 || f->cols < 10) {
			tui_write_text(f, 0, 0, "terminal too small");
			tui_present(ctx);

			continue;
		}

		char title[512];
		if (server_specified) {
			if (server_port)
				snprintf(title, sizeof(title), "chat - %s (%s):%u", server_host, server_ip, server_port);
			else
				snprintf(title, sizeof(title), "chat - %s (%s)", server_host, server_ip);
		} else {
			snprintf(title, sizeof(title), "chat - local tui");
		}
		tui_write_text(f, 0, 2, title);
		tui_draw_hline(f, 1, '-');

		size_t msg_top = 2;
		size_t msg_bottom = f->rows - 3;
		size_t msg_rows = (msg_bottom >= msg_top) ? (msg_bottom - msg_top + 1) : 0;

		if (msg_rows == 0) {
			tui_present(ctx);
			continue;
		}

		if (line_count > msg_rows) {
			size_t max_scroll = line_count - msg_rows;

			if (scroll > max_scroll) scroll = max_scroll;
		} else {
			scroll = 0;
		}

		for (size_t i = 0; i < msg_rows; ++i) {
			size_t idx = scroll + i;

			if (idx >= line_count) break;

			tui_write_text(f, msg_top + i, 0, lines[idx]);
		}

		tui_draw_hline(f, f->rows - 2, '-');
		tui_write_text(f, f->rows - 1, 0, "> ");
		tui_write_text(f, f->rows - 1, 2, input);
		tui_present(ctx);

		int ch = tui_poll_key(50);
		if (ch == -1) continue;

		/* Ctrl-C (ETX) clears the current input; Ctrl-D (EOT) exits */
		if (ch == 3) {
			/* clear typed input */
			input_len = 0;
			input[0] = '\0';
			/* continue to redraw with cleared input */
			continue;
		}
		if (ch == 4) {
			running = 0;
			continue;
		}

		if (ch == 27) {
			running = 0;
			continue;
		}

		if (ch == '\r' || ch == '\n') {
			if (input_len > 0 && line_count < CHAT_MAX_LINES) {
				chat_add_message(lines, &line_count, current_user, input);
				input_len = 0;
				input[0] = '\0';
			}
			continue;
		}

		if (ch == 8 || ch == 127) {
			if (input_len > 0) {
				input_len--;
				input[input_len] = '\0';
			}
			continue;
		}

		if (ch >= 32 && ch <= 126) {
			if (input_len + 1 < CHAT_MAX_TEXT) {
				input[input_len++] = (char)ch;
				input[input_len] = '\0';
			}
		}
	}

	tui_ansi_show_cursor(ctx->out);
	tui_ansi_clear_screen(ctx->out);
	tui_ctx_free(ctx);

	return 0;
}

static void chat_add_message(char lines[CHAT_MAX_LINES][CHAT_MAX_TEXT], size_t *line_count, const char *nickname, const char *msg) {
    if (!lines || !line_count || !nickname || !msg) return;
    if (*line_count >= CHAT_MAX_LINES) return;

    snprintf(lines[*line_count], CHAT_MAX_TEXT, "%s: %s", nickname, msg);

    (*line_count)++;
}
