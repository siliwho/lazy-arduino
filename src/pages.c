#include "pages.h"
#include "state.h"
#include "ui.h"
#include "status.h"
#include "sketches.h"
#include "board.h"
#include "logs.h"
#include "serial.h"
#include "color_picker.h"
#include <string.h>
#include <ncurses.h>

Panel dashboard_panels[4];
const int NUM_DASHBOARD_PANELS = 4;

void dashboard_init() {
    resize_windows(); // Create windows
    touchwin(sketch_win);
    touchwin(board_win);
    touchwin(log_win);
    touchwin(serial_win);

    dashboard_panels[0] = (Panel){"Sketches", sketch_win, draw_sketches_panel, handle_sketches_input, NULL};
    dashboard_panels[1] = (Panel){"Boards", board_win, draw_boards_panel, NULL, NULL};
    dashboard_panels[2] = (Panel){"Logs", log_win, draw_logs_panel, NULL, NULL};
    dashboard_panels[3] = (Panel){"Serial", serial_win, draw_serial_panel, NULL, NULL};
}

void dashboard_draw() {
    for (int i = 0; i < NUM_DASHBOARD_PANELS; i++) {
        Panel *p = &dashboard_panels[i];
        if (p->draw_func) {
            p->draw_func(p, app_state.focus_idx == i);
        }
    }
}

void dashboard_handle_input(int key) {
    if (key == '\t') {
        app_state.focus_idx = (app_state.focus_idx + 1) % NUM_DASHBOARD_PANELS;
        return;
    }

    Panel *focused_panel = &dashboard_panels[app_state.focus_idx];
    if (focused_panel->inp_func) {
        focused_panel->inp_func(focused_panel, key);
    }
}

void dashboard_resize() {
    resize_windows();
    dashboard_panels[0].win = sketch_win;
    dashboard_panels[1].win = board_win;
    dashboard_panels[2].win = log_win;
    dashboard_panels[3].win = serial_win;
}

void dashboard_destroy() {
    delwin(sketch_win);
    delwin(board_win);
    delwin(log_win);
    delwin(serial_win);
    sketch_win = board_win = log_win = serial_win = NULL;
}

void unimplemented_draw() {
    erase(); // This clears the standard screen buffer
    mvprintw(1, 2, "[%s] Page Under Construction", page_registry[app_state.current_idx].name);
    refresh(); // REMOVED: This was breaking the main draw cycle.
}

void unimplemented_input(int key) {
    (void)key; // No action
}

void color_picker_init(){ }
void color_picker_draw() {werase(stdscr); draw_color_picker_page(); wnoutrefresh(stdscr);}
void color_picker_input(int key) { handle_color_picker_input(key); }

const Page page_registry[] = {
    {"Dash",     dashboard_init,      dashboard_draw,      dashboard_handle_input, dashboard_resize, dashboard_destroy, true},
    {"Boards",   NULL,                unimplemented_draw,  unimplemented_input,    NULL,             NULL,              true},
    {"Libs",     NULL,                unimplemented_draw,  unimplemented_input,    NULL,             NULL,              true},
    {"Cores",    NULL,                unimplemented_draw,  unimplemented_input,    NULL,             NULL,              true},
    {"Examples", NULL,                unimplemented_draw,  unimplemented_input,    NULL,             NULL,              true},
    {"Colors",   color_picker_init,   color_picker_draw,   color_picker_input,     NULL,             NULL,              false}
};

/* const Page page_registry[] = { */
/*     {"Dashboard", dashboard_init, dashboard_draw, dashboard_handle_input, dashboard_resize, dashboard_destroy}, */
/*     {"Boards", NULL, unimplemented_draw, unimplemented_input, NULL, NULL}, */
/*     {"Libraries", NULL, unimplemented_draw, unimplemented_input, NULL, NULL}, */
/*     {"Cores", NULL, unimplemented_draw, unimplemented_input, NULL, NULL}, */
/*     {"Examples", NULL, unimplemented_draw, unimplemented_input, NULL, NULL}, */
/*     {"Color Picker", color_picker_init, color_picker_draw, color_picker_input, NULL, NULL} */
/* }; */
const int NUM_PAGES = sizeof(page_registry) / sizeof(Page);

void init_current_page() {
    const Page *page = &page_registry[app_state.current_idx];
    if (page->init) {
        page->init();
    }
}

void draw_current_page() {
    const Page *page = &page_registry[app_state.current_idx];
    if (page->drw) {
        page->drw();
    }
    draw_status(status_win);
}

void draw_status_bar(){
    draw_status(status_win);
}
void handle_current_page_input(int key) {
    const Page *page = &page_registry[app_state.current_idx];
    if (page->ipt) {
        page->ipt(key);
    }
}

void resize_current_page() {
    const Page *page = &page_registry[app_state.current_idx];
    if (page->resize) {
        page->resize();
    }
    
    // Also resize status and command bars
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    if(status_win) { mvwin(status_win, max_y - 2, 0); wresize(status_win, 1, max_x); }
    if(cmd_win) { mvwin(cmd_win, max_y - 1, 0); wresize(cmd_win, 1, max_x); }

    clear();
    refresh();
}

void destroy_current_page() {
    const Page *page = &page_registry[app_state.current_idx];
    if (page->destroy) {
        page->destroy();
    }
}

void switch_page(int new_page_index) {
    if (new_page_index < 0 || new_page_index >= NUM_PAGES || new_page_index == app_state.current_idx) {
        return;
    }
    destroy_current_page();
    app_state.current_idx = new_page_index;
    app_state.focus_idx = 0; // Reset focus on page switch
    init_current_page();
    /* clear(); // Clear screen for new page */
    /* draw_current_page(); */
}
