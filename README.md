# C argument parsing library

## How to use it
<ol>
<li>Create a structure containing strings and booleans: make sure that you don't allocate your strings on the stack by doing

```c
char* argument = NULL;
```

and not

```c
char argument[50];
```

This structure will hold values of every flag.
</li>

<li>Create a 0-terminated array of dash_Longopt options, one for each flag you want. A dash_Longopt structure is defined as follows:

```c
typedef struct {
    char opt_name;
    bool allow_flag_unset;
    bool param_optional;
    const char* param_name;
    const char* longopt_name;
    const char* description;
    void* user_pointer;
} dash_Longopt;
```
- `opt_name`: A single char defining the short name of the option. If not set, the option has no short name.
- `allow_flag_unset` Indicates if it's possible to write +X instead of -X to remove the X flag. If not set, +X is forbidden.
- `param_optional` Indicates whether or not the parameter of the option can be omitted. This statement as no effect if `param_name` is unset. If not set, the parameter isn't optional.
- `param_name`: The name of the parameter. If not set, the option takes no parameter and `user_pointer` should be a `bool*` that points to a boolean. If set, `user_pointer` should be a `char*`.
- `longopt_name`: the long name of the option, callable with --NAME. If not set, the option has no short name.
- `description`: the description of the option for dash_print_usage, every `$` character will be replaced by the content of `param_name`
- `user_pointer`: A pointer to the data to register, either a `bool*` or a `char*`, MUST be set

Example:
```c
char* argument = NULL;
bool flag;

dash_Longopt options[] = {
    {.user_pointer = &argument, .longopt_name = "my_flag", .opt_name = 'f', .param_optional = True, .param_name = "flag_argument", .description = "Set value of my flag to $"},
    {0}
};
```

> [!NOTE]
> Every entry with no `opt_name` and no `longopt_name` will be considered like the 0-element at the end of the array.

</li>

<li>Then for every mandatory argument, create a new NULL-terminated string array, for example:

```c
const char* required_arguments[] = {
    "output_stream",
    NULL
};
```

</li>

<li>Finally you can call arg_parser():

```c
if (!dash_arg_parser(&argc, argv, options))
{
    dash_print_usage(argv[0], "help message header", "help message footer", required_argumnets, options);
    dash_free(options);
    exit(1);
}

// use argument, flag and argv

dash_free(options);
```

</li>
</ol>

## What is supported

- Short / long flags with or without an argument
- Long flags with an argument provided with the '=' sign (e.g. `--output=file`)
- Unsetting short flags with a plus sign instead of a hyphen (e.g. `+s` means explicitly disable flag `s`)
- Multiple short options with a single hyphen (e.g. `-abc` means `-a -b -c`)
- Short option and argument without a space in between for options requiring arguments (e.g. `-cecho` means `-c echo`)
- Double hyphen marks the end of flags, any argument beginning with an hyphen after that will not be considered as a flag
- A single hyphen will not be considered as a flag, meaning you can use it freely (for example as an alias to /dev/stdin)

A complete example is available in `example.c`, you can build it with `make example`, in this example, you can call
`./build/example -i --command="echo hi" v1 -s v2 -f +o "autocd noglob"` for example, you should get this output:
```
interactive                         = (bool) True
command                             = (string) echo hi
stdin                               = (bool) True
a                                   = (bool) False
b                                   = (bool) False
C                                   = (bool) False
e                                   = (bool) False
f                                   = (bool) True
h                                   = (bool) False
m                                   = (bool) False
o                                   = (string) +autocd noglob
u                                   = (bool) False
v                                   = (bool) False
x                                   = (bool) False
help                                = (bool) False
usage                               = (bool) False
Remaining arguments: ./build/example v1 v2
```
Other example:
`./build/example --help`
```
example shell, version 0.0.1
Usage: ./build/example [options] output_file
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
- You can't set a non-boolean flag several times
