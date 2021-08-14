#ifndef __SHELL_H
#define __SHELL_H

#include <los/types.h>

usize read_line(char** buffer);
int parse_arguments(char* buffer, char*** argv);
int run_internal_command(int argc, const char** argv);

#endif