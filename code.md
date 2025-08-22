# Understanding the Lazy-Arduino Codebase

## Introduction

Welcome to the code explanation for your Lazy-Arduino project! This document is designed to walk you through the entire codebase, from the high-level structure down to the details of each file. The goal is to give you a clear understanding of how the application works, enabling you to modify and build upon it.

The application is a terminal-based user interface (TUI) for interacting with Arduino boards using the `arduino-cli` command-line tool. It's built in C and uses the `ncurses` library to create the text-based interface.

## High-Level Architecture

The application is built around a few core concepts:

1.  **Modularity**: The code is split into several files, each responsible for a specific piece of functionality (e.g., `sketches.c` handles everything related to Arduino sketches, `board.c` handles board detection, `ui.c` manages the user interface, etc.). This makes the code easier to understand and maintain.

2.  **State-Driven UI**: The application's behavior is controlled by a central state machine. The `app_state` variable (in `state.h`/`state.c`) keeps track of the current mode (Normal, Command, or Completion), which page is active, and which panel has focus.

3.  **Pages and Panels**: The UI is organized into "Pages." The main page is the "Dashboard," which contains several "Panels" (Sketches, Boards, Logs, Serial). This is a common pattern in TUI applications.

4.  **Event Loop**: The `main` function contains a `while(true)` loop that continuously waits for user input, processes it based on the current application state, and then redraws the UI.

## Core Concepts Explained

### The Application State (`state.h`)

This is the heart of the application's logic. The `appstate` struct holds all the critical information about the current status of the UI:

-   `appmode mode`: Determines how user input is handled.
    -   `mode_normal`: Standard interaction, like navigating lists.
    -   `mode_command`: The user is typing a command (e.g., `:setfqbn ...`).
    -   `mode_completion`: The user has triggered tab-completion for a command.
-   `int current_idx`: The index of the currently visible page (e.g., Dashboard, Color Picker).
-   `int focus_idx`: On the dashboard, this is the index of the currently focused *panel* (e.g., Sketches, Boards).
-   `char command_buffer[]`: Stores the text the user is typing in command mode.

### Pages (`pages.h`)

The UI is divided into pages. Each page is a full-screen view. The `Page` struct defines what a page is:

-   `const char *name`: The name of the page (e.g., "Dash").
-   `page_init init`: A function pointer to code that runs when the page is first opened.
-   `page_drw drw`: A function pointer to the code that draws the page's content.
-   `page_ipt ipt`: A function pointer that handles user input for that page.
-   `page_rsize resize`: A function pointer to handle terminal resizing.
-   `page_dest destroy`: A function pointer to clean up when the page is closed.

The `page_registry[]` array in `pages.c` lists all available pages in the application.

### Panels (`panel.h`)

A page can be divided into smaller windows called panels. The "Dashboard" page uses four panels. The `Panel` struct defines what a panel is:

-   `const char *title`: The title displayed in the panel's border.
-   `WINDOW *win`: An `ncurses` window pointer for this panel.
-   `panel_drw draw_func`: A function to draw the panel's content.
-   `panel_ipt inp_func`: A function to handle input when the panel is focused.

## File-by-File Breakdown

Here is a description of each file in the project.

---

### `main.c`

-   **Purpose**: This is the entry point and central hub of the application. It contains the main event loop.
-   **Key Functions**:
    -   `main()`: Initializes everything (UI, config), enters the main `while` loop to get user input, calls the appropriate handlers based on the application mode, and redraws the screen.
    -   `handle_command_mode()`: Processes keystrokes when the user is typing a command (e.g., handles backspace, enter, etc.).
    -   `handle_completion_mode()`: Processes keystrokes when the tab-completion box is active.
    -   `draw_command_bar()`: Draws the bar at the bottom of the screen where commands are typed.
    -   `check_dependencies()`: Ensures `arduino-cli` is installed before the app starts.

---

### `ui.h` / `ui.c`

-   **Purpose**: Manages the `ncurses` windows and color pairs. It's responsible for creating, resizing, and destroying the visual components of the application.
-   **Key Functions**:
    -   `init_ui()`: Starts `ncurses` mode and sets up initial color pairs.
    -   `end_ui()`: Cleans up `ncurses` before the program exits.
    -   `resize_windows()`: The core layout logic. It calculates the size and position of all panels on the dashboard based on the terminal size.
    -   `apply_theme()`: Changes the application's color scheme based on a selected color.
-   **Global Variables**:
    -   `sketch_win`, `board_win`, etc.: These are the `ncurses` `WINDOW` pointers for each panel.

---

### `config.h` / `config.c`

-   **Purpose**: Handles loading and saving configuration settings to a file (`~/.lazyduino/config.ini`).
-   **Key Functions**:
    -   `load_config()`: Reads the config file from disk into global variables. If the file doesn't exist, it calls `save_config()` to create a default one.
    -   `save_config()`: Writes the current settings (like target board and port) to the config file.
-   **Global Variables**:
    -   `target_fqbn`, `target_port`, `target_baud`: Store the user's target settings for compiling and uploading.

---

### `sketches.h` / `sketches.c`

-   **Purpose**: Manages finding, displaying, and interacting with `.ino` sketch files.
-   **Key Functions**:
    -   `load_sketches()`: Scans the current directory for files ending in `.ino` and populates the `sketches` array.
    -   `draw_sketches_panel()`: Draws the list of sketches in its panel. It highlights the `selected_sketch`.
    -   `handle_sketches_input()`: Handles user input for the sketches panel (up/down navigation, 'c' to compile, 'u' to upload, Enter to edit).
    -   `open_in_editor()`: Opens the selected sketch file in the user's default text editor (`$EDITOR`).

---

### `board.h` / `board.c`

-   **Purpose**: Detects connected Arduino boards.
-   **Key Functions**:
    -   `get_boards()`: Runs the command `arduino-cli board list --format json`, then parses the JSON output to find connected boards and their ports.
    -   `draw_boards_panel()`: Displays the list of detected boards in its panel.
-   **Note**: The JSON parsing in this file is done manually by searching for keywords. This is fragile and could be improved by using a dedicated JSON parsing library.

---

### `command.h` / `command.c`

-   **Purpose**: Defines and processes the commands that can be typed in the command bar (e.g., `:setfqbn`).
-   **Key Structures**:
    -   `Command`: A struct that holds the name, description, action function, and completion function for a command.
-   **Key Functions**:
    -   `process_command()`: Takes the raw input from the command bar, finds the corresponding command in the `command_registry`, and calls its action function.
    -   `command_set_fqbn()`, `command_set_port()`, etc.: These are the actual functions that execute the commands.
-   **Global Variables**:
    -   `command_registry[]`: An array of all available commands in the application.

---

### `completion.h` / `completion.c`

-   **Purpose**: Handles the tab-completion logic for commands and their arguments.
-   **Key Functions**:
    -   `trigger_completion()`: Called when Tab is pressed in command mode. It figures out what to complete (a command name or an argument) and calls the appropriate completion function.
    -   `draw_completion_box()`: Draws the suggestions in the status bar.
    -   `cycle_completion()`: Moves the highlight to the next/previous suggestion.
    -   `apply_completion()`: Fills the command buffer with the selected suggestion.
    -   `complete_command()`, `complete_fqbn_arg()`, etc.: Functions that generate lists of possible completions.

---

### `logs.h` / `logs.c`

-   **Purpose**: Manages a simple, in-memory logging system to display status messages to the user.
-   **Key Functions**:
    -   `add_log()`: Adds a new message to the log buffer. If the buffer is full, it discards the oldest message.
    -   `draw_logs_panel()`: Draws the log messages in their panel.

---

### `arduino.h` / `arduino.c`

-   **Purpose**: A wrapper around the `arduino-cli` tool. It builds and runs the compile and upload commands.
-   **Key Functions**:
    -   `run_command()`: A generic function that temporarily exits `ncurses` mode to run a shell command and display its output directly in the terminal. This is used for showing the output of `arduino-cli`.
    -   `compile_sketch()`: Builds the `arduino-cli compile ...` command.
    -   `upload_sketch()`: Builds the `arduino-cli upload ...` command.

---

### `colors.h` / `color_picker.h` / `colors.c` / `color_picker.c`

-   **Purpose**: These files manage the theme and color selection.
-   **Key Functions**:
    -   `draw_color_picker_page()`: Draws the grid of 256 colors for the user to choose from.
    -   `handle_color_picker_input()`: Handles arrow key navigation on the color grid.
    -   `apply_theme()`: The main theming function in `ui.c`. It takes a base color and sets up all the application's color pairs based on it.
    -   `hex_to_rgb()` / `rgb_to_256()`: Helper functions to convert color formats.

---

### `serial.h` / `serial.c`

-   **Purpose**: Intended to provide a serial monitor.
-   **Status**: This feature is currently a placeholder. The functions are defined but contain `// TODO` comments, meaning the implementation is not yet complete.

---

### `status.h` / `status.c`

-   **Purpose**: Manages the status bar at the bottom of the screen.
-   **Key Functions**:
    -   `draw_status()`: This is a complex drawing function. It draws the current mode (`NORMAL`, `COMMAND`), the page tabs (F1:Dash, F2:Boards, etc.), and right-aligned information like the target board and focused panel. It changes colors and styles based on the application state.

## Execution Flow (The `main` function)

Let's trace the execution of the program when you run it:

1.  **`main()` starts.**
2.  `check_dependencies()`: It first makes sure `arduino-cli` is installed.
3.  `load_config()`: It loads your saved settings from `~/.lazyduino/config.ini`.
4.  `init_ui()`: It initializes the `ncurses` library, which clears the terminal and allows the program to draw anywhere on the screen. It also calls `resize_windows()` to create the initial set of panels.
5.  **Initial State**: The `app_state` is set to `mode_normal` on the "Dashboard" page (`current_idx = 0`) with the "Sketches" panel focused (`focus_idx = 0`).
6.  `load_sketches(".")`: It finds all `.ino` files in the current directory.
7.  `get_boards()`: It checks for any connected Arduino boards.
8.  `init_current_page()`: It calls the `init` function for the Dashboard page (`dashboard_init`), which sets up the `dashboard_panels` array.
9.  **The Main Loop (`while(true)`) begins:**
    a. `draw_current_page()`: This calls the `drw` function for the dashboard (`dashboard_draw`), which in turn calls the `draw_func` for each of its four panels (`draw_sketches_panel`, `draw_boards_panel`, etc.).
    b. `draw_command_bar()`: The command buffer is drawn at the bottom.
    c. `doupdate()`: This is a crucial `ncurses` function. It takes all the drawing that has happened in memory (in the previous steps) and pushes it to the actual terminal screen, making it visible all at once to prevent flickering.
    d. `getch()`: The program waits here for you to press a key.
    e. **Input Handling**:
        - If the mode is `mode_normal`, it might switch pages (F-keys), switch focus (`	`), or pass the key to the focused panel's input handler (`handle_sketches_input`). If you press `:`, it switches to `mode_command`.
        - If the mode is `mode_command`, `handle_command_mode` takes over to manage the text input.
        - If the mode is `mode_completion`, `handle_completion_mode` manages navigating the suggestions.
    f. The loop repeats, redrawing the screen with any new changes.
10. **Exiting**: If you press 'q' in normal mode, the loop breaks.
11. `end_ui()`: This function is called to shut down `ncurses` gracefully, returning your terminal to its normal state.
12. The program exits.

I hope this detailed breakdown is helpful. Feel free to ask if any part is unclear or if you'd like to dive deeper into a specific function!
