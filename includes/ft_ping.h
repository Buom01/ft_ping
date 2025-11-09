#ifndef FT_PING_H
# define FT_PING_H

# include "ft_ping/types.h"
# include <stdio.h>
# include <errno.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define BINARY "ft_ping"
# define ICMP_ID 42

typedef struct s_options
{
  bool      verbose;
  bool      help;
  char      *target;
  struct in_addr  addr;
  uint16_t  sequence;
  int       sockfd;
  int       ping;
  int       pong;
} t_options;

typedef struct s_icmp_req
{
  uint8_t   type;
  uint8_t   code;
  uint16_t  checksum;
  uint16_t	id;
  uint16_t	sequence;
} t_icmp_req;

typedef struct s_icmp_res
{
  uint8_t   type;
  uint8_t   code;
  uint16_t  checksum;
  char      data[572];
} t_icmp_res;

void ft_ping(t_options *options);

#endif