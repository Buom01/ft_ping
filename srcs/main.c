#include "ft_ping.h"
#include "ft_ping/messages.h"

int main(int argc, char **argv)
{
  t_options options = {0};

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
    return print_usage();

  if (!options.target)
    return print_usage_error("Destination address required");

  ft_ping(&options);

  return 0;
}
