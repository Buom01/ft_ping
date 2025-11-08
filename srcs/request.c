#include "ft_ping.h"
#include "ft_ping/request.h"
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

static t_icmp_req icmp_packet(const uint16_t seq)
{
  t_icmp_req packet = {
    8,
    0,
    0,
    htons(ICMP_ID),
    htons(seq)
  };
  packet.checksum = checksum(&packet, sizeof(packet));

  return packet;
}

// manual used: socket, imcp(7), raw(7), ip(7), bind(2)
void ping_request(t_options *options)
{
  struct sockaddr_in sockaddr;
  bzero(&sockaddr, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr = options->addr;

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0)
  {
    printf("Socket creation failed: %s\n", strerror(errno));
    return;
  }
  t_icmp_req packet = icmp_packet(options->sequence++);

  ssize_t sent = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
  if (sent < 0) {
      printf("Send failed: %s\n", strerror(errno));
      return;
  }

  struct timeval tv_timeout;
  tv_timeout.tv_sec = 1;
  tv_timeout.tv_usec = 0;

  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));

  char buff[256];
  bzero(buff, 256);

  struct sockaddr_in recv_addr;
  socklen_t addr_len = sizeof(recv_addr);
  bzero(&recv_addr, sizeof(recv_addr));
  
  ssize_t ret = recvfrom(sockfd, buff, 256, 0, 
                        (struct sockaddr*)&recv_addr, &addr_len);

  printf("Response is %zi\n", ret);
  if (ret <= 0) {
    printf("Recv failed: %s\n", strerror(errno));
    return;
  }
}