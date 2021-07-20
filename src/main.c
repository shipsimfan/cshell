#include "shell.h"
#include <los.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LENGTH 1024

int main() {
    printf("\n    Lance OS Shell\n");
    printf("======================\n\n");

    char cwd[BUFFER_LENGTH];

    char* command_buffer;
    char** argv;

    while (1) {
        get_current_working_directory(cwd, BUFFER_LENGTH);
        printf("%s> ", cwd);
        read_line(&command_buffer);
        if (command_buffer[0] == 0) {
            free(command_buffer);
            continue;
        }

        int argc = parse_arguments(command_buffer, &argv);

        if (run_internal_command(argc, (const char*)argv) == 0) {
            ProcessID pid = execute(argv[0], (const char**)argv, (const char**)environ);
            if (pid < 0) {
                printf("Error while executing %s: %li\n", argv[0], pid);
                free(command_buffer);
                free(argv);
                continue;
            }

            int64_t status = wait_process(pid);

            if (status != 0)
                printf("Process exited with status %li\n", status);
        }

        free(command_buffer);
        free(argv);
    }
}
