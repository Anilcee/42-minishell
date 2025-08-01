/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:53 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 16:35:47 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_child_command(t_command *current_cmd, t_shell *shell,
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
		exit_code = handle_exit_builtin_child(cmd);
		cleanup_and_exit(shell, all_cmds, all_tokens, exit_code);
	}
	cleanup_and_exit(shell, all_cmds, all_tokens, 0);
}

static int	calculate_final_exit_code(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
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
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGPIPE)
			write(STDERR_FILENO, "Broken pipe\n", 12);
		if (temp->next == NULL)
			final_exit_code = calculate_final_exit_code(status);
		temp = temp->next;
	}
	return (final_exit_code);
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
