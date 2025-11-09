#ifndef FT_PING_RESPONSE_H
# define FT_PING_RESPONSE_H

# include "types.h"
# include "ft_ping.h"
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>

int ping_response(void);

#endif