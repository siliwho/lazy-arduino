#ifndef COMPLETION_H
#define COMPLETION_H

#include "command.h"
#include <ncurses.h>

#define max_completions 32 
#define max_comp_len 128

typedef struct CompletionResult{
    char suggestions[max_completions][max_comp_len];
    int count;
} CompletionResult;

void trigger_completion(void);
void clear_completion(void);
void cycle_completion(int direction);
void draw_completion_box(void);
void apply_completion(void);

void complete_command(const char *input, CompletionResult *result);
void complete_fqbn_arg(const char *input, CompletionResult *result);
void complete_port_arg(const char *input, CompletionResult *result);
void complete_baud_arg(const char *input, CompletionResult *result);
void complete_color_arg(const char *input, CompletionResult *result);


#endif
