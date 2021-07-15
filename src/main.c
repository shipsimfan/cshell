#include <los.h>

#define BUFFER_LENGTH 1024

char read_char() {
    Event e;
    while (1) {
        while (peek_event(&e)) {
            if (e.type == 0) {
                return (char)e.param1;
            }
        }
    }
}

uint64_t read_line(char* buffer, uint64_t buffer_length) {
    uint64_t idx = 0;
    while (idx < buffer_length) {
        char c = read_char();
        if (c == 0x4B) {
            buffer[idx] = 0;
            console_write('\n');
            return idx;
        } else if (c == 0x3E) {
            if (idx > 0) {
                idx--;
                console_write('\b');
            }
            idx--;
        } else {
            buffer[idx] = c;
            console_write(c);
        }

        idx++;
    }
}

int main() {
    printf("\n  Lance OS Shell\n");
    printf("==================\n\n");

    char buffer[BUFFER_LENGTH];

    while (1) {
        console_write_str("> ");
        read_line(buffer, BUFFER_LENGTH);

        console_write_str(buffer);
        console_write('\n');
    }
}
