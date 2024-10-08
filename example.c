#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dash.h"

typedef struct {
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


static void print_help(const char* program_name, const dash_Longopt* options, FILE* output_file)
{
    const char* required_arguments[] = {
        "output_file",
        NULL
    };
    dash_print_usage(program_name, "example shell, version 0.0.1", "Home page : https://example.com/shell", required_arguments, options, output_file);
}


int main(int argc, char* argv[])
{
    int error_code = 1;

    Arguments args = {0};

    dash_Longopt options[] = {
        {.user_pointer = &(args.interactive), .longopt_name = "interactive", .opt_name = 'i', .description = "Start an interactive shell"},
        {.user_pointer = &(args.command_string), .longopt_name = "command", .opt_name = 'c', .param_name = "line", .description = "Execute $ as a command"},
        {.user_pointer = &(args.read_from_standard_input), .longopt_name = "stdin", .opt_name = 's', .description = "Read commands from standard input"},
        {.user_pointer = &(args.always_export_variables), .opt_name = 'a', .allow_flag_unset = true, .description = "Always export variables on assignment"},
        {.user_pointer = &(args.notify_background_processes), .opt_name = 'b', .allow_flag_unset = true, .description = "Notify asynchronously of background completion"},
        {.user_pointer = &(args.no_overwrite_files_redirection), .opt_name = 'C', .allow_flag_unset = true, .description = "Don't overwrite files on redirect with >"},
        {.user_pointer = &(args.exit_on_error), .opt_name = 'e', .allow_flag_unset = true, .description = "When any command fails, exit"},
        {.user_pointer = &(args.no_pathname_expansion), .opt_name = 'f', .allow_flag_unset = true, .description = "Disable pathname expansion"},
        {.user_pointer = &(args.use_function_history), .opt_name = 'h', .allow_flag_unset = true, .description = "Locate utilities on function definition"},
        {.user_pointer = &(args.report_exit_status), .opt_name = 'm', .allow_flag_unset = true, .description = "Print background process status changes before drawing PS1"},
        {.user_pointer = &(args.print_options), .opt_name = 'o', .allow_flag_unset = true, .param_optional = true, .param_name = "option", .description = "If $ is non null, set options to $, else show all enabled options, use +o to print in an inputable format"},
        {.user_pointer = &(args.error_on_undefined_expansion), .opt_name = 'u', .allow_flag_unset = true, .description = "Fail when expanding an unset parameter"},
        {.user_pointer = &(args.print_input_on_stderr), .opt_name = 'v', .allow_flag_unset = true, .description = "Write shell input to stderr"},
        {.user_pointer = &(args.trace_comand_on_stderr), .opt_name = 'x', .allow_flag_unset = true, .description = "Print every command after expansion before execution"},
        {.user_pointer = &(args.display_help), .longopt_name = "help", .description = "Show this help message"},
        {.user_pointer = &(args.display_help), .longopt_name = "usage", .description = "Show this help message"},
        {0}
    };

    if (!dash_arg_parser(&argc, argv, options))
    {
        fputs("Invalid arguments\n", stderr);
        print_help(argv[0], options, stderr);
        goto END;
    }

    if (args.display_help)
    {
        print_help(argv[0], options, stdout);
    }
    else
    {
        dash_print_summary(argc, argv, options, stdout);
    }

    error_code = 0;
END:
    free(args.print_options);
    free(args.command_string);
    
    return error_code;
}
