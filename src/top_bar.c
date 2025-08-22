
#include "top_bar.h"
#include "board.h"
#include "config.h"
#include "ui.h"
#include <ncurses.h>

void draw_top_bar(WINDOW *win) {
    if (!win) return;

    // Set the background for the entire bar and erase
    wbkgd(win, COLOR_PAIR(CP_STATUS_NORMAL));
    werase(win);

    // --- Draw Board, Port, and Baud Rate (Bold) ---
    wattron(win, COLOR_PAIR(CP_STATUS_NORMAL) | A_BOLD);
    mvwprintw(win, 0, 1, "Board: %s | Port: %s | Baud: %s", target_fqbn, target_port, target_baud);

    wnoutrefresh(win);
}
