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
#include <netinet/ip_icmp.h>

# define BINARY "ft_ping"
# define ICMP_ID 42
# define RES_MAX_LEN 60 + 572

typedef struct s_options
{
  bool            verbose;
  bool            help;
  char            *target;
  struct in_addr  addr;
  uint16_t        id;
  uint16_t        sequence;
  int             sockfd;
  int             ping;
  int             pong;
  struct timeval  send_time;
  float           rtt;
} t_options;

extern t_options g_options;

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
  char      ip_hdr[20];  // IP header size without options
  uint8_t   type;
  uint8_t   code;
  uint16_t  checksum;
  uint16_t  identifier;
  uint16_t  sequence;
  char      data[568];
} t_icmp_res;

void ft_ping(void);

#endif