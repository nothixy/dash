#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <stdbool.h>
#include <stddef.h>

enum ARGUMENT_REQUIRE_LEVEL {
    ARGUMENT_REQUIRE_LEVEL_NONE,
    ARGUMENT_REQUIRE_LEVEL_OPTIONAL,
    ARGUMENT_REQUIRE_LEVEL_REQUIRED
};

typedef struct _longopt {
    void* address_of_flag_value;
    const char* longopt_name;
    char opt_name;
    enum ARGUMENT_REQUIRE_LEVEL argument_require_level;
    bool allow_flag_unset;
    const char* help_param_name;
    const char* help_description;
} Longopt;

bool arg_parser(int* argc, char* argv[], struct _longopt* options);
void print_usage(const char* argv0, const char* header, const char* footer, char* required_arguments[], const struct _longopt* options);
void print_summary(const int* argc, char* argv[], const struct _longopt* options);

#endif
