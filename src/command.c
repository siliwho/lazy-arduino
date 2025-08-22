#include "status.h"
#include "command.h"
#include "completion.h"
#include "state.h"
#include "logs.h"
#include "colors.h"
#include "pages.h"
#include "sketches.h"
#include "config.h"
#include "arduino.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void command_compile(const char *args);
void command_upload(const char *args);

void command_set_fqbn(const char *args) {
    if (strlen(args) > 0) {
        strncpy(target_fqbn, args, sizeof(target_fqbn) - 1);
        target_fqbn[sizeof(target_fqbn) - 1] = '\0';
        save_config();
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Target board set to: %s", target_fqbn);
        add_log(log_msg);
    } else {
        add_log("Usage: :setfqbn <your:board:fqbn>");
    }
}

void command_set_port(const char *args) {
    if (strlen(args) > 0) {
        strncpy(target_port, args, sizeof(target_port) - 1);
        target_port[sizeof(target_port) - 1] = '\0';
        save_config();
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Target port set to: %s", target_port);
        add_log(log_msg);
    } else {
        add_log("Usage: :setport /dev/your_port");
    }
}

void command_set_baud(const char *args){
    if(strlen(args) > 0){
        strncpy(target_baud, args, sizeof(target_baud) - 1);
        target_baud[sizeof(target_baud)-1 ] = '\0';
        save_config();
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Target baud rate set to: %s", target_baud);
        add_log(log_msg);
    } else{
        add_log("Usage: :setbaud <rate>");
    }
}

void command_newfile(const char *args) {
    char filename[128] = {0};
    sscanf(args, "%127s", filename);

    if (strlen(filename) == 0) {
        add_log("Error: :newfile requires a filename");
        return;
    }

    if (!strstr(filename, ".ino")) {
        strncat(filename, ".ino", sizeof(filename) - strlen(filename) - 1);
    }

    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "void setup() {\n");
        fprintf(file, "  // put your setup code here, to run once:\n\n");
        fprintf(file, "}\n\n");
        fprintf(file, "void loop() {\n");
        fprintf(file, "  // put your main code here, to run repeatedly:\n\n");
        fprintf(file, "}\n");
        fclose(file);

        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Created file: %s", filename);
        add_log(log_msg);
        load_sketches(".");
    } else {
        add_log("Error: Could not create file.");
    }
}

void command_color(const char *args) {
    if (strlen(args) > 0) {
        char hex_clr[8] = {0};
        sscanf(args, "%7s", hex_clr);

        if (hex_clr[0] == '#') {
            memmove(hex_clr, hex_clr + 1, strlen(hex_clr));
        }

        if (strlen(hex_clr) == 6) {
            bool is_valid = true;
            for(int i = 0; i < 6; i++) {
                if (!isxdigit(hex_clr[i])) {
                    is_valid = false;
                    break;
                }
            }

            if(is_valid) {
                char log_msg[64];
                snprintf(log_msg, sizeof(log_msg), "Set color to #%s", hex_clr);
                add_log(log_msg);
                apply_theme_hex(hex_clr);
            } else {
                add_log("Error: Invalid hex characters.");
            }
        } else {
            add_log("Error: Invalid hex format. Use #RRGGBB");
        }
    } else {
        for (int i = 0; i < NUM_PAGES; i++) {
            if (strcmp(page_registry[i].name, "Colors") == 0) {
                switch_page(i);
                break;
            }
        }
    }
}

const Command command_registry[] = {
    {"newfile", "Create a new .ino file", command_newfile, NULL},
    {"color", "Set theme from a hex color or open color picker", command_color, complete_color_arg},
    {"setfqbn", "Set the target board FQBN", command_set_fqbn, complete_fqbn_arg},
    {"setport", "Set the target serial port", command_set_port, complete_port_arg},
    {"setbaud", "Set the target baud rate", command_set_baud, complete_baud_arg},
    {"compile", "Compile the current sketch", command_compile, NULL},
    {"upload", "Upload the current sketch", command_upload, NULL}
};
const int num_commands = sizeof(command_registry) / sizeof(Command);

const Command* find_command(const char *name){
    for(int i = 0; i < num_commands; i++){
        if(strcmp(name, command_registry[i].name) == 0){
            return &command_registry[i];
        }
    }
    return NULL;
}

void process_command(const char *input) {
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *cmd_name = buffer;
    if (cmd_name[0] == ':') {
        cmd_name++;
    }

    char *args = strchr(cmd_name, ' ');
    if(args != NULL){
        *args = '\0';
        args++;
    } else{
        args = "";
    }

    const Command* cmd = find_command(cmd_name);
    if(cmd){
        char status[256];
        strncpy(status, "Executing: ", sizeof(status) - 1);
        strncat(status, cmd_name, sizeof(status) - strlen(status) - 1);
        strncat(status, "...", sizeof(status) - strlen(status) - 1);
        set_status(status);
        cmd->action(args);
        set_status("Idle");
    } else {
        char log_msg[256];
        strncpy(log_msg, "Error: Unknown command '", sizeof(log_msg) - 1);
        strncat(log_msg, cmd_name, sizeof(log_msg) - strlen(log_msg) - 1);
        strncat(log_msg, "'", sizeof(log_msg) - strlen(log_msg) - 1);
        add_log(log_msg);
    }
}

/* void compile_sketch(const char *sketch_path) { */
/*     (void)sketch_path; // Suppress unused parameter warning */
/*     add_log("Compiling sketch..."); */
/*     // Actual implementation will be added later */
/* } */

/* void upload_sketch(const char *sketch_path) { */
/*     (void)sketch_path; // Suppress unused parameter warning */
/*     add_log("Uploading sketch..."); */
/*     // Actual implementation will be added later */
/* } */

void command_compile(const char *args) {
    (void)args; // Suppress unused parameter warning
    if (sketch_count > 0) {
        compile_sketch(sketches[selected_sketch], target_fqbn);
    } else {
        add_log("No sketches to compile.");
    }
}

void command_upload(const char *args) {
    (void)args; // Suppress unused parameter warning
    if (sketch_count > 0) {
        upload_sketch(sketches[selected_sketch], target_fqbn, target_port);
    } else {
        add_log("No sketches to upload.");
    }
}
