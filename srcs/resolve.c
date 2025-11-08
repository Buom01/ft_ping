#include "ft_ping.h"
#include "ft_ping/messages.h"
#include "ft_ping/resolve.h"


int ping_resolve(t_options *options)
{
  if (!inet_aton(options->target, &(options->addr)))
  {
    print_error(options->target, "Name or service not known");
    return 1;
  }

  return 0;
}