#ifndef PAGES_H
#define PAGES_H

#include "panel.h"
#include <stdbool.h>

typedef void (*page_init)(void);
typedef void (*page_drw)(void);
typedef void (*page_ipt)(int key);
typedef void (*page_rsize)(void);
typedef void (*page_dest)(void);

typedef struct {
    const char *name;
    page_init init;
    page_drw drw;
    page_ipt ipt;
    page_rsize resize;
    page_dest destroy;
    bool show_in_tabs;
} Page;

extern const Page page_registry[];
extern const int NUM_PAGES;

void init_current_page(void);
void draw_current_page(void);
void draw_status_bar(void);
void handle_current_page_input(int key);
void resize_current_page(void);
void destroy_current_page(void);
void switch_page(int page_idx);

#endif // PAGES_H
