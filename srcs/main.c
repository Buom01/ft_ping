#include "ft_ping.h"

static int print_usage()
{
  printf("\n");
  printf("Usage\n");
  printf("  ft_ping [options] <destination>\n");
  printf("\n");
  printf("Options :\n");
  printf("  <destination>      DNS name or IP address\n");
  printf("  -v                 verbose output\n");
  printf("  -?                 show this help\n");

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
    return print_usage();

  ft_ping(&options);

  return 0;
}
