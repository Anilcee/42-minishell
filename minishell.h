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

typedef struct s_history {
    char *line;
    struct s_history *next;
} t_history;

typedef struct s_env {
    char *key;
    char *value;
    struct s_env *next;
} t_env;

char	**ft_split(const char *str, char separator);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strdup(const char *s);
size_t	ft_strlen(const char *s);
int external_commands(t_command *cmd, char **envp);
t_token *tokenize(char *input);
int builtin_cd(t_command *cmd);
void builtin_pwd(void);
void builtin_env(char **env);
void builtin_echo(t_command *cmd, t_env *env_list);
void builtin_history(char *line);
char **builtin_export(t_command *cmd, char** envp, t_env **env_list);
char **copy_env(char **envp);
int handle_redirections(t_command *cmd);
int has_pipe(t_command *cmds);
void execute_piped_commands(t_command *cmds);
t_command *parse_tokens(t_token *tokens);
t_env *add_env_list(t_env **head, char *input);
char **add_envp(char **envp, char *input);
t_env *envp_to_list(char **envp);
char **builtin_unset(t_command *cmd, char **envp, t_env **env_list);
void unset_from_env_list(t_env **head, const char *key);
char **unset_from_envp(char **envp, const char *key);
char	*ft_strndup(const char *s, size_t n);
int	ft_strncmp(const char *s1, const char *s2, size_t n);

#endif
