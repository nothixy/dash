#ifndef DASH_H
#define DASH_H

#include <stdbool.h>
#include <stddef.h>


typedef struct {
    char opt_name;
    bool allow_flag_unset;
    bool param_optional;
    const char* param_name;
    const char* longopt_name;
    const char* description;
    void* user_pointer;
} dash_Longopt;

bool dash_arg_parser(int* argc, char* argv[], dash_Longopt* options);
void dash_print_usage(const char* argv0, const char* header, const char* footer, const char* required_arguments[], const dash_Longopt* options);
void dash_print_summary(int argc, char** argv, const dash_Longopt* options);

#endif
