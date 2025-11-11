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

  printf("--- %s ping statistics ---\n", g_options.hostname[0] ? g_options.hostname : g_options.target);
  printf(
    "%i packets transmitted, %i packets received, %.0f%% packet loss\n",
    g_options.ping, g_options.pong, loss
  );

  if (g_options.pong > 0)
  {
    // mdev is the mean standard deviation (fr: écart type) of the round-trip times
    // Formula : https://fr.wikipedia.org/wiki/%C3%89cart_type#D%C3%A9finition
    float avg = g_options.rtt_sum / g_options.pong;
    float variance = (g_options.rtt_sum_squares / g_options.pong) - (avg * avg);
    float mdev = variance > 0 ? sqrtf(variance) : 0;

    printf(
      "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
      g_options.rtt_min, avg, g_options.rtt_max, mdev
    );
  }
  exit(1);
}

static void use_timeout(int timeout)
{
  t_timeval tv_timeout;
  tv_timeout.tv_sec = timeout / 1000;
  tv_timeout.tv_usec = (timeout % 1000) * 1000;

  setsockopt(g_options.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));
}

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

  if (ping_resolve() != 0)
    return ;

  g_options.id = getpid();

  printf(
    "PING %s (%s): %lu data bytes",
    g_options.target, inet_ntoa(g_options.sockaddr.sin_addr),
    sizeof(((t_icmp_req *)0)->data)
  );
  if (g_options.verbose)
    printf(", id 0x%04x = %d", g_options.id, g_options.id);
  printf("\n");

  open_socket();

  if (g_options.timeout == 0)
    g_options.timeout = 1000;

  use_timeout(g_options.timeout);

  int req = 0, res = 0;
  while (req == 0 && (res == 0 || res == 2 || res == 3 || res == 4))
  {

    if ((req = ping_request()) == 0)
    {
      while ((res = ping_handle_response()) == 1)
        ;
    }
    if (res == 0)
      print_req_result();
    if (res != 4)
    {
      struct timespec ts = {1, 0}, rem;
      while (nanosleep(&ts, &rem) == -1 && errno == EINTR)
        ts = rem;
    }
  }

  close(g_options.sockfd);
}

static char*  get_next_arg(int argc, char **argv, int *i)
{
  if (*i + 1 >= argc)
    return NULL;
  (*i)++;
  return argv[*i];
}

int main(int argc, char **argv)
{
  bzero(&g_options, sizeof(g_options));

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] != '-')
      g_options.target = argv[i];
    else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
      g_options.verbose = true;
    else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
      g_options.help = true;
    else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--numeric") == 0)
      g_options.no_rev_dns = true;
    else if (strcmp(argv[i], "-W") == 0)
    {
      char *arg = get_next_arg(argc, argv, &i);
      if (!arg)
        return print_usage_error("option requires an argument --", "W");
      g_options.timeout = atoi(arg) * 1000;
    }
    else if (strncmp(argv[i], "--timeout=", 10) == 0)
      g_options.timeout = atoi(argv[i] + 10) * 1000;
    else
    {
      if (argv[i][1] == '-')
        return print_usage_error("unrecognized option", argv[i]);
      else
        return print_usage_error("invalid option --", argv[i] + 1);
    }
  }

  if (g_options.help)
    return print_usage();

  if (!g_options.target)
  {
    print_error("missing host operand");
    return print_usage_tip();
  }

  ft_ping();

  return 0;
}
