#include "ft_ping.h"
#include "ft_ping/messages.h"
#include "ft_ping/resolve.h"


int ping_resolve(void)
{
  if (!inet_aton(g_options.target, &(g_options.addr)))
  {
    print_error(g_options.target, "Name or service not known");
    return 1;
  }

  return 0;
}