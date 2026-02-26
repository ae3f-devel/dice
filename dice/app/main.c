#include <stdio.h>
#include "dice/tui.h"

int main(void);
int main(void)
{
    if (dice_tui_init() != DICE_TUI_OK){
		fprintf(stderr, "failed to initialize TUI\n");
		return 1;
	}

	const char *msg = "Hello, world!";

    int start_x = ((int)dice_tui_ctx.m_width - (int)strlen(msg)) / 2;
    int start_y = ((int)dice_tui_ctx.m_height) / 2;

    for (int i = 0; msg[i]; ++i) {
        dice_tui_set_char(start_x + i, start_y, msg[i]);
    }

    dice_tui_render();

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

        dice_tui_set_char(xpos, ypos, (char)ch);
        dice_tui_render();

        xpos++;

        if (xpos >= dice_tui_ctx.m_width) {
            return -1;
		}
	}

    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
	printf("\nbye\n");
	
	return 0;슨
}