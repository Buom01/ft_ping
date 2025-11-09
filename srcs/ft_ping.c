#include "ft_ping.h"
#include "ft_ping/resolve.h"
#include "ft_ping/request.h"
#include "ft_ping/response.h"

static void ft_ping_stop()
{
  /*
--- 127.0.0.1 ping statistics ---
12 packets transmitted, 12 received, 0% packet loss, time 11242ms
rtt min/avg/max/mdev = 0.009/0.022/0.057/0.014 ms
  */
  float loss = ((float)(g_options.ping - g_options.pong) / g_options.ping) * 100.0;

  printf("\n");
  printf("--- %s ping statistics ---\n", g_options.target);
  printf("%i packets transmitted, %i received, %f%% packet loss, time ___TODO___ ms\n", g_options.ping, g_options.pong, loss);
  exit(1);
}

static void ft_ping_detail()
{
  //"12/12 paquets, 0 % perdus, min/moy/mmpe/max = 10.867/11.476/11.405/13.364 ms"
  printf("SIGQUIT\n");
  signal(SIGQUIT, ft_ping_detail);
}

void ft_ping(void)
{
  signal(SIGINT, ft_ping_stop);
  signal(SIGQUIT, ft_ping_detail);

  if (ping_resolve() != 0)
    return ;

  printf("PING %s (%s) %i(%i) bytes of data.\n", g_options.target, inet_ntoa(g_options.addr), 56, 84);
  g_options.id = getpid();

  int res = 0;
  while (res == 0)
  {
    ping_request();
    while ((res = ping_response()) == 1)
      ;
    if (res == 0)
      sleep(1);
  }
}