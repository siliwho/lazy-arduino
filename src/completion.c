#include "completion.h"
#include "command.h"
#include "state.h"
#include "ui.h"
#include <string.h>
#include <ncurses.h>
#include <stdio.h>

static CompletionResult current_completions;
static int selected_completion = -1;

extern const Command command_registry[];
extern const int num_commands;

void trigger_completion(void) {
    const char *input = app_state.command_buffer;
    current_completions.count = 0;
    selected_completion = -1;

    char *space = strchr(input, ' ');
    if (space == NULL) {
        complete_command(input, &current_completions);
    } else {
        char command_name[128];
        int len = space - input;
        strncpy(command_name, input, len);
        command_name[len] = '\0';

        const char *cmd_to_find = command_name;
        if (cmd_to_find[0] == ':') {
            cmd_to_find++;
        }

        const Command *cmd = find_command(cmd_to_find);
        if (cmd && cmd->complete) {
            cmd->complete(space + 1, &current_completions);
        }
    }

    if (current_completions.count > 0) {
        app_state.mode = mode_completion;
        selected_completion = 0;
    } else {
        app_state.mode = mode_command;
    }
}

void clear_completion(void) {
    current_completions.count = 0;
    selected_completion = -1;
    if (app_state.mode == mode_completion) {
        app_state.mode = mode_command;
    }
}

void cycle_completion(int direction) {
    if (current_completions.count == 0) return;

    selected_completion += direction;

    if (selected_completion < 0) {
        selected_completion = current_completions.count - 1;
    } else if (selected_completion >= current_completions.count) {
        selected_completion = 0;
    }
    apply_completion();
}

void apply_completion(void) {
    if (selected_completion < 0 || selected_completion >= current_completions.count) {
        return;
    }

    const char* suggestion = current_completions.suggestions[selected_completion];
    
    char *space = strrchr(app_state.command_buffer, ' ');
    if(space == NULL) { // It's a command
        snprintf(app_state.command_buffer, sizeof(app_state.command_buffer), ":%s", suggestion);
    } else { // It's an argument
        *(space + 1) = '\0';
        strcat(app_state.command_buffer, suggestion);
    }
}

void draw_completion_box(void) {
    if (current_completions.count == 0 || app_state.mode != mode_completion) {
        return;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    (void)max_y;

    wattron(status_win, COLOR_PAIR(CP_STATUS_COMMAND));
    werase(status_win);

    int start_x = 1;
    for (int i = 0; i < current_completions.count; i++) {
        if (i == selected_completion) {
            wattron(status_win, A_REVERSE);
        }
        mvwprintw(status_win, 0, start_x, "%s", current_completions.suggestions[i]);
        if (i == selected_completion) {
            wattroff(status_win, A_REVERSE);
        }
        start_x += strlen(current_completions.suggestions[i]) + 2;
        if (start_x > max_x - 2) {
            break;
        }
    }
    wrefresh(status_win);
    wattroff(status_win, COLOR_PAIR(CP_STATUS_COMMAND));
}

void complete_command(const char *input, CompletionResult *result) {
    result->count = 0;
    const char *prefix = input;
    if (*prefix == ':') {
        prefix++;
    }

    for (int i = 0; i < num_commands; i++) {
        if (strncmp(command_registry[i].name, prefix, strlen(prefix)) == 0) {
            strncpy(result->suggestions[result->count], command_registry[i].name, max_comp_len - 1);
            result->suggestions[result->count][max_comp_len - 1] = '\0';
            result->count++;
            if (result->count >= max_completions) {
                break;
            }
        }
    }
}

void complete_fqbn_arg(const char *input, CompletionResult *result) {
    const char *fqbns[] = {"arduino:avr:uno", "arduino:avr:mega", "esp32:esp32:esp32doit-devkit-v1", "esp8266:esp8266:nodemcuv2"};
    int num_fqbns = sizeof(fqbns) / sizeof(fqbns[0]);
    result->count = 0;

    for (int i = 0; i < num_fqbns; i++) {
        if (strncmp(fqbns[i], input, strlen(input)) == 0) {
            strncpy(result->suggestions[result->count], fqbns[i], max_comp_len - 1);
            result->suggestions[result->count][max_comp_len - 1] = '\0';
            result->count++;
            if (result->count >= max_completions) {
                break;
            }
        }
    }
}

void complete_port_arg(const char *input, CompletionResult *result) {
    const char *ports[] = {"/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyACM0", "COM1", "COM3"};
    int num_ports = sizeof(ports) / sizeof(ports[0]);
    result->count = 0;

    for (int i = 0; i < num_ports; i++) {
        if (strncmp(ports[i], input, strlen(input)) == 0) {
            strncpy(result->suggestions[result->count], ports[i], max_comp_len - 1);
            result->suggestions[result->count][max_comp_len - 1] = '\0';
            result->count++;
            if (result->count >= max_completions) {
                break;
            }
        }
    }
}

void complete_baud_arg(const char *input, CompletionResult *result) {
    const char *bauds[] = {"9600", "19200", "38400", "57600", "115200"};
    int num_bauds = sizeof(bauds) / sizeof(bauds[0]);
    result->count = 0;

    for (int i = 0; i < num_bauds; i++) {
        if (strncmp(bauds[i], input, strlen(input)) == 0) {
            strncpy(result->suggestions[result->count], bauds[i], max_comp_len - 1);
            result->suggestions[result->count][max_comp_len - 1] = '\0';
            result->count++;
            if (result->count >= max_completions) {
                break;
            }
        }
    }
}

void complete_color_arg(const char *input, CompletionResult *result) {
    const char *colors[] = {"#FF0000", "#FF7F00", "#FFFF00", "#00FF00", "#0000FF", "#4B0082", "#9400D3"};
    const char *color_names[] = {"Red", "Orange", "Yellow", "Green", "Blue", "Indigo", "Violet"};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    result->count = 0;

    for (int i = 0; i < num_colors; i++) {
        if (strncasecmp(color_names[i], input, strlen(input)) == 0 || strncmp(colors[i], input, strlen(input)) == 0) {
            strncpy(result->suggestions[result->count], colors[i], max_comp_len - 1);
            result->suggestions[result->count][max_comp_len - 1] = '\0';
            result->count++;
            if (result->count >= max_completions) {
                break;
            }
        }
    }
}
