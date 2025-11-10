#include "ft_ping.h"
#include "ft_ping/types.h"
#include "ft_ping/utils.h"
#include "ft_ping/icmp.h"
#include "ft_ping/messages.h"

// Get ICMP type description for verbose output
static const char* get_icmp_type_name(uint8_t type, uint8_t code)
{
  switch (type)
  {
    case ICMP_ECHOREPLY:
      return "Echo Reply";
    case ICMP_DEST_UNREACH:
      switch (code)
      {
        case ICMP_NET_UNREACH: return "Destination Net Unreachable";
        case ICMP_HOST_UNREACH: return "Destination Host Unreachable";
        case ICMP_PROT_UNREACH: return "Destination Protocol Unreachable";
        case ICMP_PORT_UNREACH: return "Destination Port Unreachable";
        case ICMP_FRAG_NEEDED: return "Frag needed and DF set";
        case ICMP_SR_FAILED: return "Source Route Failed";
        default: return "Dest Unreachable";
      }
    case ICMP_SOURCE_QUENCH:
      return "Source Quench";
    case ICMP_REDIRECT:
      return "Redirect";
    case ICMP_ECHO:
      return "Echo Request";
    case ICMP_TIME_EXCEEDED:
      if (code == ICMP_EXC_TTL)
        return "Time to live exceeded";
      else if (code == ICMP_EXC_FRAGTIME)
        return "Frag reassembly time exceeded";
      return "Time Exceeded";
    case ICMP_PARAMETERPROB:
      return "Parameter Problem";
    case ICMP_TIMESTAMP:
      return "Timestamp Request";
    case ICMP_TIMESTAMPREPLY:
      return "Timestamp Reply";
    case ICMP_INFO_REQUEST:
      return "Information Request";
    case ICMP_INFO_REPLY:
      return "Information Reply";
    case ICMP_ADDRESS:
      return "Address Mask Request";
    case ICMP_ADDRESSREPLY:
      return "Address Mask Reply";
    default:
      return "Unknown ICMP";
  }
}

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
    htons(g_options.sequence),
    {0}
  };
  packet.checksum = checksum(&packet, sizeof(packet));

  g_options.sequence++;

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

static void update_rtt_stats()
{
  float rtt = g_options.rtt = get_time_diff(g_options.send_time, g_options.response_time);

  if (g_options.pong == 0)
  {
    g_options.rtt_min = rtt;
    g_options.rtt_max = rtt;
    g_options.rtt_sum = rtt;
    g_options.rtt_sum_squares = rtt * rtt;
    g_options.rtt_ewma = rtt;
  }
  else
  {
    if (rtt < g_options.rtt_min)
      g_options.rtt_min = rtt;
    else if (rtt > g_options.rtt_max)
      g_options.rtt_max = rtt;
    g_options.rtt_sum += rtt;
    g_options.rtt_sum_squares += rtt * rtt;  // For standard deviation calculation
    g_options.rtt_ewma = (rtt * EWMA_ALPHA) + (g_options.rtt_ewma * (1 - EWMA_ALPHA));  // https://corporatefinanceinstitute.com/resources/career-map/sell-side/capital-markets/exponentially-weighted-moving-average-ewma/
  }
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
    if (errno == EINTR)
      return 1; // Interrupted by signal, try again
    if (errno == EWOULDBLOCK || errno == EAGAIN)
      return 4;  // Timeout
    perror(BINARY);
    return 2;
  }

   // Ignore echo requests from ourselves
  if (res.type == 8 && ntohs(res.identifier) == g_options.id)
    return 1;
    
  if (res.type != 0) // Not an echo reply
  {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &recv_addr.sin_addr, addr_str, INET_ADDRSTRLEN);
    
    printf(
      "%ld bytes from %s: %s\n", 
      ret - sizeof(res.ip_hdr),
      addr_str,
      get_icmp_type_name(res.type, res.code)
    );

    if (g_options.verbose)
    {
      // the original IP header is in payload ICMP error messages
      struct iphdr *orig_ip_hdr = (struct iphdr *)res.data;

      printf("IP Hdr Dump:\n ");
      for(int i = 0; i < 20; i++)
      {
        printf("%02x", *((uint8_t *)orig_ip_hdr + i));
        if (i % 2 == 1)
          printf(" ");
      }
      printf("\n");
    }

    return 3;  // Don't print normal response
  }

  g_options.packet_size = ret - sizeof(res.ip_hdr);
  g_options.response_seq = ntohs(res.sequence);
  g_options.ttl = res.ip_hdr.ttl;
  g_options.timeout = g_options.ttl * 2;
  g_options.response_time = get_time();
  update_rtt_stats();
  g_options.pong++;

  return 0;
}