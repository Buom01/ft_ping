#include "ft_ping.h"
#include "ft_ping/messages.h"

int print_usage()
{
  printf("Usage: %s [OPTION...] HOST ...\n", BINARY);
  printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
  printf("\n");
  printf(" Options valid for all request types:\n");
  printf("\n");
  printf("  -n, --numeric              do not resolve host addresses\n");
  printf("  -v, --verbose              verbose output\n");
  printf("  -w, --timeout=N            stop after N seconds\n");
  printf("\n");
  printf(" Options valid for --echo requests:\n");
  printf("\n");
  printf("  -?, --help                 give this help list\n");
  printf("\n");
  printf("Mandatory or optional arguments to long options are also mandatory or optional\n");
  printf("for any corresponding short options.\n");

  return 0;
}

int print_usage_tip()
{
  printf("Try '%s --help' for more information.\n", BINARY);
  return 1;
}

int print_error(const char *reason)
{
  printf("%s: %s\n", BINARY, reason);
  return 1;
}

int print_usage_error(const char *reason, const char *arg)
{
  printf("%s: %s '%s'\n", BINARY, reason, arg);
  print_usage_tip();
  return 1;
}

void print_req_result()
{
  const char *format;
  float msec = g_options.rtt;

  if (msec < 0.1)
      format = "%.3f";
  else if (msec < 10)
      format = "%.2f";
  else if (msec < 100)
      format = "%.1f";
  else
      format = "%.0f";

  printf("%d bytes from ", g_options.packet_size);
  printf("%s", inet_ntoa(g_options.sockaddr.sin_addr));

  printf(
    ": icmp_seq=%d ttl=%d time=",
    g_options.response_seq, g_options.ttl
  );
  printf(format, msec);
  printf(" ms\n");
}