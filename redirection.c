/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:53 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 01:35:54 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_wifexited(int status)
{
	return ((status & 0x7f) == 0);
}

int	ft_wexitstatus(int status)
{
	return ((status & 0xff00) >> 8);
}

int	ft_wifsignaled(int status)
{
	return ((status & 0x7f) != 0 && (status & 0x7f) != 0x7f);
}

int	ft_wtermsig(int status)
{
	return (status & 0x7f);
}

void	execute_builtin_commands(t_command *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		exit(builtin_cd(cmd, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		exit(builtin_pwd());
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		exit(builtin_env(shell->envp));
}

void	execute_more_builtins(t_command *cmd, t_shell *shell)
{
	int	ret;
	int	exit_code;

	if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(cmd);
		exit(0);
	}
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		exit(builtin_export(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		exit(builtin_unset(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
	{
		ret = builtin_exit(cmd, &exit_code);
		if (ret == EXIT_TOO_MANY_ARGS)
			exit(1);
		exit(0);
	}
}

int	is_builtin(const char *cmd)
{
	return (ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "env") == 0 || ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "export") == 0 || ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}

void	add_pid(t_pid_list **head, pid_t pid)
{
	t_pid_list	*new_node;
	t_pid_list	*temp;

	new_node = malloc(sizeof(t_pid_list));
	if (!new_node)
		return ;
	new_node->pid = pid;
	new_node->next = NULL;
	if (!*head)
		*head = new_node;
	else
	{
		temp = *head;
		while (temp->next)
			temp = temp->next;
		temp->next = new_node;
	}
}

void	wait_and_free_pids(t_pid_list *head)
{
	t_pid_list	*temp;

	while (head)
	{
		waitpid(head->pid, NULL, 0);
		temp = head;
		head = head->next;
		free(temp);
	}
}

int	has_pipe(t_command *cmds)
{
	return (cmds && cmds->next != NULL);
}

int	handle_heredoc(const char *delimiter)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	return (pipefd[0]);
}

static int	handle_input_redirect(t_redirect *redir, int *in_fd, int out_fd)
{
	if (*in_fd != -1)
		close(*in_fd);
	if (redir->type == REDIR_HEREDOC)
		*in_fd = handle_heredoc(redir->filename);
	else
		*in_fd = open(redir->filename, O_RDONLY);
	if (*in_fd < 0)
	{
		perror(redir->filename);
		if (out_fd != -1)
			close(out_fd);
		return (-1);
	}
	return (0);
}

static int	handle_output_redirect(t_redirect *redir, int *out_fd, int in_fd)
{
	int	flags;

	if (*out_fd != -1)
		close(*out_fd);
	flags = O_WRONLY | O_CREAT;
	if (redir->type == REDIR_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	*out_fd = open(redir->filename, flags, 0644);
	if (*out_fd < 0)
	{
		perror(redir->filename);
		if (in_fd != -1)
			close(in_fd);
		return (-1);
	}
	return (0);
}

static void	apply_redirections(int in_fd, int out_fd)
{
	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
}

int	handle_redirections(t_command *cmd)
{
	t_redirect	*redir;
	int			in_fd;
	int			out_fd;

	redir = cmd->redirects;
	in_fd = -1;
	out_fd = -1;
	while (redir)
	{
		if (redir->type == REDIR_IN || redir->type == REDIR_HEREDOC)
		{
			if (handle_input_redirect(redir, &in_fd, out_fd) < 0)
				return (-1);
		}
		else if (redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
		{
			if (handle_output_redirect(redir, &out_fd, in_fd) < 0)
				return (-1);
		}
		redir = redir->next;
	}
	apply_redirections(in_fd, out_fd);
	return (0);
}

static void	print_error_and_exit(char *cmd, char *msg, int exit_code)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, msg, ft_strlen(msg));
	exit(exit_code);
}

static char	*validate_absolute_path(char *command_name)
{
	DIR	*dir;

	if (access(command_name, F_OK) != 0)
		print_error_and_exit(command_name, ": No such file or directory\n",
			127);
	dir = opendir(command_name);
	if (dir != NULL)
	{
		closedir(dir);
		print_error_and_exit(command_name, ": Is a directory\n", 126);
	}
	if (access(command_name, X_OK) != 0)
		print_error_and_exit(command_name, ": Permission denied\n", 126);
	return (ft_strdup(command_name));
}

static void	cleanup_paths_array(char **paths)
{
	int	j;

	j = 0;
	while (paths[j])
		free(paths[j++]);
	free(paths);
}

static char	*search_in_paths(char *command_name, char **paths)
{
	char	*program_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		program_path = build_path(paths[i], command_name);
		if (access(program_path, X_OK) == 0)
			return (program_path);
		free(program_path);
		program_path = NULL;
		i++;
	}
	return (NULL);
}

static char	*get_path_env(t_shell *shell)
{
	int	i;

	if (shell->env_list)
		return (get_env_value(shell->env_list, "PATH"));
	else
	{
		i = -1;
		while (shell->envp && shell->envp[++i])
		{
			if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
				return (shell->envp[i] + 5);
		}
	}
	return (NULL);
}

static char	*resolve_path(char *command_name, char *path_env)
{
	char	**paths;
	char	*program_path;

	paths = ft_split(path_env, ':');
	program_path = search_in_paths(command_name, paths);
	cleanup_paths_array(paths);
	return (program_path);
}

static char	*resolve_path_command(char *command_name, t_shell *shell)
{
	char	*path_env;
	char	*program_path;

	path_env = get_path_env(shell);
	if (!path_env)
		print_error_and_exit(command_name, ": No such file or directory\n",
			127);
	program_path = resolve_path(command_name, path_env);
	if (!program_path)
		print_error_and_exit(command_name, ": command not found\n", 127);
	return (program_path);
}

static char	*resolve_command_path(char *command_name, t_shell *shell)
{
	if (command_name[0] == '/' || command_name[0] == '.')
		return (validate_absolute_path(command_name));
	else
		return (resolve_path_command(command_name, shell));
}

static void	setup_child_pipes(int prev_fd, int *fd, t_command *current)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (current->next)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
}

static void	run_child_command(t_command *current_cmd, t_shell *shell,
		t_command *all_cmds, t_token *all_tokens)
{
	char	*program_path;
	int		ret;

	if (handle_redirections(current_cmd) < 0)
		cleanup_and_exit(shell, all_cmds, all_tokens, 1);
	if (!current_cmd->args || !current_cmd->args[0])
		cleanup_and_exit(shell, all_cmds, all_tokens, 0);
	if (is_builtin(current_cmd->args[0]))
	{
		ret = execute_builtin(current_cmd, shell);
		cleanup_and_exit(shell, all_cmds, all_tokens, ret);
	}
	program_path = resolve_command_path(current_cmd->args[0], shell);
	if (!program_path)
	{
		print_error_message(current_cmd->args[0], ": command not found\n", 127,
			NULL);
		cleanup_and_exit(shell, all_cmds, all_tokens, 127);
	}
	execve(program_path, current_cmd->args, shell->envp);
	free(program_path);
	perror("minishell: execve");
	cleanup_and_exit(shell, all_cmds, all_tokens, 126);
}

static int	handle_basic_builtins(t_command *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(shell->envp));
	else if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(cmd);
		return (0);
	}
	return (-1);
}

static int	handle_env_builtins(t_command *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd, &shell->envp, &shell->env_list));
	return (-1);
}

static int	handle_exit_builtin(t_command *cmd)
{
	int	real_exit_code;
	int	ret;

	ret = builtin_exit(cmd, &real_exit_code);
	if (ret == EXIT_ARG_VALUE)
		return (real_exit_code);
	else if (ret == EXIT_TOO_MANY_ARGS)
		return (1);
	else if (ret == EXIT_NOT_NUMERIC)
		return (2);
	return (0);
}

void	execute_builtin_in_child(t_command *cmd, t_shell *shell,
		t_command *all_cmds, t_token *all_tokens)
{
	int	exit_code;

	exit_code = handle_basic_builtins(cmd, shell);
	if (exit_code != -1)
		cleanup_and_exit(shell, all_cmds, all_tokens, exit_code);
	exit_code = handle_env_builtins(cmd, shell);
	if (exit_code != -1)
		cleanup_and_exit(shell, all_cmds, all_tokens, exit_code);
	if (ft_strcmp(cmd->args[0], "exit") == 0)
	{
		exit_code = handle_exit_builtin(cmd);
		cleanup_and_exit(shell, all_cmds, all_tokens, exit_code);
	}
	cleanup_and_exit(shell, all_cmds, all_tokens, 0);
}

static int	setup_pipe_if_needed(t_command *current, int *fd)
{
	if (current->next)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (-1);
		}
	}
	return (0);
}

static void	handle_child_process_inline(t_execution_context *ctx)
{
	setup_child_pipes(ctx->pipe_data.prev_fd, ctx->pipe_data.fd, ctx->current);
	run_child_command(ctx->current, ctx->shell, ctx->all_cmds, ctx->all_tokens);
}

static void	handle_parent_process_inline(t_execution_context *ctx, pid_t pid)
{
	add_pid(ctx->pid_list, pid);
	if (ctx->pipe_data.prev_fd != -1)
		close(ctx->pipe_data.prev_fd);
	if (ctx->current->next)
	{
		close(ctx->pipe_data.fd[1]);
		ctx->pipe_data.prev_fd = ctx->pipe_data.fd[0];
	}
}

static int	handle_process_creation(t_execution_context *ctx)
{
	pid_t	pid;

	if (setup_pipe_if_needed(ctx->current, ctx->pipe_data.fd) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		handle_child_process_inline(ctx);
	else
		handle_parent_process_inline(ctx, pid);
	return (0);
}

static int	calculate_final_exit_code(int status)
{
	if (ft_wifexited(status))
		return (ft_wexitstatus(status));
	else if (ft_wifsignaled(status))
		return (128 + ft_wtermsig(status));
	return (0);
}

static int	wait_for_all_processes(t_pid_list *pid_list)
{
	t_pid_list	*temp;
	int			status;
	int			final_exit_code;

	final_exit_code = 0;
	temp = pid_list;
	while (temp)
	{
		waitpid(temp->pid, &status, 0);
		if (ft_wifsignaled(status) && ft_wtermsig(status) == SIGPIPE)
			write(STDERR_FILENO, "Broken pipe\n", 12);
		if (temp->next == NULL)
			final_exit_code = calculate_final_exit_code(status);
		temp = temp->next;
	}
	return (final_exit_code);
}

void	cleanup_resources(t_pid_list *pid_list, t_shell shell)
{
	t_pid_list	*temp;

	while (pid_list)
	{
		temp = pid_list;
		pid_list = pid_list->next;
		free(temp);
	}
	if (shell.env_list)
		free_env_list(shell.env_list);
}

int	execute_piped_commands(t_command *cmds, t_token *tokens, t_shell *shell)
{
	t_command			*current;
	t_pid_list			*pid_list;
	t_execution_context	ctx;
	int					final_exit_code;

	current = cmds;
	pid_list = NULL;
	ctx.pipe_data.prev_fd = -1;
	ctx.shell = shell;
	ctx.all_cmds = cmds;
	ctx.all_tokens = tokens;
	ctx.pid_list = &pid_list;
	while (current)
	{
		ctx.current = current;
		if (handle_process_creation(&ctx) < 0)
		{
			wait_and_free_pids(pid_list);
			return (1);
		}
		current = current->next;
	}
	final_exit_code = wait_for_all_processes(pid_list);
	wait_and_free_pids(pid_list);
	return (final_exit_code);
}
