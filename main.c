#include "argument_parser.h"

#include <stdio.h>
#include <stdlib.h>

void setup_arguments(int* argc, char* argv[])
{
    int error_code = 0;

    bool display_help = false;
    bool always_export_variables = false;
    bool notify_background_processes = false;
    bool no_overwrite_files_redirection = false;
    bool exit_on_error = false;
    bool no_pathname_expansion = false;
    bool use_function_history = false;
    bool report_exit_status = false;
    bool error_on_undefined_expansion = false;
    bool print_input_on_stderr = false;
    bool trace_comand_on_stderr = false;
    bool interactive = false;
    bool read_from_standard_input = false;
    char* print_options = NULL;
    char* command_string = NULL;

    Longopt options[] = {
        {&(interactive), "interactive", 'i', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Start an interactive shell"},
        {&(command_string), "command", 'c', ARGUMENT_REQUIRE_LEVEL_REQUIRED, false, "line", "Execute $ as a command"},
        {&(read_from_standard_input), "stdin", 's', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Read commands from standard input"},
        {&(always_export_variables), NULL, 'a', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Always export variables on assignment"},
        {&(notify_background_processes), NULL, 'b', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Notify asynchronously of background completion"},
        {&(no_overwrite_files_redirection), NULL, 'C', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Don't overwrite files on redirect with >"},
        {&(exit_on_error), NULL, 'e', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "When any command fails, exit"},
        {&(no_pathname_expansion), NULL, 'f', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Disable pathname expansion"},
        {&(use_function_history), NULL, 'h', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Locate utilities on function definition"},
        {&(report_exit_status), NULL, 'm', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Print background process status changes before drawing PS1"},
        {&(print_options), NULL, 'o', ARGUMENT_REQUIRE_LEVEL_OPTIONAL, true, "option", "If $ is non null, set options to $, else show all enabled options, use +o to print in an inputable format"},
        {&(error_on_undefined_expansion), NULL, 'u', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Fail when expanding an unset parameter"},
        {&(print_input_on_stderr), NULL, 'v', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Write shell input to stderr"},
        {&(trace_comand_on_stderr), NULL, 'x', ARGUMENT_REQUIRE_LEVEL_NONE, true, NULL, "Print every command after expansion before execution"},
        {&(display_help), "help", '\0', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Show this help message"},
        {&(display_help), "usage", '\0', ARGUMENT_REQUIRE_LEVEL_NONE, false, NULL, "Show this help message"},
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

#ifdef DEBUG
    fprintf(stderr, "Interactive = (bool) %d\n", interactive);
    fprintf(stderr, "Command string = (string) %s\n", command_string);
    fprintf(stderr, "Read from standard input = (bool) %d\n", read_from_standard_input);
    fprintf(stderr, "Always export variables = (bool) %d\n", always_export_variables);
    fprintf(stderr, "Notify background processes = (bool) %d\n", notify_background_processes);
    fprintf(stderr, "No overwrite files redirection = (bool) %d\n", no_overwrite_files_redirection);
    fprintf(stderr, "Exit on error = (bool) %d\n", exit_on_error);
    fprintf(stderr, "No pathname expansion = (bool) %d\n", no_pathname_expansion);
    fprintf(stderr, "Use function history = (bool) %d\n", use_function_history);
    fprintf(stderr, "Report exit status = (bool) %d\n", report_exit_status);
    fprintf(stderr, "Print options = (string) %s\n", print_options);
    fprintf(stderr, "Error on undefined expansion = (bool) %d\n", error_on_undefined_expansion);
    fprintf(stderr, "Print input on stderr = (bool) %d\n", print_input_on_stderr);
    fprintf(stderr, "Trace command on stderr = (bool) %d\n", trace_comand_on_stderr);
    fprintf(stderr, "Display help = (bool) %d\n", display_help);
    fprintf(stderr, "Remaining arguments: ");
    for (int i = 0; i < *argc; i++)
    {
        fprintf(stderr, "%s ", argv[i]);
    }
    fprintf(stderr, "\n");
#endif

    if (display_help)
    {
        goto print_usage;
    }

    goto end;

print_usage:
    print_usage(argv[0], "hixy's ysh, version 0.0.1", "Home page : https://hixy.tk/ysh", required_arguments, options);
end:
    free(print_options);
    free(command_string);
    exit(error_code);
}

int main(int argc, char* argv[])
{
    setup_arguments(&argc, argv);
}
