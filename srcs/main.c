#include "ft_ping.h"
#include "ft_ping/resolve.h"
#include "ft_ping/icmp.h"
#include "ft_ping/messages.h"
#include "ft_ping/utils.h"


t_options g_options;

static void handle_stop()
{
  /*
--- 127.0.0.1 ping statistics ---
12 packets transmitted, 12 received, 0% packet loss, time 11242ms
rtt min/avg/max/mdev = 0.009/0.022/0.057/0.014 ms
  */
  float loss = ((float)(g_options.ping - g_options.pong) / g_options.ping) * 100.0;

  // inutils2-ping is buggy on the total time elapsed
  // https://github.com/iputils/iputils/issues/193#issuecomment-508861630
  // Also : https://serverfault.com/questions/999595/what-does-the-time-field-indicate-in-ping-statistics
  printf("\n");
  printf("--- %s ping statistics ---\n", g_options.hostname[0] ? g_options.hostname : g_options.target);
  printf("%i packets transmitted, %i received, %0.f%% packet loss, time %.0fms\n", g_options.ping, g_options.pong, loss, get_time_diff(g_options.start_time, g_options.response_time));
  exit(1);
}

static void handle_details()
{
  //"12/12 paquets, 0 % perdus, min/moy/mmpe/max = 10.867/11.476/11.405/13.364 ms"
  printf("SIGQUIT\n");
  signal(SIGQUIT, handle_details);
}

/*
static void use_timeout(int timeout)
{
  t_timeval tv_timeout;
  tv_timeout.tv_sec = timeout;
  tv_timeout.tv_usec = 0;

  setsockopt(g_options.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));
}
*/

static void open_socket()
{
  g_options.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (g_options.sockfd < 0)
  {
    perror("socket");
    exit(1);
  }
}

static void ft_ping()
{
  signal(SIGINT, handle_stop);
  signal(SIGQUIT, handle_details);

  if (ping_resolve() != 0)
    return ;

  printf("PING %s (%s) %i(%i) bytes of data.\n", g_options.target, inet_ntoa(g_options.sockaddr.sin_addr), 56, 84);
  g_options.id = getpid();
  g_options.start_time = get_time();

  open_socket();

  int req = 0, res = 0;
  while (req == 0 && res == 0)
  {
    if ((req = ping_request()) == 0)
    {
      while ((res = ping_handle_response()) == 1)
        ;
    }
    if (res == 0)
    {
      print_req_result();
      sleep(1);
    }
  }

  close(g_options.sockfd);
}

int main(int argc, char **argv)
{
  bzero(&g_options, sizeof(g_options));

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] != '-')
      g_options.target = argv[i];
    else if (strcmp(argv[i], "-v") == 0)
      g_options.verbose = true;
    else if (strcmp(argv[i], "-?") == 0)
      g_options.help = true;
  }

  if (g_options.help)
    return print_usage();

  if (!g_options.target)
    return print_usage_error("Destination address required");

  ft_ping();

  return 0;
}
