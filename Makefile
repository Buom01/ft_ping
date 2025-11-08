CC 			:=	clang
NAME		:=	ft_ping
RM			:=	rm -f

SRCS		:=	srcs/main.c \
			srcs/messages.c \
			srcs/ft_ping.c \
			srcs/resolve.c \
			srcs/request.c \
			srcs/response.c

OBJS		:=	$(SRCS:.c=.o)
CFLAGS		?=	-Wall -Werror -Wextra -g3
INCLUDES	:=	-I includes

# Colors
RED			:= $(shell printf "\033[0;91m")
GREEN		:= $(shell printf "\033[0;92m")
BLUE		:= $(shell printf "\033[0;94m")
MAGENTA		:= $(shell printf "\033[0;95m")
RESET		:= $(shell printf "\033[0;0m")
PREFIX		:= $(MAGENTA)$(NAME)$(RESET) => 

$(NAME): $(OBJS)
	@echo "$(PREFIX)$(GREEN)Building executable $(RESET)$(NAME)"
	@$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	@echo "$(PREFIX)$(GREEN)Compiling file $(RESET)$< $(BLUE)to $(RESET)$@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

fclean:
	@echo "$(PREFIX)$(RED)Cleaning all files$(RESET)"
	$(RM) $(OBJS)
	$(RM) $(NAME)

clean:
	@echo "$(PREFIX)$(RED)Cleaning object files$(RESET)"
	$(RM) $(OBJS)

re: fclean
	$(MAKE) all
