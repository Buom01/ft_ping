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

void print_req_result()
{
  const char *format;
  float msec = g_options.rtt;

  if (msec < 0.1)
      format = "%.3f";
  else if (msec < 1)
      format = "%.2f";
  else if (msec < 10)
      format = "%.1f";
  else
      format = "%.0f";

  printf(
    "%d bytes from %s: icmp_seq=%d ttl=64 time=",
    64,  // FIXME: Hardcoded size
    inet_ntoa(g_options.addr),
    g_options.sequence
  );
  printf(format, msec);
  printf(" ms\n");
}