# C argument parsing library

## How to use it

Create a structure containing strings and booleans: make sure that you don't allocate your strings from the heap by doing
```c
char* argument = NULL;
```
and not
```c
char argument[50];
```

Create a 0-terminated array of Longopt options, one for each flag you want. A Longopt structure is defined as follows:
```c
typedef struct _longopt {
    void* address_of_flag_value;
    char* flag_name_long;
    char flag_name_short;
    enum ARGUMENT_REQUIRE_LEVEL argument_level;
    bool allow_flag_unset;
    char* argument_name_in_help;
    char* flag_description_in_help;
} Longopt;
```
where `ARGUMENT_REQUIRE_LEVEL` can be `ARGUMENT_REQUIRE_LEVEL_NONE`, `ARGUMENT_REQUIRE_LEVEL_OPTIONAL`, or `ARGUMENT_REQUIRE_LEVEL_REQUIRED`.
Furthermore, in flag_description_in_help, every `$` character will be replaced with `argument_name_in_help`

For example you could do:
```c
Longopt options[] = {
    {&argument, "my_flag", 'f', ARGUMENT_REQUIRE_LEVEL_OPTIONAL, false, "flag_argument", "Set value of my flag to $"},
    {0}
};
```

Then for every mandatory argument, create a new NULL-terminated string array, for example:
```c
char* required_arguments[] = {
    "output_stream",
    NULL
};
```

Then you can call arg_parser():
```c
if (!arg_parser(argc, argv, options))
{
    print_usage(argv[0], "help message header", "help message footer", required_argumnets, options);
    exit(1);
}
```

## What is supported

A complete example is available in `main.c`, in this example, you can call
`./main -i -c /bin/zsh v1 -s v2` for example, you should get this output:
```
interactive                         = (bool) True
command                             = (string) /bin/zsh
stdin                               = (bool) True
a                                   = (bool) False
b                                   = (bool) False
C                                   = (bool) False
e                                   = (bool) False
f                                   = (bool) True
h                                   = (bool) False
m                                   = (bool) False
o                                   = (string) +autocd salut
u                                   = (bool) False
v                                   = (bool) False
x                                   = (bool) False
help                                = (bool) False
usage                               = (bool) False
Remaining arguments: ./main v1 v2 
```
Other example:
`./main --help`
```
example shell, version 0.0.1
Usage: ./main [options] output_file
  -i, --interactive       Start an interactive shell
  -c, --command line      Execute line as a command
  -s, --stdin             Read commands from standard input
  -a/+a                   Always export variables on assignment
  -b/+b                   Notify asynchronously of background completion
  -C/+C                   Don't overwrite files on redirect with >
  -e/+e                   When any command fails, exit
  -f/+f                   Disable pathname expansion
  -h/+h                   Locate utilities on function definition
  -m/+m                   Print background process status changes before drawing PS1
  -o/+o [option]          If option is non null, set options to option, else show all enabled options, use +o to print in an inputable format
  -u/+u                   Fail when expanding an unset parameter
  -v/+v                   Write shell input to stderr
  -x/+x                   Print every command after expansion before execution
  --help                  Show this help message
  --usage                 Show this help message
Home page : https://example.com/shell

```

## Current limitations

- This library can only handle boolean flags and flags with string values, it could be improved to handle integers for example.
- You can't set a flag several times (this also applies to +/-)
