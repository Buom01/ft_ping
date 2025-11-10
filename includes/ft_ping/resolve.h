#ifndef FT_PING_RESOLVE_H
# define FT_PING_RESOLVE_H

# include "types.h"
# include "ft_ping.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

int ping_resolve(void);

#endif