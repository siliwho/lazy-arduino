#ifndef STATE_H
#define STATE_H

#include "pages.h"

typedef enum {
    mode_normal,
    mode_command,
    mode_completion
} appmode;

typedef struct {
    int focus_idx;
    int current_idx;
    appmode mode;
    char command_buffer[256];
} appstate;

extern appstate app_state;

#endif // STATE_H
