#include "shell.h"
#include <los.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 1024

const StandardIO stdio = {
    STDIO_TYPE_CONSOLE,
    0,
    STDIO_TYPE_CONSOLE,
    0,
    STDIO_TYPE_CONSOLE,
    0};

void execute_command(const char* command, const char** argv, int argc) {
    // Check for run type
    int wait = 1;
    if (strcmp(argv[argc - 1], "&") == 0) {
        wait = 0;
        argv[argc - 1] = NULL;
    }

    ProcessID pid = execute(command, (const char**)argv, (const char**)environ, &stdio);
    if (pid < 0)
        printf("Error while executing %s: %s\n", argv[0], strerror(pid));
    else {
        if (wait) {
            isize status = wait_process(pid);

            if (status != 0)
                printf("Process exited with status %li\n", status);
        } else
            printf("Started background process as %li\n", pid);
    }
}

void run_command(int argc, const char** argv) {
    int search_path = 1;
    int search_extension = 1;
    const char* ptr = (const char*)argv[0];
    while (*ptr) {
        if (*ptr == '\\' || *ptr == '/') {
            search_path = 0;
            break;
        }

        if (*ptr == '.')
            search_extension = 0;

        ptr++;
    }

    if (search_path) {
        char* original_path = getenv("PATH");
        if (original_path != NULL) {
            char* path = (char*)malloc(strlen(original_path) + 1);
            char* ptr = original_path;
            int i = 0;
            while (*ptr) {
                path[i] = *ptr;

                i++;
                ptr++;
            }

            path[i] = *ptr;

            int cmd_length = strlen(argv[0]);

            ptr = strtok(path, ";");
            while (ptr != NULL) {
                int path_length = strlen(ptr);
                int padding = 1;
                if (ptr[path_length - 1] != '\\' || ptr[path_length - 1] != '/')
                    padding = 2;

                int filepath_length = path_length + cmd_length + padding + (search_extension * 4);
                char* filepath = (char*)malloc(filepath_length);
                strcpy(filepath, ptr);
                if (padding == 2)
                    strcat(filepath, "/");
                strcat(filepath, argv[0]);

                isize fd = open_file(filepath, OPEN_READ);
                if (fd >= 0) {
                    close_file(fd);

                    execute_command(filepath, argv, argc);
                    return;
                } else if (search_extension) {
                    strcat(filepath, ".app");
                    isize fd = open_file(filepath, OPEN_READ);
                    if (fd >= 0) {
                        close_file(fd);
                        execute_command(filepath, argv, argc);
                        return;
                    }
                }

                ptr = strtok(NULL, ";");
            }
        }
    }

    execute_command(argv[0], argv, argc);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

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

        if (run_internal_command(argc, (const char**)argv) == 0)
            run_command(argc, (const char**)argv);

        free(command_buffer);
        free(argv);
    }
}
