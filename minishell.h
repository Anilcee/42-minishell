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

typedef enum e_error_code
{
	CMD_SUCCESS = 0,
	CMD_NOT_FOUND = -1,
	FILE_NOT_FOUND = -2,
	IS_DIRECTORY = -3,
	PERMISSION_DENIED = -4,
	PATH_NOT_SET = -5
}						t_error_code;

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

typedef struct s_pipe_data
{
	int					prev_fd;
	int					fd[2];
}						t_pipe_data;

typedef struct s_execution_context
{
	t_command			*current;
	t_shell				*shell;
	t_command			*all_cmds;
	t_token				*all_tokens;
	t_pipe_data			pipe_data;
	t_pid_list			**pid_list;
}						t_execution_context;

typedef struct s_redirection_context
{
	t_command			*cmds;
	int					*saved_stdout;
	int					*saved_stdin;
	t_shell				*shell;
}						t_redirection_context;

typedef enum e_exit_result
{
	EXIT_OK = 0,
	EXIT_TOO_MANY_ARGS = 1,
	EXIT_NOT_NUMERIC = 2,
	EXIT_NO_ARG = 3,
	EXIT_ARG_VALUE = 4 // Gerçek exit kodu pointer ile dönecek
}						t_exit_result;

int						ft_wifexited(int status);
int						ft_wexitstatus(int status);
int						ft_wifsignaled(int status);
int						ft_wtermsig(int status);

char					**ft_split(const char *str, char separator);
char					*ft_strjoin(char const *s1, char const *s2);
char					*ft_strdup(const char *s);
char					*ft_itoa(int n);
char					*build_path(const char *dir, const char *filename);
char					*append_string(char *original, const char *to_append);
size_t					ft_strlen(const char *s);
size_t					ft_strlcpy(char *dst, const char *src, size_t size);
int						external_commands(t_command *cmd, t_shell *shell);
t_token					*tokenize(char *input, t_shell *shell);
int						execute_builtin(t_command *cmds, t_shell *shell);
int						builtin_cd(t_command *cmd, t_env **env_list);
int						builtin_pwd(void);
int						builtin_env(char **env);
int						builtin_echo(t_command *cmd);
void					builtin_history(char *line);
void					cleanup_history(void);
int						builtin_export(t_command *cmd, char ***envp,
							t_env **env_list);
int						is_valid_identifier(const char *str);
char					**copy_env(char **envp);
int						handle_redirections(t_command *cmd);
int						has_pipe(t_command *cmds);
int						execute_piped_commands(t_command *cmds, t_token *tokens,
							t_shell *shell);
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
int						execute_command(t_command *cmds, t_token *tokens,
							t_shell *shell);
int						builtin_exit(t_command *cmd, int *real_exit_code);
void					execute_builtin_in_child(t_command *cmd, t_shell *shell,
							t_command *all_cmds, t_token *all_tokens);
int						is_builtin(const char *cmd);
void					cleanup_and_exit(t_shell *shell, t_command *all_cmds,
							t_token *all_tokens, int exit_code);
int						ft_atoi(const char *str);
void					free_env_list(t_env *head);
void					free_tokens(t_token *head);
void					free_commands(t_command *head);
void					free_redirects(t_redirect *head);
void					free_envp_array(char **envp);
void					free_shell(t_shell *shell);
void					free_history_list(t_history *head);
int						check_absolute_path(char *command_name);
char					*find_in_path(char *command_name, t_shell *shell);
int						execute_child_process(char *program_path,
							t_command *cmd, char **envp);
void					free_paths_array(char **paths);
int						get_exit_status(int status);
void					handle_external_error2(t_command *cmds, int result,
							t_shell *shell);
void					print_error_message(char *cmd_name, char *error_msg,
							int exit_code, t_shell *shell);

#endif