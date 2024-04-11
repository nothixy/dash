#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
#endif

#include "argument_parser.h"

enum COLORS {
    COLOR_BLUE,
    COLOR_RED,
    COLOR_GREEN
};

static void print_stderr_in_color(const char* str, enum COLORS color)
{
    #if defined(_WIN32) || defined(WIN32)
        static HANDLE  hConsole = NULL;
        WORD windows_color_code;
        switch(color)
        {
            case COLOR_BLUE:
                windows_color_code = FOREGROUND_BLUE;
                break;
            case COLOR_RED:
                windows_color_code = FOREGROUND_RED;
                break;
            case COLOR_GREEN:
                windows_color_code = FOREGROUND_GREEN;
                break;
        }

        if(hConsole == NULL)
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, windows_color_code);
        fprintf(stderr, "%s", str);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    #else
        int linux_color_code;
        switch(color)
        {
            case COLOR_BLUE:
                linux_color_code = 34;
                break;
            case COLOR_RED:
                linux_color_code = 31;
                break;
            case COLOR_GREEN:
                linux_color_code = 32;
        }
        fprintf(stderr, "\033[%dm%s\033[0m", linux_color_code, str);
    #endif
}


void print_usage(const char* argv0, const char* header, const char* footer, char* required_arguments[], struct _longopt* options)
{
    // Print header
    fputs(header, stderr);
    fputc('\n', stderr);
    fprintf(stderr, "Usage: %s [options]", argv0);
    if (required_arguments != NULL)
    {
        for (int i = 0; required_arguments[i] != NULL; i++)
        {
            fputc(' ', stderr);
            fputs(required_arguments[i], stderr);
        }
    }
    fputc('\n', stderr);

    // Calculate the number of spaces to align descriptions
    int max_length = 0;
    for (int i = 0; options[i].opt_name != '\0' || options[i].longopt_name != NULL; i++)
    {
        int length = 0;
        if (options[i].opt_name != '\0' && options[i].longopt_name != NULL)
        {
            length += 2;
            // Comma between shortopt and longopt
        }
        if (options[i].opt_name != '\0')
        {
            length += 2;
            // If there is a shortopt
            if (options[i].allow_flag_unset)
            {
                // If this shortopt has an 'unset' version with '+'
                length += 3;
            }
        }
        if (options[i].longopt_name != NULL)
        {
            length += 2 + strlen(options[i].longopt_name);
            // If there is a longopt
        }
        if (options[i].argument_require_level == ARGUMENT_REQUIRE_LEVEL_OPTIONAL)
        {
            length += 2;
            // If this option has an optional argument, print it inside brackets
        }
        if (options[i].argument_require_level != ARGUMENT_REQUIRE_LEVEL_NONE)
        {
            length += strlen(options[i].help_param_name);
            // If this option can be given arguments
        }
        length += 8;
        if (length > max_length)
        {
            max_length = length;
        }
    }

    for (int i = 0; options[i].opt_name != '\0' || options[i].longopt_name != NULL; i++)
    {
        fputs("  ", stderr);
        bool has_short = false;
        bool has_comma = false;
        bool has_long = false;
        bool has_param = false;
        bool param_is_opt = false;
        bool has_unsetopt = false;
        int length = 0;
        if (options[i].opt_name != '\0' && options[i].longopt_name != NULL)
        {
            has_comma = true;
            length += 2;
        }
        if (options[i].opt_name != '\0')
        {
            has_short = true;
            length += 2;
            if (options[i].allow_flag_unset)
            {
                has_unsetopt = true;
                length += 3;
            }
        }
        if (options[i].longopt_name != NULL)
        {
            has_long = true;
            length += 2 + strlen(options[i].longopt_name);
        }
        if (options[i].argument_require_level == ARGUMENT_REQUIRE_LEVEL_OPTIONAL)
        {
            param_is_opt = true;
            length += 2;
        }
        if (options[i].argument_require_level != ARGUMENT_REQUIRE_LEVEL_NONE)
        {
            has_param = true;
            length += strlen(options[i].help_param_name);
        }
        length += 4;
        if (has_short)
        {
            fprintf(stderr, "-%c", options[i].opt_name);
            if (has_unsetopt)
            {
                fprintf(stderr, "/+%c", options[i].opt_name);
            }
        }
        if (has_comma)
        {
            fputs(", ", stderr);
        }
        if (has_long)
        {
            fprintf(stderr, "--%s", options[i].longopt_name);
        }
        fputs(" ", stderr);
        if (param_is_opt)
        {
            fputc('[', stderr);
        }
        if (has_param)
        {
            print_stderr_in_color(options[i].help_param_name, COLOR_BLUE);
        }
        if (param_is_opt)
        {
            fputc(']', stderr);
        }
        fputs("  ", stderr);
        int diff = max_length - length;
        for (int j = 0; j < diff; j++)
        {
            fputc(' ', stderr);
        }
        for (int j = 0; options[i].help_description[j] != '\0'; j++)
        {
            if (options[i].help_description[j] != '$')
            {
                fputc(options[i].help_description[j], stderr);
            }
            else
            {
                print_stderr_in_color(options[i].help_param_name, COLOR_BLUE);
            }
        }
        fputc('\n', stderr);
    }
    fputs(footer, stderr);
    fputc('\n', stderr);
}

int strcmp_until_delimiter(const char* str1, const char* str2, char delimiter, int* index_of_delimiter)
{
    *index_of_delimiter = 0;
    while (*str1 != '\0')
    {
        if (*str1 == delimiter && *str2 == '\0')
        {
            return 0;
        }
        if (*str2 == '\0')
        {
            return -1;
        }
        if (*str1 != *str2)
        {
            return *str1 - *str2;
        }
        str1++;
        str2++;
        (*index_of_delimiter)++;
    }
    if (*str2 != '\0' && *str2 != delimiter)
    {
        return 1;
    }
    return 0;
}

int assign_longopt(char** argument, struct _longopt* options, int structure_length, bool* arg_provided_with_equal)
{
    // Search through all allowed arguments
    for (int i = 0; i < structure_length; i++)
    {
        *arg_provided_with_equal = false;
        if (options[i].longopt_name == NULL)
        {
            continue;
        }

        // Check if argument is longopt with ' ' delimiter
        if (!strcmp(options[i].longopt_name, &(*argument)[2]))
        {
            if (options[i].argument_require_level == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                *((bool*) options[i].address_of_flag_value) = true;
            }
            *argument = NULL;
            return i;
        }

        // Check if argument is longopt with '=' delimiter
        int index_of_delimiter;
        if (options[i].argument_require_level != ARGUMENT_REQUIRE_LEVEL_NONE && !strcmp_until_delimiter(options[i].longopt_name, &(*argument)[2], '=', &index_of_delimiter))
        {
            if ((*argument)[index_of_delimiter + 3] == '\0')
            {
                return -1;
            }
            *arg_provided_with_equal = true;
            int argument_length = strlen(&(*argument)[index_of_delimiter + 3]);
            if (* ((char**) options[i].address_of_flag_value) != NULL)
            {
                return -1;
            }
            * ((char**) options[i].address_of_flag_value) = malloc((argument_length + 1) * sizeof(char));
            char* dest_addr = * ((char**) options[i].address_of_flag_value);
            strcpy(&dest_addr[options[i].allow_flag_unset], &(*argument)[index_of_delimiter + 3]);
            *argument = NULL;
            return i;
        }
    }
    return -1;
}

int assign_shortopt(char argument, struct _longopt* options, int structure_length, bool unset)
{
    // Search through all allowed arguments
    for (int i = 0; i < structure_length; i++)
    {
        // Check if argument is what we want
        if (options[i].opt_name != '\0' && argument == options[i].opt_name)
        {
            if (unset && !options[i].allow_flag_unset)
            {
                return -1;
            }
            if (options[i].argument_require_level == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                *((bool*) options[i].address_of_flag_value) = !unset;
            }
            return i;
        }
    }
    return -1;
}

bool arg_parser(int* argc, char* argv[], struct _longopt* options)
{
    int argument_non_option_index = 1;
    int argument_non_option_count = 1;

    int structure_length = 0;
    while (options[structure_length].opt_name != '\0' || options[structure_length].longopt_name != NULL)
    {
        // Can't dereference a NULL pointer
        if (options[structure_length].address_of_flag_value == NULL)
        {
            return false;
        }
        structure_length++;
    }

    int found_structure_index = -1;
    bool last_opt_was_unset = false;
    bool long_opt_was_provided_with_equal = false;

    for (int i = 1; i < *argc; i++)
    {
        // Previous flag did not specify argument
        if (found_structure_index != -1 && !long_opt_was_provided_with_equal)
        {
            // No argument required, good
            if (options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                found_structure_index = -1;
            }
            // We need an argument
            else if (options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_REQUIRED)
            {
                if (argv[i][0] == '-')
                {
                    return false;
                }
                int argument_length = strlen(argv[i]);
                if (options[found_structure_index].allow_flag_unset)
                {
                    argument_length += 1;
                }
                if (* ((char**) options[found_structure_index].address_of_flag_value) != NULL)
                {
                    return false;
                }
                * ((char**) options[found_structure_index].address_of_flag_value) = malloc((argument_length + 1) * sizeof(char));
                char* dest_addr = * ((char**) options[found_structure_index].address_of_flag_value);
                strcpy(&dest_addr[options[found_structure_index].allow_flag_unset], argv[i]);
                if (options[found_structure_index].allow_flag_unset)
                {
                    dest_addr[0] = last_opt_was_unset ? '+' : '-';
                }
                argv[i] = NULL;
                found_structure_index = -1;
                continue;
            }
            // Try to search for an argument
            else
            {
                if (argv[i][0] != '-')
                {
                    int argument_length = strlen(argv[i]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        argument_length += 1;
                    }
                    if (* ((char**) options[found_structure_index].address_of_flag_value) != NULL)
                    {
                        return false;
                    }
                    * ((char**) options[found_structure_index].address_of_flag_value) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address_of_flag_value);
                    strcpy(&dest_addr[options[found_structure_index].allow_flag_unset], argv[i]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        dest_addr[0] = last_opt_was_unset ? '+' : '-';
                    }
                    argv[i] = NULL;
                    found_structure_index = -1;
                    continue;
                }
                else
                {
                    * ((char**) options[found_structure_index].address_of_flag_value) = malloc((1 + options[found_structure_index].allow_flag_unset) * sizeof(char));
                    (* ((char**) options[found_structure_index].address_of_flag_value))[options[found_structure_index].allow_flag_unset] = '\0';
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        (* ((char**) options[found_structure_index].address_of_flag_value))[0] = last_opt_was_unset ? '+' : '-';
                    }
                    found_structure_index = -1;
                }
            }
        }

        long_opt_was_provided_with_equal = false;
        found_structure_index = -1;

        // Check if argument begins with a dash or a plus
        if (argv[i][0] != '-' && argv[i][0] != '+')
        {
            argument_non_option_count += 1;
            continue;
        }

        // If argument begins with a plus, unset shortopt
        if (argv[i][0] == '+')
        {
            last_opt_was_unset = true;
            int c = 1;
            bool option_should_have_argument = false;
            while (argv[i][c] != '\0')
            {
                if (option_should_have_argument == true)
                {
                    int argument_length = strlen(&argv[i][c]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        argument_length += 1;
                    }
                    if (* ((char**) options[found_structure_index].address_of_flag_value) != NULL)
                    {
                        return false;
                    }
                    * ((char**) options[found_structure_index].address_of_flag_value) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address_of_flag_value);
                    strcpy(&dest_addr[options[found_structure_index].allow_flag_unset], &argv[i][c]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        dest_addr[0] = '+';
                    }
                    found_structure_index = -1;
                    break;
                }
                if ((found_structure_index = assign_shortopt(argv[i][c], options, structure_length, true)) == -1)
                {
                    return false;
                }
                option_should_have_argument = (options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_REQUIRED);
                c++;
            }
            argv[i] = NULL;
            continue;
        }

        last_opt_was_unset = false;

        // Double dash, long opt
        if (argv[i][1] == '-')
        {
            // Only double dash, end of parsing arguments
            if (argv[i][2] == '\0')
            {
                argument_non_option_count += *argc - i - 1;
                argv[i] = NULL;
                goto REORGANIZE;
            }
            if ((found_structure_index = assign_longopt(&argv[i], options, structure_length, &long_opt_was_provided_with_equal)) == -1)
            {
                return false;
            }
        }
        // Single dash, ignore (will be used as stdin)
        else if (argv[i][1] == '\0')
        {
            argument_non_option_count += 1;
            continue;
        }
        // Short opt
        else
        {
            int c = 1;
            bool option_should_have_argument = false;
            while (argv[i][c] != '\0')
            {
                if (option_should_have_argument == true)
                {
                    int argument_length = strlen(&argv[i][c]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        argument_length += 1;
                    }
                    if (* ((char**) options[found_structure_index].address_of_flag_value) != NULL)
                    {
                        return false;
                    }
                    * ((char**) options[found_structure_index].address_of_flag_value) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address_of_flag_value);
                    strcpy(&dest_addr[options[found_structure_index].allow_flag_unset], &argv[i][c]);
                    if (options[found_structure_index].allow_flag_unset)
                    {
                        dest_addr[0] = '-';
                    }
                    found_structure_index = -1;
                    break;
                }
                if ((found_structure_index = assign_shortopt(argv[i][c], options, structure_length, false)) == -1)
                {
                    return false;
                }
                option_should_have_argument = (options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_REQUIRED);
                c++;
            }
            argv[i] = NULL;
        }
    }

REORGANIZE:

    if (found_structure_index != -1 && options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_OPTIONAL && !long_opt_was_provided_with_equal)
    {
        if (* ((char**) options[found_structure_index].address_of_flag_value) != NULL)
        {
            return false;
        }
        * ((char**) options[found_structure_index].address_of_flag_value) = malloc((1 + options[found_structure_index].allow_flag_unset) * sizeof(char));
        (* ((char**) options[found_structure_index].address_of_flag_value))[options[found_structure_index].allow_flag_unset] = '\0';
        if (options[found_structure_index].allow_flag_unset)
        {
            (* ((char**) options[found_structure_index].address_of_flag_value))[0] = last_opt_was_unset ? '+' : '-';
        }
    }

    if (found_structure_index != -1 && options[found_structure_index].argument_require_level == ARGUMENT_REQUIRE_LEVEL_REQUIRED && !long_opt_was_provided_with_equal)
    {
        return false;
    }

    for (int i = 1; i < *argc; i++)
    {
        if (argv[i] != NULL)
        {
            argv[argument_non_option_index] = argv[i];
            if (i != argument_non_option_index++)
            {
                argv[i] = NULL;
            }
        }
    }

    *argc = argument_non_option_count;

    return true;
}

void print_summary(int* argc, char* argv[], struct _longopt* options)
{
    int structure_length = 0;
    while (options[structure_length].opt_name != '\0' || options[structure_length].longopt_name != NULL)
    {
        structure_length++;
    }
    for (int i = 0; i < structure_length; i++)
    {
        if (options[i].argument_require_level == ARGUMENT_REQUIRE_LEVEL_NONE)
        {
            if (options[i].longopt_name)
            {
                fprintf(stderr, "%-35s = (bool) ", options[i].longopt_name);
            }
            else
            {
                fprintf(stderr, "%-35c = (bool) ", options[i].opt_name);
            }
            bool value;
            if ((value = (* (bool*) options[i].address_of_flag_value)))
            {
                print_stderr_in_color("True", COLOR_GREEN);
            }
            else
            {
                print_stderr_in_color("False", COLOR_RED);
            }
            fputc('\n', stderr);
        }
        else
        {
            if (options[i].longopt_name)
            {
                fprintf(stderr, "%-35s = (string) ", options[i].longopt_name);
            }
            else
            {
                fprintf(stderr, "%-35c = (string) ", options[i].opt_name);
            }
            char* value;
            if ((value = * (char**) options[i].address_of_flag_value))
            {
                print_stderr_in_color(value, COLOR_BLUE);
            }
            else
            {
                fprintf(stderr, "%s", value);
            }
            fputc('\n', stderr);
        }
    }
    fputs("Remaining arguments: ", stderr);
    for (int i = 0; i < *argc; i++)
    {
        fprintf(stderr, "%s ", argv[i]);
    }
    fputc('\n', stderr);
}
