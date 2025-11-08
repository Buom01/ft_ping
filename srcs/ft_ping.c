#include "ft_ping.h"
#include "ft_ping/resolve.h"
#include "ft_ping/request.h"
#include "ft_ping/response.h"

static void ft_ping_stop()
{
  /*
--- statistiques ping google.com ---
17 paquets transmis, 17 reçus, 0% packet loss, time 16006ms
rtt min/avg/max/mdev = 10.814/11.565/13.364/0.822 ms
  */
  printf("SIGINT\n");
  exit(1);
}

static void ft_ping_detail()
{
  //"12/12 paquets, 0 % perdus, min/moy/mmpe/max = 10.867/11.476/11.405/13.364 ms"
  printf("SIGQUIT\n");
  signal(SIGQUIT, ft_ping_detail);
}

void ft_ping(t_options *options)
{
  signal(SIGINT, ft_ping_stop);
  signal(SIGQUIT, ft_ping_detail);

  if (ping_resolve(options))
    return ;

  // @TODO: Handle errors
  ping_request(options);
  ping_response(options);
}