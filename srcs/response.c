#include "ft_ping.h"
#include "ft_ping/response.h"
#include "ft_ping/messages.h"


// manual used: socket, imcp(7), raw(7), ip(7), bind(2)
int ping_response(t_options *options)
{
  struct timeval tv_timeout;
  tv_timeout.tv_sec = 1;
  tv_timeout.tv_usec = 0;

  setsockopt(options->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));

  t_icmp_res packet = {0};

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(recv_addr);
  bzero(&recv_addr, sizeof(recv_addr));
  
  ssize_t ret = recvfrom(options->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&recv_addr, &addr_len);

  printf("Response is %zi\n", ret);
  if (ret <= 0) {
    printf("Recv failed: %s\n", strerror(errno));
    return 1;
  }

  return 0;
}