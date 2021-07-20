#include "shell.h"

#include <stdlib.h>
#include <string.h>

int parse_arguments(char* buffer, char*** argv) {
    int argv_length = 1;
    int idx = 0;
    *argv = (char**)malloc(sizeof(char*) * argv_length);
    (*argv)[idx] = strtok(buffer, " ");

    while ((*argv)[idx] != NULL) {
        idx++;

        if (idx == argv_length) { // Reallocate argv
            int new_length = argv_length * 2;
            char** new_argv = (char**)malloc(sizeof(char*) * new_length);
            for (int i = 0; i < argv_length; i++)
                new_argv[i] = (*argv)[i];
            free(*argv);
            argv_length = new_length;
            *argv = new_argv;
        }

        (*argv)[idx] = strtok(NULL, " ");
    }

    return idx;
}
