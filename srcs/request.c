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

static t_icmp_req icmp_request_packet(t_options *options)
{
  t_icmp_req packet = {
    8,
    0,
    0,
    htons(options->id),
    htons(options->sequence++)
  };
  packet.checksum = checksum(&packet, sizeof(packet));

  return packet;
}

// manual used: socket, imcp(7), raw(7), ip(7), bind(2)
int ping_request(t_options *options)
{
  struct sockaddr_in sockaddr = {0};
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr = options->addr;

  options->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (options->sockfd < 0)
  {
    printf("Socket creation failed: %s\n", strerror(errno));
    return 1;
  }

  t_icmp_req packet = icmp_request_packet(options);

  gettimeofday(&options->ping_time, NULL);

  ssize_t sent = sendto(options->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
  if (sent < 0) {
    printf("Send failed: %s\n", strerror(errno));
    return 1;
  }
  options->ping++;

  return 0;
}