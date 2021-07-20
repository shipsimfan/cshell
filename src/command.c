#include "shell.h"

#include <los.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmd_exit() { exit(0); }

void cmd_clear() { console_clear(); }

void cmd_cd(int argc, const char** argv) {
    if (argc >= 2) {
        int64_t status = set_current_working_directory(argv[1]);
        if (status < 0)
            printf("Error while opening %s: %li\n", argv[0], status);
    }
}

void cmd_export(int argc, const char** argv) {
    if (argc >= 3)
        setenv(argv[1], argv[2], 1);
}

int run_internal_command(int argc, const char** argv) {
    if (strcmp(argv[0], "exit") == 0)
        cmd_exit();
    else if (strcmp(argv[0], "clear") == 0)
        console_clear();
    else if (strcmp(argv[0], "cd") == 0)
        cmd_cd(argc, argv);
    else if (strcmp(argv[0], "export") == 0)
        cmd_export(argc, argv);
    else
        return 0;

    return 1;
}