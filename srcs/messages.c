#include "ft_ping.h"
#include "ft_ping/messages.h"

int print_usage()
{
  printf("\n");
  printf("Usage\n");
  printf("  %s [options] <destination>\n", BINARY);
  printf("\n");
  printf("Options :\n");
  printf("  <destination>      DNS name or IP address\n");
  printf("  -v                 verbose output\n");
  printf("  -?                 show this help\n");

  return 0;
}

int print_error(const char *what, const char *reason)
{
  printf("%s: %s: %s\n", BINARY, what, reason);

  return 0;
}

int print_usage_error(const char *reason)
{
  print_error("usage error", reason);

  return 0;
}