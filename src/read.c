#include "shell.h"

#include <los.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB_WIDTH 4
#define BUFFER_STARTING_SIZE 2

typedef struct {
    usize code;
    usize state;
} KeyEvent;

KeyEvent read_char() {
    Event e;
    isize status;
    while (1) {
        status = poll_event(&e);
        if (status == EINT)
            continue;

        if (status < 0) {
            printf("Error while peeking event: %s\n", strerror(status));
            exit(1);
        }

        if (e.type == EVENT_TYPE_KEY_PRESS) {
            KeyEvent key;
            key.code = e.param1;
            key.state = e.param2;
            return key;
        }
    }
}

char translate_keycode(usize keycode, int caps) {
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
            if ((char)keycode >= 'a' && (char)keycode <= 'z')
                return ((char)keycode) - 'a' + 'A';
            return (char)keycode;
        }
    } else {
        return (char)keycode;
    }
}

usize insert_char(char** buffer, usize* buffer_length, char c, usize idx) {
    if (idx == *buffer_length) {
        usize new_length = *buffer_length * 2;
        char* new_buffer = (char*)malloc(new_length);
        for (usize i = 0; i < *buffer_length; i++)
            new_buffer[i] = (*buffer)[i];
        free(*buffer);
        *buffer_length = new_length;
        *buffer = new_buffer;
    }

    (*buffer)[idx] = c;

    return idx + 1;
}

usize read_line(char** buffer) {
    usize buffer_length = BUFFER_STARTING_SIZE;
    *buffer = (char*)malloc(buffer_length);

    usize idx = 0;
    while (1) {
        KeyEvent key = read_char();

        if (key.state & KEY_STATE_LEFT_CTRL || key.state & KEY_STATE_RIGHT_CTRL)
            continue;

        if (key.code == KEYCODE_NUM_ASTERICK) {
            idx = insert_char(buffer, &buffer_length, '*', idx);
            printf("*");
        } else if (key.code == KEYCODE_NUM_MINUS) {
            idx = insert_char(buffer, &buffer_length, '-', idx);
            printf("-");
        } else if (key.code == KEYCODE_NUM_PLUS) {
            idx = insert_char(buffer, &buffer_length, '+', idx);
            printf("+");
        } else if (key.code == KEYCODE_NUM_PERIOD) {
            idx = insert_char(buffer, &buffer_length, '.', idx);
            printf(".");
        } else if ((key.code >= KEYCODE_SPACE && key.code <= KEYCODE_EQUAL) ||
                   (key.code >= KEYCODE_OPEN_SQUARE_BRACKET &&
                    key.code <= KEYCODE_Z)) {
            int caps_status = 0;
            if (key.state & KEY_STATE_CAPS_LOCK)
                caps_status = !caps_status;

            if ((key.state & KEY_STATE_LEFT_SHIFT) ||
                (key.state & KEY_STATE_RIGHT_SHIFT))
                caps_status = !caps_status;

            char c = translate_keycode(key.code, caps_status);

            idx = insert_char(buffer, &buffer_length, c, idx);
            printf("%c", c);
        } else if (key.code == KEYCODE_BACKSPACE) {
            if (idx > 0) {
                idx--;
                printf("\b");
            }
        } else if (key.code == KEYCODE_TAB) {
            if (idx % TAB_WIDTH == 0) {
                idx = insert_char(buffer, &buffer_length, ' ', idx);
                printf(" ");
            }

            for (int i = 0;
                 i < TAB_WIDTH && idx < buffer_length && idx % TAB_WIDTH != 0;
                 i++, idx++) {
                idx = insert_char(buffer, &buffer_length, ' ', idx);
                printf(" ");
            }
        } else if (key.code == KEYCODE_ENTER) {
            idx = insert_char(buffer, &buffer_length, 0, idx);
            printf("\n");
            return idx;
        }
    }

    return idx;
}
