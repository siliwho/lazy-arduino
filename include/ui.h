#ifndef UI_H
#define UI_H

#include <ncurses.h>

// Color Pair definitions
enum {
    CP_STANDARD = 1,
    CP_HIGHLIGHT_BORDER,
    CP_HIGHLIGHT_TAB,
    CP_STATUS_NORMAL,
    CP_STATUS_COMMAND,
    CP_COMPLETION_BOX,
    CP_COMPLETION_HEILIGHT
};

// Global window pointers
extern WINDOW *sketch_win, *board_win, *log_win, *serial_win, *status_win, *cmd_win, *comp_win;

void init_ui(void);
void end_ui(void);
void resize_windows(void);
void apply_theme(int base_color_256);

#endif // UI_H
