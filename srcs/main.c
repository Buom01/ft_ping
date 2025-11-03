#include "ft_ping.h"

static int print_usage(const char *prog)
{
    (void)prog;

    return 0;
}

int main(int argc, char **argv)
{
    t_options options = {false, false, NULL};

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
            options.target = argv[i];
        else if (strcmp(argv[i], "-v") == 0)
            options.verbose = true;
        else if (strcmp(argv[i], "-?") == 0)
            options.help = true;
    }

    if (options.help)
        return print_usage(argv[0]);

    ft_ping(&options);

    return 0;
}
