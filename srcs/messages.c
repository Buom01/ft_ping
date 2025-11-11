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
  fprintf(stderr, "Try '%s --help' for more information.\n", BINARY);
  return 1;
}

int print_error(const char *reason)
{
  fprintf(stderr, "%s: %s\n", BINARY, reason);
  return 1;
}

int print_usage_error(const char *reason, const char *arg)
{
  fprintf(stderr, "%s: %s '%s'\n", BINARY, reason, arg);
  print_usage_tip();
  return 1;
}

void print_req_result()
{
  printf("%d bytes from %s", g_options.packet_size, inet_ntoa(g_options.sockaddr.sin_addr));
  printf(": icmp_seq=%d ttl=%d time=", g_options.response_seq, g_options.ttl);
  printf("%0.3f ms\n", g_options.rtt);
}