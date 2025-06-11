NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror
LDFLAGS     = -lreadline
SRCS        = minishell.c utils.c tokenizer.c
OBJS        = $(SRCS:.c=.o)
HEADER      = minishell.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re