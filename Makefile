NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror
LDFLAGS     = -lreadline
SRCS        =   minishell.c				\
				utils1.c				\
				utils2.c				\
				utils3.c				\
				utils4.c				\
				utils5.c				\
				redirection.c			\
				builtin.c				\
				tokenizer.c				\
				parser.c				\
				parser_utils.c			\
				free.c					\
				free2.c					\
				builtin_cd.c			\
				builtin_export.c		\
				builtin_echo.c			\
				builtin_history.c		\
				builtin_unset.c			\
				builtin_env.c			\
				builtin_exit.c			\
				env_list.c				\
				env_node.c				\
				external_utils_1.c		\
				external_utils_2.c		\
				expand.c				
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