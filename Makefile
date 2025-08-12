NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror
LDFLAGS     = -lreadline
SRCS        =   minishell.c						\
				utils1.c						\
				utils2.c						\
				utils3.c						\
				utils4.c						\
				utils5.c						\
				execution.c						\
				tokenizer.c						\
				lexer.c							\
				lexer_utils.c					\
				parser.c						\
				parser_utils.c					\
				free.c							\
				free2.c							\
				builtin/builtin_cd.c			\
				builtin/builtin_echo.c			\
				builtin/builtin_env.c			\
				builtin/builtin_exit.c			\
				builtin/builtin_export.c		\
				builtin/builtin_pwd.c			\
				builtin/builtin_unset.c			\
				builtin/builtin_executor.c		\
				env_list.c						\
				env_node.c						\
				external_utils.c				\
				char_utils.c					\
				error_handler.c					\
				error_handler2.c				\
				command_executor.c				\
				command_flow.c					\
				command_utils.c					\
				signal.c						\
				signal_helper.c					\
				redirection_and_heredoc_utils.c	\
				pipe_utils.c 					\
				path_finder.c 					\
				redirection_handler.c			\
				expand.c						\
				heredoc.c						\
				builtin/builtin_export_util.c   \
				heredoc_helper.c
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