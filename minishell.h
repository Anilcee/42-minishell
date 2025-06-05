#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <signal.h>

typedef struct s_token
{
	char			*value;
	struct s_token	*next;
}	t_token;

char	**ft_split(const char *str, char separator);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strdup(const char *s);
size_t	ft_strlen(const char *s);
int		external_commands(char **args, char **envp);
t_token *tokenize(char *input);
#endif
