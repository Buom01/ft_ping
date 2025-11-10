#include "ft_ping.h"
#include "ft_ping/messages.h"
#include "ft_ping/resolve.h"

int ping_resolve()
{
  struct hostent *host;
  struct in_addr addr;

  g_options.hostname[0] = '\0';

  // Try to parse as IP address
  if (inet_aton(g_options.target, &addr))
  {
    // Try reverse DNS lookup
    g_options.sockaddr.sin_addr = addr;
    return 0;
  }

  // Try to resolve as hostname
  host = gethostbyname(g_options.target);
  if (!host || !host->h_addr_list[0]) {
    print_error("unknown host");
    return 1;
  }
  memcpy(&g_options.sockaddr.sin_addr, host->h_addr_list[0], sizeof(struct in_addr));
  return 0;
}