#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argument_parser.h"

typedef struct _arguments {
    bool display_help;
    bool always_export_variables;
    bool notify_background_processes;
    bool no_overwrite_files_redirection;
    bool exit_on_error;
    bool no_pathname_expansion;
    bool use_function_history;
    bool report_exit_status;
    bool error_on_undefined_expansion;
    bool print_input_on_stderr;
    bool trace_comand_on_stderr;
    bool interactive;
    bool read_from_standard_input;
    char* print_options;
    char* command_string;
} Arguments;

void setup_arguments(int* argc, char* argv[])
{
    int error_code = 0;

    Arguments args = {0};

    Longopt options[] = {
        {&(args.interactive), "interactive", 'i', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Start an interactive shell"},
        {&(args.command_string), "command", 'c', ARGUMENT_REQUIRE_LEVEL_REQUIRED, false, "line", "Execute $ as a command"},
        {&(args.read_from_standard_input), "stdin", 's', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Read commands from standard input"},
        {&(args.always_export_variables), NULL, 'a', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Always export variables on assignment"},
        {&(args.notify_background_processes), NULL, 'b', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Notify asynchronously of background completion"},
        {&(args.no_overwrite_files_redirection), NULL, 'C', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Don't overwrite files on redirect with >"},
        {&(args.exit_on_error), NULL, 'e', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "When any command fails, exit"},
        {&(args.no_pathname_expansion), NULL, 'f', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Disable pathname expansion"},
        {&(args.use_function_history), NULL, 'h', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Locate utilities on function definition"},
        {&(args.report_exit_status), NULL, 'm', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Print background process status changes before drawing PS1"},
        {&(args.print_options), NULL, 'o', ARGUMENT_REQUIRE_LEVEL_OPTIONAL, true, "option", "If $ is non null, set options to $, else show all enabled options, use +o to print in an inputable format"},
        {&(args.error_on_undefined_expansion), NULL, 'u', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Fail when expanding an unset parameter"},
        {&(args.print_input_on_stderr), NULL, 'v', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Write shell input to stderr"},
        {&(args.trace_comand_on_stderr), NULL, 'x', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Print every command after expansion before execution"},
        {&(args.display_help), "help", 0, ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Show this help message"},
        {&(args.display_help), "usage", 0, ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Show this help message"},
        {.longopt_name = 0, .opt_name = 0}
    };

    char* required_arguments[] = {
        "output_file",
        NULL
    };

    if (!arg_parser(argc, argv, options))
    {
        fputs("Invalid arguments\n", stdout);
        error_code = 1;
        goto print_usage;
    }

    if (args.display_help)
    {
        goto print_usage;
    }

    #ifdef DEBUG
    print_summary(argc, argv, options);
    #endif

    goto end;

print_usage:
    print_usage(argv[0], "example shell, version 0.0.1", "Home page : https://example.com/shell", required_arguments, options);
end:
    // free(print_options);
    free(args.command_string);
    exit(error_code);
}

int main(int argc, char* argv[])
{
    setup_arguments(&argc, argv);
}
