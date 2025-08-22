#include "top_bar.h"
#include "ui.h"
#include "board.h"
#include "logs.h"
#include "serial.h"
#include "sketches.h"
#include "colors.h"
#include "status.h"
#include <ncurses.h>

WINDOW *top_bar_win, *sketch_win, *board_win, *log_win, *serial_win, *status_win, *cmd_win;

void apply_theme(int base_color_256) {
    init_pair(CP_STATUS_NORMAL, COLOR_BLACK, base_color_256);
    init_pair(CP_STATUS_COMMAND, COLOR_BLACK, COLOR_YELLOW); // Command mode can keep a distinct color
    init_pair(CP_HIGHLIGHT_BORDER, base_color_256, -1);
    init_pair(CP_HIGHLIGHT_TAB, base_color_256, COLOR_BLACK);
}

void resize_windows() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int main_area_height = max_y - 3; // Make space for the top bar
    int top_bar_y = 0;
    int status_bar_y = max_y - 2;
    int cmd_bar_y = max_y - 1;

    int mid_x = max_x / 2;
    int top_half_h = main_area_height / 2;
    int bottom_half_h = main_area_height - top_half_h;
    
    // Split the bottom half for logs and serial monitor
    int log_h = bottom_half_h / 2;
    int serial_h = bottom_half_h - log_h;
    int bottom_start_y = top_half_h + 1; // Adjust for top bar
    int serial_start_y = bottom_start_y + log_h;

    // Delete old windows before creating new ones
    if (top_bar_win) delwin(top_bar_win);
    if (sketch_win) delwin(sketch_win);
    if (board_win) delwin(board_win);
    if (log_win) delwin(log_win);
    if (serial_win) delwin(serial_win);
    if (status_win) delwin(status_win);
    if (cmd_win) delwin(cmd_win);

    top_bar_win = newwin(1, max_x, top_bar_y, 0);
    sketch_win = newwin(top_half_h, mid_x, 1, 0); // Adjust for top bar
    board_win = newwin(top_half_h, max_x - mid_x, 1, mid_x); // Adjust for top bar
    log_win = newwin(log_h, max_x, bottom_start_y, 0);
    serial_win = newwin(serial_h, max_x, serial_start_y, 0);
    status_win = newwin(1, max_x, status_bar_y, 0);
    cmd_win = newwin(1, max_x, cmd_bar_y, 0);

    clear();
    refresh();
}


void init_ui() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_STANDARD, COLOR_WHITE, -1);
        init_pair(CP_HIGHLIGHT_BORDER, COLOR_CYAN, -1);
        init_pair(CP_HIGHLIGHT_TAB, COLOR_CYAN, COLOR_BLACK);
        init_pair(CP_STATUS_NORMAL, COLOR_BLACK, COLOR_CYAN);
        init_pair(CP_STATUS_COMMAND, COLOR_BLACK, COLOR_YELLOW);
    }
    
    resize_windows();
    draw_top_bar(top_bar_win);
    refresh();
}


void end_ui() {
    if (top_bar_win) delwin(top_bar_win);
    if (sketch_win) delwin(sketch_win);
    if (board_win) delwin(board_win);
    if (log_win) delwin(log_win);
    if (serial_win) delwin(serial_win);
    if (status_win) delwin(status_win);
    if (cmd_win) delwin(cmd_win);
    endwin();
}
