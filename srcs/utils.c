#include "ft_ping/utils.h"

t_timeval get_time()
{
  t_timeval time;
  gettimeofday(&time, NULL);
  return time;
}

float get_time_diff(t_timeval start, t_timeval end)
{
  unsigned long usec_diff = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
  return usec_diff / 1000.0;  
}