#include "ft_ping.h"
#include "ft_ping/types.h"
#include "ft_ping/utils.h"
#include "ft_ping/icmp.h"
#include "ft_ping/messages.h"

// Adapted from https://datatracker.ietf.org/doc/html/rfc1071#section-4.1
static uint16_t checksum(void *buff, size_t len)
{
  // Using 32 bit to handle overflow
  uint32_t sum = 0;

  while (len > 1)
  {
    sum += *((uint16_t *)buff);
    len -= 2;
    buff += 2;
  }

  // Adding last byte
  if (len > 0)
    sum += *((uint8_t *)buff++);

  // Fold 32-bit sum to 16 bits
  // Adding 16 lower bits to 16 overflowing upper bits
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);

  return (~sum);
}

static t_icmp_req get_ping_request()
{
  t_icmp_req packet = {
    8,
    0,
    0,
    htons(g_options.id),
    htons(g_options.sequence++)
  };
  packet.checksum = checksum(&packet, sizeof(packet));

  return packet;
}

// manual used: socket, imcp(7), raw(7), ip(7), bind(2)
int ping_request()
{
  t_icmp_req packet = get_ping_request();

  g_options.send_time = get_time();

  ssize_t sent = sendto(g_options.sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&g_options.sockaddr, sizeof(g_options.sockaddr));
  if (sent < 0) {
    printf("Send failed: %s\n", strerror(errno));
    return 1;
  }
  g_options.ping++;

  return 0;
}

int ping_handle_response()
{
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

  g_options.response_time = get_time();
  g_options.rtt = get_time_diff(g_options.send_time, g_options.response_time);
  g_options.pong++;

  return 0;
}