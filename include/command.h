#ifndef COMMAND_H
#define COMMAND_H

struct CompletionResult;

typedef void (*command_act)(const char *args);
typedef void (*completion_func)(const char *input, struct CompletionResult *result) ;

typedef struct {
    const char *name;
    const char *description;
    command_act action;
    completion_func complete;
} Command;

void process_command(const char *input);
const Command* find_command(const char *name);

#endif // COMMAND_H
