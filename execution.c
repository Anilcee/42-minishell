/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:53 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/06 21:28:54 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_child_command(t_command *current_cmd, t_shell *shell,
		t_command *all_cmds, t_token *all_tokens)
{
	char	*program_path;
	int		ret;

	if (handle_redirections(current_cmd, shell) < 0)
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

static int	wait_for_all_processes(t_pid_list *pid_list)
{
	t_pid_list	*temp;
	t_pid_list	*next;
	int			status;
	int			final_exit_code;

	final_exit_code = 0;
	temp = pid_list;
	while (temp)
	{
		waitpid(temp->pid, &status, 0);
		if (temp->next == NULL)
			final_exit_code = process_exit_status(status);
		next = temp->next;
		free(temp);
		temp = next;
	}
	return (final_exit_code);
}

static int	execute_command_loop(t_execution_context *ctx)
{
	t_command	*current;

	current = ctx->all_cmds;
	while (current)
	{
		ctx->current = current;
		if (handle_process_creation(ctx) < 0)
		{
			return (1);
		}
		current = current->next;
	}
	return (0);
}

int	execute_piped_commands(t_command *cmds, t_token *tokens, t_shell *shell)
{
	t_pid_list			*pid_list;
	t_execution_context	ctx;
	int					final_exit_code;

	pid_list = NULL;
	ctx.pipe_data.prev_fd = -1;
	ctx.shell = shell;
	ctx.all_cmds = cmds;
	ctx.all_tokens = tokens;
	ctx.pid_list = &pid_list;
	setup_signals_parent();
	if (execute_command_loop(&ctx) != 0)
	{
		wait_and_free_pids(pid_list);
		setup_signals();
		return (1);
	}
	final_exit_code = wait_for_all_processes(pid_list);
	return (final_exit_code);
}
