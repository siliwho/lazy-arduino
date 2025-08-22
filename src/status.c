#include "status.h"
#include "state.h"
#include "board.h"
#include "pages.h"
#include "ui.h"
#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

bool is_loading = false;
static int loading_idx = 0;
static const char spinner[] = "|/-\"";

void start_loading(void) {
    is_loading = true;
    loading_idx = 0;
}

void stop_loading(void) {
    is_loading = false;
}

void load_anime(void) {
    if (!is_loading) return;
    loading_idx = (loading_idx + 1) % (strlen(spinner));
}

// src/status.c

static char status_message[256] = "Idle";

void set_status(const char *message) {
    strncpy(status_message, message, sizeof(status_message) - 1);
    status_message[sizeof(status_message) - 1] = '\0';
}

void draw_status(WINDOW *win) {
    if (!win) return;

    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    (void)max_y; // Unused variable

    int status_color_pair = CP_STATUS_NORMAL;
    const char *mode_str = "NORMAL >>";
    if (app_state.mode == mode_command) {
        status_color_pair = CP_STATUS_COMMAND;
        mode_str = "COMMAND >>";
    }

    // Set the background for the entire bar and erase
    wbkgd(win, COLOR_PAIR(status_color_pair));
    werase(win);

    // --- Draw Mode (Bold) ---
    wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD);
    mvwprintw(win, 0, 1, "%s", mode_str);

    // --- Draw Page Tabs (Bold) ---
    int current_pos = strlen(mode_str) + 2;
    for (int i = 0; i < NUM_PAGES; i++) {
        if (!page_registry[i].show_in_tabs) continue;

        if (i == app_state.current_idx) {
            // Active tab
            wattron(win, COLOR_PAIR(CP_HIGHLIGHT_TAB) | A_BOLD);
        } else {
            // Inactive tab
            wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD);
        }
        mvwprintw(win, 0, current_pos, " F%d:%s ", i + 1, page_registry[i].name);
        current_pos += strlen(page_registry[i].name) + 6;
    }

    // --- Draw Right-aligned Status (Bold) ---
    char right_status[256];
    if (is_loading) {
        int bar_width = 20;
        int progress = loading_idx % (bar_width + 1);
        snprintf(right_status, sizeof(right_status), "[%-*s>%-*s] %d%%", progress, "", bar_width - progress, "", (int)((float)progress / bar_width * 100));
    } else {
        snprintf(right_status, sizeof(right_status), "%s", status_message);
    }

    // Set attributes for the right-aligned text explicitly
    wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD);
    mvwprintw(win, 0, max_x - strlen(right_status) - 1, "%s", right_status);

    wnoutrefresh(win);
}

/* void draw_status(WINDOW *win) { */
/*     if (!win) return; */

/*     int max_y, max_x; */
/*     getmaxyx(win, max_y, max_x); */
/*     (void)max_y; */

/*     int status_color_pair = CP_STATUS_NORMAL; */
/*     const char *mode_str = "NORMAL >>"; */
/*     if (app_state.mode == mode_command) { */
/*         status_color_pair = CP_STATUS_COMMAND; */
/*         mode_str = "COMMAND >>"; */
/*     } */

/*     wattron(win, A_BOLD); */

/*     wbkgd(win, COLOR_PAIR(status_color_pair)); */
/*     werase(win); */

/*      wattron(win, COLOR_PAIR(status_color_pair)); */
/*     mvwprintw(win, 0, 1, "%s", mode_str); */
/*     wattroff(win, COLOR_PAIR(status_color_pair)); */

/*    /1* wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD); *1/ */
/*    /1*  mvwprintw(win, 0, 1, "%s", mode_str); *1/ */
/*    /1*  wattroff(win, COLOR_PAIR(status_color_pair) | A_BOLD); *1/ */

/*     int current_pos = strlen(mode_str) + 2; */
/*     int tab_f = 1; */
/*     for (int i = 0; i < NUM_PAGES; i++) { */
/*         if(!page_registry[i].show_in_tabs) continue; */
/*         if (i == app_state.current_idx) { */
/*             wattron(win, COLOR_PAIR(CP_HIGHLIGHT_TAB)); */
/*         } else { */
/*             wattron(win, COLOR_PAIR(status_color_pair)); */
/*         } */
/*         /1* if(page_registry[i].name != "Color Picker") mvwprintw(win, 0, current_pos, " F%d:%s ", i + 1, page_registry[i].name); *1/ */
/*         mvwprintw(win, 0, current_pos, " F%d:%s ", i + 1, page_registry[i].name); */
/*         current_pos += strlen(page_registry[i].name) + 6; */
/*     } */

/*     // Draw Right-aligned Status */
/*     char right_status[256]; */
/*     if (is_loading) { */
/*         snprintf(right_status, sizeof(right_status), "Working... %c", spinner[loading_idx]); */
/*     } else { */
/*         const char *board_status_str = (board_count > 0) ? "Connected" : "No Board"; */
/*         if (app_state.current_idx == 0) { // If on Dashboard page */
/*             const char *focus_str = (app_state.focus_idx < 4) ? panel_names[app_state.focus_idx] : "Unknown"; */
/*             snprintf(right_status, sizeof(right_status), "Board: %s | Port: %s | Focus: %s (TAB)", target_fqbn, target_port, focus_str); */
/*         } else { */
/*             snprintf(right_status, sizeof(right_status), "Board: %s", board_status_str); */
/*         } */
/*     } */
/*     /1* wattron(win, COLOR_PAIR(status_color_pair)); *1/ */
/*     mvwprintw(win, 0, max_x - strlen(right_status) - 1, "%s", right_status); */
/*     /1* wattroff(win, A_BOLD); *1/ */
/*     wnoutrefresh(win); */
/* } */

/* #include "status.h" */
/* #include "state.h" */
/* #include "board.h" */
/* #include "pages.h" */
/* #include "ui.h" */
/* #include "config.h" */
/* #include <stdbool.h> */
/* #include <stdio.h> */
/* #include <string.h> */
/* #include <ncurses.h> */

/* // This helper function is from a previous step, ensure it's here */
/* const char* get_short_board_name(const char* fqbn) { */
/*     const char* last_colon = strrchr(fqbn, ':'); */
/*     if (last_colon) { */
/*         return last_colon + 1; // Return the string part after the last ':' */
/*     } */
/*     return fqbn; // Or return the full string if format is unexpected */
/* } */


/* bool is_loading = false; */
/* static int loading_idx = 0; */
/* static const char *panel_names[] = {"Sketches", "Boards", "Logs", "Serial"}; */
/* static const char spinner[] = "|/-\\"; */

/* void start_loading(void) { */
/*     is_loading = true; */
/*     loading_idx = 0; */
/* } */

/* void stop_loading(void) { */
/*     is_loading = false; */
/* } */

/* void load_anime(void) { */
/*     if (!is_loading) return; */
/*     loading_idx = (loading_idx + 1) % (strlen(spinner)); */
/* } */

/* void draw_status(WINDOW *win) { */
/*     if (!win) return; */

/*     int max_y, max_x; */
/*     getmaxyx(win, max_y, max_x); */
/*     (void)max_y; */

/*     int status_color_pair = CP_STATUS_NORMAL; */
/*     const char *mode_str = "NORMAL >>"; */
/*     if (app_state.mode == mode_command) { */
/*         status_color_pair = CP_STATUS_COMMAND; */
/*         mode_str = "COMMAND >>"; */
/*     } */

/*     // Set the background color for the entire bar */
/*     wbkgd(win, COLOR_PAIR(status_color_pair)); */
/*     werase(win); */

/*     // --- Draw Mode --- */
/*     // Combine bold and color attributes every time for reliability */
/*     wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD); */
/*     mvwprintw(win, 0, 1, "%s", mode_str); */

/*     // --- Draw Page Tabs --- */
/*     int current_pos = strlen(mode_str) + 2; */
/*     int tab_f_key = 1; // FIX: Use a separate counter to avoid F-key gaps */
/*     for (int i = 0; i < NUM_PAGES; i++) { */
/*         if (!page_registry[i].show_in_tabs) continue; */

/*         if (i == app_state.current_idx) { */
/*             wattron(win, COLOR_PAIR(CP_HIGHLIGHT_TAB) | A_BOLD); */
/*         } else { */
/*             wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD); */
/*         } */
/*         mvwprintw(win, 0, current_pos, " F%d:%s ", tab_f_key, page_registry[i].name); */
/*         current_pos += strlen(page_registry[i].name) + 6; */
/*         tab_f_key++; // Only increment for visible tabs */
/*     } */

/*     // --- Draw Right-aligned Status --- */
/*     char right_status[128] = {0}; */
/*     if (is_loading) { */
/*         snprintf(right_status, sizeof(right_status), "Working... %c", spinner[loading_idx]); */
/*     } else { */
/*         // Using the elegant version from previous steps */
/*         snprintf(right_status, sizeof(right_status), "%s", get_short_board_name(selected_fqbn)); */
/*         if (board_count > 0) { */
/*             char port_str[80]; */
/*             snprintf(port_str, sizeof(port_str), " @ %s", connected_boards[0].port); */
/*             strncat(right_status, port_str, sizeof(right_status) - strlen(right_status) - 1); */
/*         } */
/*         if (app_state.current_idx == 0) { */
/*             const char *focus_str = (app_state.focus_idx < 4) ? panel_names[app_state.focus_idx] : "Unknown"; */
/*             char focus_part[80]; */
/*             snprintf(focus_part, sizeof(focus_part), " | Focus: %s", focus_str); */
/*             strncat(right_status, focus_part, sizeof(right_status) - strlen(right_status) - 1); */
/*         } */
/*     } */

/*     // Ensure the attributes are set for the right side as well */
/*     wattron(win, COLOR_PAIR(status_color_pair) | A_BOLD); */
/*     mvwprintw(win, 0, max_x - strlen(right_status) - 1, "%s", right_status); */

/*     wnoutrefresh(win); */
/* } */
