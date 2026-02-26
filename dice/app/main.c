#include <stdio.h>
#include "tui.h"

int main(void);
int main(void)
{
    if (tui_init() != TUI_OK){
		fprintf(stderr, "failed to initialize TUI\n");
		return 1;
	}
	
	const char *msg = "Hello, world!";

    int start_x = ((int)tui_ctx.width - (int)strlen(msg)) / 2;
    int start_y = ((int)tui_ctx.height) / 2;

    for (int i = 0; msg[i]; ++i) {
        tui_set_char(start_x + i, start_y, msg[i]);
    }

    tui_render();

    struct termios orig, raw;

	tcgetattr(STDIN_FILENO, &orig);

    raw = orig;
    raw.c_lflag &= (tcflag_t)~(ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	int ch;
	int xpos = 0, ypos = 1;

    while ((ch = getchar()) != 'q'){
		if (ch == '\n') {
			continue;
		}

        tui_set_char(xpos, ypos, (char)ch);
        tui_render();

        xpos++;

        if (xpos >= tui_ctx.width) {
		}
	}

    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
	printf("\nbye\n");
	
	return 0;
}
