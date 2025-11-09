#ifndef FT_PING_ICMP_H
# define FT_PING_ICMP_H

# include "types.h"
# include "ft_ping.h"
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
    
int ping_request(void);
int ping_handle_response(void);

#endif