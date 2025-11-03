#include "ft_ping.h"

// Adapted from https://datatracker.ietf.org/doc/html/rfc1071#section-4.1
static int16_t checksum(void *buff, size_t len)
{
  // Using 32 bit to handle overflow
  int32_t sum = 0;

  while (len > 1)
  {
    sum += *((uint16_t *)buff++);
    len -= 2;
  }

  if (len > 0)
    sum += *((uint8_t *)buff++);

  // Fold 32-bit sum to 16 bits
  // Adding 16 lower bits to 16 overflowing upper bits
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);

  return (~sum);
}

void request(uint16_t seq)
{
  t_icmp_req packet = {
    8,
    0,
    0,
    42,
    seq
  };
  packet.checksum = checksum(&packet, sizeof(packet));
}