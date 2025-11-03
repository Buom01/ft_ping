#ifndef FT_PING_H
#define FT_PING_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

typedef struct s_options {
  bool verbose;
  bool help;
  char *target;
} t_options;

void ft_ping(const t_options *options);

#endif