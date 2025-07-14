#ifndef MINISHELL_H
# define MINISHELL_H

# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

typedef enum e_redirect_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}						t_redirect_type;

typedef struct s_redirect
{
	t_redirect_type		type;
	char				*filename;
	struct s_redirect	*next;
}						t_redirect;

typedef struct s_command
{
	char				**args;
	t_redirect			*redirects;
	struct s_command	*next;
}						t_command;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_APPEND,
	T_HEREDOC
}						t_token_type;

typedef struct s_token
{
	char				*value;
	char				quote_type;
	struct s_token		*next;
	t_token_type		t_type;
}						t_token;

typedef struct s_history
{
	char				*line;
	struct s_history	*next;
}						t_history;

typedef struct s_env
{
	char				*key;
	char				*value;
	struct s_env		*next;
}						t_env;

typedef struct s_shell
{
	int					last_exit_code;
	t_env				*env_list;
	char				**envp;
}						t_shell;

typedef struct s_pid_list
{
	pid_t				pid;
	struct s_pid_list	*next;
}						t_pid_list;

int						ft_wifexited(int status);
int						ft_wexitstatus(int status);
int						ft_wifsignaled(int status);
int						ft_wtermsig(int status);

char					**ft_split(const char *str, char separator);
char					*ft_strjoin(char const *s1, char const *s2);
char					*ft_strdup(const char *s);
char					*ft_itoa(int n);
size_t					ft_strlen(const char *s);
size_t					ft_strlcpy(char *dst, const char *src, size_t size);
int						external_commands(t_command *cmd, char **envp);
t_token					*tokenize(char *input, t_env *env_list, t_shell *shell);
void					execute_builtin(t_command *cmds, t_shell *shell);
int						builtin_cd(t_command *cmd, t_env *env_list);
int						builtin_pwd(void);
int						builtin_env(char **env);
int						builtin_echo(t_command *cmd);
void					builtin_history(char *line);
int						builtin_export(t_command *cmd, char ***envp,
							t_env **env_list);
int						is_valid_identifier(const char *str);
char					**copy_env(char **envp);
int						handle_redirections(t_command *cmd);
int						has_pipe(t_command *cmds);
int						execute_piped_commands(t_command *cmds, char **envp);
t_command				*parse_tokens(t_token *tokens);
t_env					*add_env_list(t_env **head, char *input);
char					**add_envp(char **envp, char *input);
t_env					*envp_to_list(char **envp);
int						builtin_unset(t_command *cmd, char ***envp,
							t_env **env_list);
void					unset_from_env_list(t_env **head, const char *key);
char					**unset_from_envp(char **envp, const char *key);
char					*get_env_value(t_env *env_list, const char *key);
char					*ft_strndup(const char *s, size_t n);
int						ft_strncmp(const char *s1, const char *s2, size_t n);
int						is_num(char *str);
int						ft_isalnum(char c);
int						ft_strcmp(const char *s1, const char *s2);
char					*ft_strchr(const char *s, int i);
int						execute_command(t_command *cmds, t_shell *shell);
int						builtin_exit(t_command *cmd);
int						execute_builtin_in_child(t_command *cmd,
							t_shell *shell);
int						is_builtin(const char *cmd);
int						ft_atoi(const char *str);
void					free_env_list(t_env *head);
#endif
