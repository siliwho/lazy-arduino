#include "ui.h"
#include "top_bar.h"
#include "board.h"
#include "command.h"
#include "pages.h"
#include "sketches.h"
#include "state.h"
#include "logs.h"
#include "config.h"
#include "completion.h"
#include <ctype.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void draw_command_bar() {
    werase(cmd_win);
    if (app_state.mode == mode_command || app_state.mode == mode_completion) {
        mvwprintw(cmd_win, 0, 0, "%s", app_state.command_buffer);
        wmove(cmd_win, 0, strlen(app_state.command_buffer));
    }
    wnoutrefresh(cmd_win);
}

void check_dependencies() {
    if (system("command -v arduino-cli &> /dev/null") != 0) {
        endwin();
        fprintf(stderr, "Error: 'arduino-cli' is not installed or not in your PATH.\n");
        fprintf(stderr, "Please install it to use this application.\n");
        fprintf(stderr, "See: https://arduino.github.io/arduino-cli/latest/installation/\n");
        exit(1);
    }
}

void handle_command_mode(int ch);
void handle_completion_mode(int ch);

void handle_command_mode(int ch) {
    int len = strlen(app_state.command_buffer);
    switch (ch) {
        case '\n':
            process_command(app_state.command_buffer);
            app_state.mode = mode_normal;
            app_state.command_buffer[0] = '\0';
            curs_set(0);
            break;
        case 27: // esc
            app_state.mode = mode_normal;
            app_state.command_buffer[0] = '\0';
            curs_set(0);
            break;
        case '\t':
            trigger_completion();
            break; 
        case 127:
        case KEY_BACKSPACE:
        case 8:
            if (len > 1) {
                app_state.command_buffer[len - 1] = '\0';
            } else if (len == 1) {
                app_state.command_buffer[0] = '\0';
                app_state.mode = mode_normal;
                curs_set(0);
            }
            break;
        default:
            if (isprint(ch) && len < sizeof(app_state.command_buffer) - 1) {
                app_state.command_buffer[len] = ch;
                app_state.command_buffer[len + 1] = '\0';
            }
            break;
    }
}

void handle_completion_mode(int ch) {
    switch (ch) {
        case '\t':
            cycle_completion(1);
            break; 
        case KEY_BTAB:
            cycle_completion(-1);
            break;
        case '\n':
            apply_completion();
            clear_completion();
            break;
        case 27: // esc
            clear_completion();
            app_state.mode = mode_normal;
            app_state.command_buffer[0] = '\0';
            curs_set(0);
            break;
        default:
            clear_completion();
            handle_command_mode(ch);
            break;
    }
}

int main() {
    check_dependencies();
    load_config();
    init_ui();

    app_state.current_idx = 0; // Dashboard
    app_state.focus_idx = 0;   // Sketches panel
    app_state.mode = mode_normal;

    load_sketches(".");
    board_count = get_boards(connected_boards, MAX_BOARDS);
    add_log("Welcome to lazy-arduino!");

    init_current_page();

    while (true) {
        draw_top_bar(top_bar_win);
        draw_current_page();
        draw_command_bar();
        if (app_state.mode == mode_completion) {
            draw_completion_box();
        }
        doupdate();

        int ch = getch();
        if (ch == ERR) continue;

        if (app_state.mode == mode_completion) {
            handle_completion_mode(ch);
        } else if (app_state.mode == mode_command) {
            handle_command_mode(ch);
        } else if (app_state.mode == mode_normal) {
            switch (ch) {
                case KEY_RESIZE:
                    resize_current_page();
                    break;
                case ':':
                    app_state.mode = mode_command;
                    strcpy(app_state.command_buffer, ":");
                    curs_set(1);
                    break;
                case KEY_F(1): switch_page(0); break;
                case KEY_F(2): switch_page(1); break;
                case KEY_F(3): switch_page(2); break;
                case KEY_F(4): switch_page(3); break;
                case KEY_F(5): switch_page(4); break;
                case KEY_F(6): switch_page(5); break;
                case 'q':
                    end_ui();
                    return 0;
                default:
                    handle_current_page_input(ch);
                    break;
            }
        }
    }

    end_ui();
    return 0;
}
