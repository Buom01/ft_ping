#include "ft_ping.h"
#include "ft_ping/messages.h"
#include "ft_ping/resolve.h"


static int resolve_reverse_ip(struct in_addr *addr)
{
  struct hostent *host;

  host = gethostbyaddr((const void *)addr, sizeof(*addr), AF_INET);
  if (host && host->h_name)
  {
    strncpy(g_options.hostname, host->h_name, sizeof(g_options.hostname) - 1);
    g_options.hostname[sizeof(g_options.hostname) - 1] = '\0';
    return 0;
  }
  return 1;
}

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
    resolve_reverse_ip(&addr);
    return 0;
  }

  // Try to resolve as hostname
  host = gethostbyname(g_options.target);
  if (!host || !host->h_addr_list[0]) {
    print_error(g_options.target, "Name or service not known");
    return 1;
  }
  memcpy(&g_options.sockaddr.sin_addr, host->h_addr_list[0], sizeof(struct in_addr));
  resolve_reverse_ip(&g_options.sockaddr.sin_addr);
  return 0;
}