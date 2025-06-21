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
#include <fcntl.h>


typedef struct s_command {
    char **args;             
    char *infile;             
    char *outfile;           
    int append;               
    int heredoc;              
    struct s_command *next;   
} t_command;

typedef enum e_token_type {
	T_WORD,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_APPEND,
	T_HEREDOC
}	t_token_type;

typedef struct s_token
{
	char			*value;
	struct s_token	*next;
	t_token_type t_type;
}	t_token;


#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

char	*get_next_line(int fd);
char	**ft_split(const char *str, char separator);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strdup(const char *s);
size_t	ft_strlen(const char *s);
int		external_commands(char **args, char **envp);
t_token *tokenize(char *input);
int builtin_cd(t_command *cmd);
void builtin_pwd(void);
void builtin_env(char **env);
void builtin_echo(t_command *cmd);
void builtin_history(char *line);
char **builtin_export(char *arg, char **env);
char **copy_env(char **envp);
int handle_redirections(char **args);
int *count_args_per_command(char **args, int command_count);
int count_commands(char **args);
int has_pipe(char **args);
void execute_piped_commands(char ***commands) ;
char ***split_by_pipe(char **args);
t_command *parse_tokens(t_token *tokens);


#endif
