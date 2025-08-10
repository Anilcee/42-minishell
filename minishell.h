/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:46 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:31:38 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}								t_error_code;

typedef enum e_redirect_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}								t_redirect_type;

typedef struct s_redirect
{
	t_redirect_type				type;
	char						*filename;
	int							processed_fd;
	struct s_redirect			*next;
}								t_redirect;

typedef struct s_command
{
	char						**args;
	t_redirect					*redirects;
	struct s_command			*next;
}								t_command;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_APPEND,
	T_HEREDOC
}								t_token_type;

typedef struct s_token
{
	char						*value;
	char						quote_type;
	struct s_token				*next;
	t_token_type				t_type;
}								t_token;

typedef struct s_history
{
	char						*line;
	struct s_history			*next;
}								t_history;

typedef struct s_env
{
	char						*key;
	char						*value;
	struct s_env				*next;
}								t_env;

typedef struct s_shell
{
	int							last_exit_code;
	t_env						*env_list;
	char						**envp;
}								t_shell;

typedef struct s_pid_list
{
	pid_t						pid;
	struct s_pid_list			*next;
}								t_pid_list;

typedef struct s_pipe_data
{
	int							prev_fd;
	int							fd[2];
}								t_pipe_data;

typedef struct s_execution_context
{
	t_command					*current;
	t_shell						*shell;
	t_command					*all_cmds;
	t_token						*all_tokens;
	t_pipe_data					pipe_data;
	t_pid_list					**pid_list;
}								t_exec_context;

typedef struct s_redirection_context
{
	t_command					*cmds;
	int							*saved_stdout;
	int							*saved_stdin;
	t_shell						*shell;
}								t_redir_context;

typedef enum e_exit_result
{
	EXIT_OK,
	EXIT_TOO_MANY_ARGS,
	EXIT_NOT_NUMERIC,
	EXIT_NO_ARG,
	EXIT_ARG_VALUE
}								t_exit_result;

extern volatile sig_atomic_t	g_signal_received;

char							**ft_split(const char *str, char separator);
char							*ft_strjoin(char const *s1, char const *s2);
char							*ft_strdup(const char *s);
char							*ft_itoa(int n);
char							*build_path(const char *dir,
									const char *filename);
char							*append_string(char *original,
									const char *to_append);
size_t							ft_strlen(const char *s);
size_t							ft_strlcpy(char *dst, const char *src,
									size_t size);
int								external_commands(t_command *cmd,
									t_shell *shell);
t_token							*tokenize(char *input, t_shell *shell);
int								execute_builtin(t_command *cmds,
									t_shell *shell);
int								builtin_cd(t_command *cmd, t_env **env_list);
int								builtin_pwd(void);
int								builtin_env(char **env);
int								builtin_echo(t_command *cmd);
void							builtin_history(char *line);
void							cleanup_history(void);
int								builtin_export(t_command *cmd, char ***envp,
									t_env **env_list);
int								is_valid_identifier(const char *str);
char							**copy_env(char **envp);
int								handle_redirections(t_exec_context *ctx);
int								has_pipe(t_command *cmds);
int								execute_piped_commands(t_command *cmds,
									t_token *tokens, t_shell *shell);
t_command						*parse_tokens(t_token *tokens, t_shell *shell);
t_command						*create_new_command(void);
void							add_arg(t_command *cmd, char *arg);
void							add_redirect(t_command *cmd,
									t_redirect_type type, char *filename);
int								handle_redirect_token(t_token **tokens,
									t_command *current_cmd, t_command **head,
									t_shell *shell);
void							handle_word_token(t_command *current_cmd,
									t_token *tokens);
int								handle_pipe_token(t_command **current_cmd,
									t_token **tokens, t_command **head,
									t_shell *shell);
t_env							*add_env_list(t_env **head, char *input);
char							**add_envp(char **envp, char *input);
t_env							*envp_to_list(char **envp);
char							**env_list_to_array(t_env *env_list, int count);
int								builtin_unset(t_command *cmd, char ***envp,
									t_env **env_list);
char							*get_env_value(t_env *env_list,
									const char *key);
char							*ft_strndup(const char *s, size_t n);
int								ft_strncmp(const char *s1, const char *s2,
									size_t n);
void							print_cd_error(const char *path,
									const char *msg);
void							update_pwd_vars(t_env **env_list,
									char *old_pwd_val);
int								is_num(char *str);
int								ft_isalnum(char c);
int								ft_strcmp(const char *s1, const char *s2);
char							*ft_strchr(const char *s, int i);
int								execute_command(t_exec_context *exec);
int								builtin_exit(t_command *cmd,
									int *real_exit_code);
int								is_builtin(const char *cmd);
void							cleanup_and_exit(t_exec_context *ctx,
									int exit_code);
int								ft_atoi(const char *str);
void							bubble_sort(char **arr, int count);
void							free_tokens(t_token *head);
void							free_commands(t_command *head);
void							free_exec(t_exec_context *exec);
void							free_history_list(t_history *head);
char							*find_in_path(char *command_name,
									t_shell *shell);
void							free_array(char **arr);
int								get_exit_status(int status);
void							bubble_sort(char **arr, int count);
t_env							*create_new_env_node(char *key, char *value);
void							append_env_node(t_env **head, t_env *new_node);
t_env							*update_existing_env_node(t_env *current,
									char *key, char *value);
t_env							*create_env_node_from_envp(char *env_str);
void							add_env_node_to_list(t_env **head,
									t_env **current, t_env *new_node);
void							print_export_error(char *input);
void							print_error_message(char *cmd_name,
									char *error_msg, int exit_code,
									t_shell *shell);
int								ft_clean(char **srg, int i);
char							*process_word_with_expansion(char *input,
									int start, int end, t_shell *shell);
int								is_valid_first_char(char c);
int								is_valid_identifier_char(char c);
int								ft_isspace(int c);
int								is_special_char(char c);
int								is_quote(char c);
int								preprocess_heredocs(t_exec_context *exec);
void							cleanup_heredocs(t_command *cmds);
void							add_token_to_list(t_token **head,
									t_token **tail, char *word);
t_token_type					get_token_type(char *value);
void							handle_special_chars(char *input, int *i,
									t_token **head, t_token **tail);
char							*process_word_token(char *input, int *i,
									t_shell *shell);
char							*process_word_no_expansion(char *input,
									int start, int end);
int								validate_command(t_command *cmds,
									t_shell *shell);
int								handle_pipes(t_command *cmds, t_token *tokens,
									t_shell *shell);
int								handle_redirections_block(t_redir_context *ctx,
									t_exec_context *exec);
int								handle_builtin_or_external(t_command *cmds,
									t_shell *shell, int saved_stdout,
									int saved_stdin);
void							restore_redirections(int saved_stdout,
									int saved_stdin);
void							handle_signal_interrupt(t_shell *shell);
void							setup_signals(void);
void							setup_signals_child(void);
void							setup_signals_parent(void);
void							handle_external_error(t_command *cmds,
									int result, t_shell *shell);
int								setup_redirections(t_redir_context *ctx,
									t_exec_context *exec, int *saved_stdout,
									int *saved_stdin);
void							wait_and_free_pids(t_pid_list *head);
int								handle_process_creation(t_exec_context *ctx);
char							*resolve_command_path(char *command_name,
									t_shell *shell);
void							run_child_command(t_exec_context *ctx);
int								handle_heredoc(const char *delimiter,
									t_exec_context *exec);
char							*get_path_env(t_shell *shell);
char							*resolve_path(char *command_name,
									char *path_env);
int								check_absolute_path_status(char *command_name);
void							setup_child_pipes(int prev_fd, int *fd,
									t_command *current);
void							add_pid(t_pid_list **head, pid_t pid);
int								setup_pipe_if_needed(t_command *current,
									int *fd);
void							setup_signals_heredoc(void);
int								check_unclosed_quotes(const char *input);
void							free_tokens_and_commands(t_token *tokens,
									t_command *cmds);
int								process_exit_status(int status);
int								find_is_path(char *command_name);
void							handle_command_not_found(t_exec_context *ctx);
int								handle_syntax_error(char *input,
									t_exec_context *exec);
int								process_parsed_commands(t_token *tokens,
									t_command *cmds, t_exec_context *exec);
int								handle_command_flow(char *input,
									t_exec_context *exec);
int								execute_command(t_exec_context *exec);
int								execute_external_cmd(t_command *cmds,
									t_shell *shell);

#endif
