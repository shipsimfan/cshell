#include <los.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hello world!\n");

    Event e;
    while (1) {
        while (peek_event(&e) != 0) {
            if (e.type == 1) {
                printf("%c", (char)e.param1);
            }
        }
    }
}
