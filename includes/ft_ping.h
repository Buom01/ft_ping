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
# include <netinet/ip_icmp.h>
# include <math.h>
# include <time.h>

# define BINARY "ft_ping"
# define ICMP_ID 42
# define RES_MAX_LEN 60 + 572

typedef struct s_options
{
  bool            verbose;
  bool            help;
  char            *target;
  bool            no_rev_dns;
  int             timeout;
  char            hostname[257];
  struct sockaddr_in sockaddr;
  uint16_t        id;
  uint16_t        sequence;
  t_timeval       start_time;
  int             sockfd;
  int             ping;
  int             pong;
  t_timeval       send_time;
  t_timeval       response_time;
  int             ttl;
  int             packet_size;
  float           rtt;
  float           rtt_min;
  float           rtt_max;
  float           rtt_sum;
  float           rtt_sum_squares;
  float           rtt_ewma;
} t_options;

extern t_options g_options;

typedef struct s_icmp_req
{
  uint8_t   type;
  uint8_t   code;
  uint16_t  checksum;
  uint16_t	id;
  uint16_t	sequence;
  uint8_t  data[56];
} t_icmp_req;

typedef struct s_icmp_res
{
  struct iphdr ip_hdr;
  uint8_t   type;
  uint8_t   code;
  uint16_t  checksum;
  uint16_t  identifier;
  uint16_t  sequence;
  char      data[568];
} t_icmp_res;

#endif