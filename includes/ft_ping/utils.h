#ifndef FT_PING_UTILS_H
# define FT_PING_UTILS_H

#include "ft_ping/types.h"

t_timeval   get_time();
float       get_time_diff(t_timeval start, t_timeval end);

#endif