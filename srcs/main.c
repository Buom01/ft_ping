#include "ft_ping.h"
#include "ft_ping/messages.h"

t_options g_options;

int main(int argc, char **argv)
{
  bzero(&g_options, sizeof(g_options));

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] != '-')
      g_options.target = argv[i];
    else if (strcmp(argv[i], "-v") == 0)
      g_options.verbose = true;
    else if (strcmp(argv[i], "-?") == 0)
      g_options.help = true;
  }

  if (g_options.help)
    return print_usage();

  if (!g_options.target)
    return print_usage_error("Destination address required");

  ft_ping();

  return 0;
}
