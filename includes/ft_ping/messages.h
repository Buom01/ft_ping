#ifndef FT_PING_MESSAGES_H
# define FT_PING_MESSAGES_H

# include <stdio.h>
# include <arpa/inet.h>

int     print_usage();
int     print_error(const char *what, const char *reason);
int     print_usage_error(const char *reason);
void    print_req_result();

#endif