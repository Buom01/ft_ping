#include "ft_ping.h"
#include "ft_ping/resolve.h"
#include "ft_ping/icmp.h"
#include "ft_ping/messages.h"
#include "ft_ping/utils.h"


t_options g_options;

// inutils2-ping is (sometimes) buggy on the total time elapsed :
// https://github.com/iputils/iputils/issues/193#issuecomment-508861630 and
// https://serverfault.com/questions/999595/what-does-the-time-field-indicate-in-ping-statistics
static void handle_stop()
{
  float loss = ((float)(g_options.ping - g_options.pong) / g_options.ping) * 100.0;

  printf("\n");
  printf("--- %s ping statistics ---\n", g_options.hostname[0] ? g_options.hostname : g_options.target);
  printf(
    "%i packets transmitted, %i received, %0.f%% packet loss, time %.0fms\n",
    g_options.ping, g_options.pong, loss, get_time_diff(g_options.start_time, g_options.response_time)
  );

  if (g_options.pong > 0)
  {
    // mdev is the mean standard deviation (fr: écart type) of the round-trip times
    // Formula : https://fr.wikipedia.org/wiki/%C3%89cart_type#D%C3%A9finition
    float avg = g_options.rtt_sum / g_options.pong;
    float variance = (g_options.rtt_sum_squares / g_options.pong) - (avg * avg);
    float mdev = variance > 0 ? sqrtf(variance) : 0;

    printf(
      "rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
      g_options.rtt_min, avg, g_options.rtt_max, mdev
    );
  }
  exit(1);
}

static void handle_details()
{
  if (g_options.pong > 0)
  {
    float loss = ((float)(g_options.ping - g_options.pong) / g_options.ping) * 100.0;
    float avg = g_options.rtt_sum / g_options.pong;

    // Using "\r" to overwrite the terminal echo of SIGQUIT (Ctrl+\)
    printf(
      "\r%i/%i packets, %.0f%% loss, min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms\n",
      g_options.pong, g_options.ping, loss,
      g_options.rtt_min, avg, g_options.rtt_ewma, g_options.rtt_max
    );
  }

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

      struct timespec ts = {1, 0}, rem;
      while (nanosleep(&ts, &rem) == -1 && errno == EINTR)
        ts = rem;
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
