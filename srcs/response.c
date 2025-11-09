#include "ft_ping.h"
#include "ft_ping/response.h"
#include "ft_ping/messages.h"

static void print_result(t_options *options)
{
  const char *format;
  float msec = options->rtt;

  if (msec < 0.1)
      format = "%.3f";
  else if (msec < 1)
      format = "%.2f";
  else if (msec < 10)
      format = "%.1f";
  else
      format = "%.0f";

  printf(
    "%d bytes from %s: icmp_seq=%d ttl=64 time=",
    64,  // FIXME: Hardcoded size
    inet_ntoa(options->addr),
    options->sequence
  );
  printf(format, msec);
  printf(" ms\n");
}

// manual used: socket, imcp(7), raw(7), ip(7), bind(2)
int ping_response()
{
  struct timeval tv_timeout;
  tv_timeout.tv_sec = 1;
  tv_timeout.tv_usec = 0;

  setsockopt(g_options.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));

  t_icmp_res res = {0};

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(recv_addr);
  bzero(&recv_addr, sizeof(recv_addr));
  
  ssize_t ret = recvfrom(g_options.sockfd, &res, sizeof(res), 0, (struct sockaddr*)&recv_addr, &addr_len);

  if (ret < 0)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      printf("Request timed out.\n");
      return -1;
    }
    perror(BINARY);
    return -1;
  }

  if (res.type == 8)
    return 1; // Ignore Echo Requests
  else if (res.type != 0)
  {
    printf("Unexpected ICMP packet received.\n");
    return -1;
  }

  struct timeval recv_time;
  gettimeofday(&recv_time, NULL);

  unsigned long usec_diff = (recv_time.tv_sec - g_options.send_time.tv_sec) * 1000000 + (recv_time.tv_usec - g_options.send_time.tv_usec);
  g_options.rtt = usec_diff / 1000.0;

  g_options.pong++;
  print_result(&g_options);
  return 0;
}