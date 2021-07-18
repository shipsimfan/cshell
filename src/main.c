#include <los.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 1024
#define TAB_WIDTH 4

typedef struct {
    uint64_t one;
    uint64_t two;
} Pair;

char* arg_list[256];
char* command;

Pair read_char() {
    Event e;
    while (1) {
        while (peek_event(&e)) {
            if (e.type == EVENT_TYPE_KEY_PRESS) {
                Pair p;
                p.one = e.param1;
                p.two = e.param2;
                return p;
            }
        }
    }
}

char translate_keycode(uint64_t keycode, int caps) {
    if (caps) {
        switch (keycode) {
        case KEYCODE_SPACE:
            return ' ';
        case KEYCODE_QUOTE:
            return '"';
        case KEYCODE_COMMA:
            return ',';
        case KEYCODE_MINUS:
            return '_';
        case KEYCODE_PERIOD:
            return '.';
        case KEYCODE_FORWARD_SLASH:
            return '?';
        case KEYCODE_0:
            return ')';
        case KEYCODE_1:
            return '!';
        case KEYCODE_2:
            return '@';
        case KEYCODE_3:
            return '#';
        case KEYCODE_4:
            return '$';
        case KEYCODE_5:
            return '%';
        case KEYCODE_6:
            return '^';
        case KEYCODE_7:
            return '&';
        case KEYCODE_8:
            return '*';
        case KEYCODE_9:
            return '(';
        case KEYCODE_SEMI_COLON:
            return ':';
        case KEYCODE_EQUAL:
            return '+';
        case KEYCODE_OPEN_SQUARE_BRACKET:
            return '{';
        case KEYCODE_BACKSLASH:
            return '|';
        case KEYCODE_CLOSE_SQUARE_BRACKET:
            return '}';
        case KEYCODE_TICK:
            return '~';
        default:
            return ((char)keycode) - 'a' + 'A';
        }
    } else {
        return (char)keycode;
    }
}

uint64_t read_line(char* buffer, uint64_t buffer_length) {
    uint64_t idx = 0;
    while (idx < buffer_length) {
        Pair p = read_char();
        uint64_t keycode = p.one;
        uint64_t keystate = p.two;

        if (keycode == KEYCODE_NUM_ASTERICK) {
            buffer[idx] = '*';
            console_write('*');
            idx++;
        } else if (keycode == KEYCODE_NUM_MINUS) {
            buffer[idx] = '-';
            console_write('-');
            idx++;
        } else if (keycode == KEYCODE_NUM_PLUS) {
            buffer[idx] = '+';
            console_write('+');
            idx++;
        } else if (keycode == KEYCODE_NUM_PERIOD) {
            buffer[idx] = '.';
            console_write('.');
            idx++;
        } else if ((keycode >= KEYCODE_SPACE && keycode <= KEYCODE_EQUAL) || (keycode >= KEYCODE_OPEN_SQUARE_BRACKET && keycode <= KEYCODE_Z)) {
            int caps_status = 0;
            if (keystate & KEY_STATE_CAPS_LOCK) {
                caps_status = !caps_status;
            }

            if ((keystate & KEY_STATE_LEFT_SHIFT) || (keystate & KEY_STATE_RIGHT_SHIFT)) {
                caps_status = !caps_status;
            }

            char c = translate_keycode(keycode, caps_status);

            buffer[idx] = c;
            console_write(c);
            idx++;
        } else if (keycode == KEYCODE_BACKSPACE) {
            if (idx > 0) {
                idx--;
                console_write('\b');
            }
        } else if (keycode == KEYCODE_TAB) {
            if (idx % TAB_WIDTH == 0) {
                buffer[idx] = ' ';
                console_write(' ');
                idx++;
            }

            for (int i = 0; i < TAB_WIDTH && idx < buffer_length && idx % TAB_WIDTH != 0; i++, idx++) {
                buffer[idx] = ' ';
                console_write(' ');
            }
        } else if (keycode == KEYCODE_ENTER) {
            buffer[idx] = 0;
            console_write('\n');
            return idx;
        }
    }

    return idx;
}

int parse_arguments(char* buffer) {
    command = strtok(buffer, " ");
    arg_list[0] = command;

    for (int i = 1; i < 256; i++) {
        arg_list[i] = strtok(NULL, " ");
        if (arg_list[i] == NULL)
            return i;
    }

    return 255;
}

int main(int argc, const char** argv, const char** envp) {
    console_clear();
    console_write_str("\n    Lance OS Shell\n");
    console_write_str("======================\n\n");

    char cwd_buffer[BUFFER_LENGTH];
    char buffer[BUFFER_LENGTH];

    while (1) {
        get_current_working_directory(cwd_buffer, BUFFER_LENGTH);
        console_write_str(cwd_buffer);
        console_write_str("> ");
        read_line(buffer, BUFFER_LENGTH);
        if (buffer[0] == 0) {
            continue;
        }

        int argc = parse_arguments(buffer);

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "cd") == 0) {
            if (argc >= 2) {
                if (set_current_working_directory(arg_list[1]) != 0) {
                    console_write_str("No such directory: ");
                    console_write_str(arg_list[1]);
                    console_write('\n');
                }
            }
        } else if (strcmp(command, "echo") == 0) {
            for (int i = 1; i < argc; i++) {
                console_write_str(arg_list[i]);
                console_write(' ');
            }
            console_write('\n');
        } else if (strcmp(command, "clear") == 0) {
            console_clear();
        } else if (strcmp(command, "export") == 0) {
            if (argc == 3) {
                setenv(arg_list[1], arg_list[2], 1);
            } else {
                printf("Usage: export name value\n");
            }
        } else if (strcmp(command, "printenv") == 0) {
            if (argc == 1) {
                char** envp = environ;
                while (*envp) {
                    printf("%s\n", *envp);
                    envp++;
                }
            } else if (argc == 2) {
                char* environment_variable = getenv(arg_list[1]);
                if (environment_variable != NULL)
                    printf("%s\n", environment_variable);
            } else {
                printf("Usage: printenv [name]");
            }
        } else {
            ProcessID pid = execute(command, (const char**)arg_list, (const char**)environ);
            if (pid == 0xFFFFFFFFFFFFFFFF) {
                printf("Command not found: %s\n", command);
                continue;
            }

            uint64_t status = wait_process(pid);

            if (status != 0) {
                printf("Process exited with status %#lX\n", status);
            }
        }
    }
}
