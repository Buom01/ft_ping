#include "ft_ping/utils.h"

t_timeval get_time()
{
  t_timeval time;
  gettimeofday(&time, NULL);
  return time;
}

float get_time_diff(t_timeval start, t_timeval end)
{
  t_timeval diff;
  timersub(&end, &start, &diff);
  return (diff.tv_sec * 1000.0) + (diff.tv_usec / 1000.0);
}