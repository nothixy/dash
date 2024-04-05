#include "argument_parser.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
            if (options[i].allow_unset)
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
        if (options[i].require_argument == ARGUMENT_REQUIRE_LEVEL_OPTIONAL)
        {
            length += 2;
            // If this option has an optional argument, print it inside brackets
        }
        if (options[i].require_argument != ARGUMENT_REQUIRE_LEVEL_NONE)
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
            if (options[i].allow_unset)
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
        if (options[i].require_argument == ARGUMENT_REQUIRE_LEVEL_OPTIONAL)
        {
            param_is_opt = true;
            length += 2;
        }
        if (options[i].require_argument != ARGUMENT_REQUIRE_LEVEL_NONE)
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
            fputs("\033[34m", stderr);
            fputs(options[i].help_param_name, stderr);
            fputs("\033[0m", stderr);
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
        int line_begin_help = max_length;
        int save_char_from_line = 0;
        for (int j = 0; options[i].help_description[j] != '\0'; j++)
        {
            if (line_begin_help + save_char_from_line >= 80)
            {
                fputc('\n', stderr);
                for (int k = 0; k < max_length + 3; k++)
                {
                    fputc(' ', stderr);
                }
                save_char_from_line = 4;
            }
            if (options[i].help_description[j] != '$')
            {
                fputc(options[i].help_description[j], stderr);
            }
            else
            {
                fputs("\033[34m", stderr);
                fputs(options[i].help_param_name, stderr);
                fputs("\033[0m", stderr);
            }
            save_char_from_line++;
        }
        fputc('\n', stderr);
    }
    fputs(footer, stderr);
    fputc('\n', stderr);
}

void free_on_error(struct _longopt* options, int structure_length)
{
    // Free all arguments that were allocated on error
    for (int i = 0; i < structure_length; i++)
    {
        if (options[i].require_argument != ARGUMENT_REQUIRE_LEVEL_NONE)
        {
            free(* ((char**) options[i].address));
        }
    }
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
            if (options[i].require_argument == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                *((bool*) options[i].address) = true;
            }
            *argument = NULL;
            return i;
        }

        // Check if argument is longopt with '=' delimiter
        int index_of_delimiter;
        if (options[i].require_argument != ARGUMENT_REQUIRE_LEVEL_NONE && !strcmp_until_delimiter(options[i].longopt_name, &(*argument)[2], '=', &index_of_delimiter))
        {
            if ((*argument)[index_of_delimiter + 3] == '\0')
            {
                return -1;
            }
            *arg_provided_with_equal = true;
            int argument_length = strlen(&(*argument)[index_of_delimiter + 3]);
            * ((char**) options[i].address) = malloc((argument_length + 1) * sizeof(char));
            char* dest_addr = * ((char**) options[i].address);
            strcpy(&dest_addr[options[i].allow_unset], &(*argument)[index_of_delimiter + 3]);
            *argument = NULL;
            return i;
        }
    }
    return -1;
}

int assign_shortopt(char argument, struct _longopt* options, int structure_length, bool unset)
{
    for (int i = 0; i < structure_length; i++)
    {
        if (options[i].opt_name != '\0' && argument == options[i].opt_name)
        {
            if (unset && !options[i].allow_unset)
            {
                return -1;
            }
            if (options[i].require_argument == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                *((bool*) options[i].address) = !unset;
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
        if (options[structure_length].address == NULL)
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
        // Previous flag requires an argument, detect it
        if (found_structure_index != -1 && !long_opt_was_provided_with_equal)
        {
            if (options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_NONE)
            {
                found_structure_index = -1;
            }
            else if (options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_REQUIRED)
            {
                if (argv[i][0] == '-')
                {
                    free_on_error(options, structure_length);
                    return false;
                }
                int argument_length = strlen(argv[i]);
                if (options[found_structure_index].allow_unset)
                {
                    argument_length += 1;
                }
                * ((char**) options[found_structure_index].address) = malloc((argument_length + 1) * sizeof(char));
                char* dest_addr = * ((char**) options[found_structure_index].address);
                strcpy(&dest_addr[options[found_structure_index].allow_unset], argv[i]);
                if (options[found_structure_index].allow_unset)
                {
                    dest_addr[0] = last_opt_was_unset ? '+' : '-';
                }
                argv[i] = NULL;
                found_structure_index = -1;
                continue;
            }
            else
            {
                if (argv[i][0] != '-')
                {
                    int argument_length = strlen(argv[i]);
                    if (options[found_structure_index].allow_unset)
                    {
                        argument_length += 1;
                    }
                    * ((char**) options[found_structure_index].address) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address);
                    strcpy(&dest_addr[options[found_structure_index].allow_unset], argv[i]);
                    if (options[found_structure_index].allow_unset)
                    {
                        dest_addr[0] = last_opt_was_unset ? '+' : '-';
                    }
                    argv[i] = NULL;
                    found_structure_index = -1;
                    continue;
                }
                else
                {
                    * ((char**) options[found_structure_index].address) = malloc((1 + options[found_structure_index].allow_unset) * sizeof(char));
                    (* ((char**) options[found_structure_index].address))[options[found_structure_index].allow_unset] = '\0';
                    if (options[found_structure_index].allow_unset)
                    {
                        (* ((char**) options[found_structure_index].address))[0] = last_opt_was_unset ? '+' : '-';
                    }
                    found_structure_index = -1;
                }
            }
        }

        long_opt_was_provided_with_equal = false;

        // Check if argument begins with a dash
        if (argv[i][0] != '-' && argv[i][0] != '+')
        {
            argument_non_option_count += 1;
            continue;
        }

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
                    if (options[found_structure_index].allow_unset)
                    {
                        argument_length += 1;
                    }
                    * ((char**) options[found_structure_index].address) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address);
                    strcpy(&dest_addr[options[found_structure_index].allow_unset], &argv[i][c]);
                    if (options[found_structure_index].allow_unset)
                    {
                        dest_addr[0] = '+';
                    }
                    found_structure_index = -1;
                    break;
                }
                if ((found_structure_index = assign_shortopt(argv[i][c], options, structure_length, true)) == -1)
                {
                    free_on_error(options, structure_length);
                    return false;
                }
                option_should_have_argument = (options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_REQUIRED);
                c++;
            }
            argv[i] = NULL;
            continue;
        }

        last_opt_was_unset = false;

        if (argv[i][1] == '-')
        {
            if (argv[i][2] == '\0')
            {
                argument_non_option_count += *argc - i - 1;
                argv[i] = NULL;
                goto REORGANIZE;
            }
            if ((found_structure_index = assign_longopt(&argv[i], options, structure_length, &long_opt_was_provided_with_equal)) == -1)
            {
                free_on_error(options, structure_length);
                return false;
            }
        }
        else if (argv[i][1] == '\0')
        {
            argument_non_option_count += 1;
            continue;
        }
        else
        {
            int c = 1;
            bool option_should_have_argument = false;
            while (argv[i][c] != '\0')
            {
                if (option_should_have_argument == true)
                {
                    int argument_length = strlen(&argv[i][c]);
                    if (options[found_structure_index].allow_unset)
                    {
                        argument_length += 1;
                    }
                    * ((char**) options[found_structure_index].address) = malloc((argument_length + 1) * sizeof(char));
                    char* dest_addr = * ((char**) options[found_structure_index].address);
                    strcpy(&dest_addr[options[found_structure_index].allow_unset], &argv[i][c]);
                    if (options[found_structure_index].allow_unset)
                    {
                        dest_addr[0] = '-';
                    }
                    found_structure_index = -1;
                    break;
                }
                if ((found_structure_index = assign_shortopt(argv[i][c], options, structure_length, false)) == -1)
                {
                    free_on_error(options, structure_length);
                    return false;
                }
                option_should_have_argument = (options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_REQUIRED);
                c++;
            }
            argv[i] = NULL;
        }
    }

REORGANIZE:

    if (found_structure_index != -1 && options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_OPTIONAL && !long_opt_was_provided_with_equal)
    {
        * ((char**) options[found_structure_index].address) = malloc((1 + options[found_structure_index].allow_unset) * sizeof(char));
        (* ((char**) options[found_structure_index].address))[options[found_structure_index].allow_unset] = '\0';
        if (options[found_structure_index].allow_unset)
        {
            (* ((char**) options[found_structure_index].address))[0] = last_opt_was_unset ? '+' : '-';
        }
    }

    if (found_structure_index != -1 && options[found_structure_index].require_argument == ARGUMENT_REQUIRE_LEVEL_REQUIRED && !long_opt_was_provided_with_equal)
    {
        free_on_error(options, structure_length);
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

#ifdef DEBUG
    for (int i = 0; i < *argc; i++)
    {
        fprintf(stderr, "%s ", argv[i]);
    }
    fprintf(stderr, "\n");
#endif

    return true;
}
