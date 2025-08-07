/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_handler.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:31:17 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 15:35:29 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	setup_redirections(t_command *cmds, int *saved_stdout, int *saved_stdin,
						t_shell *shell)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (handle_redirections(cmds, shell) < 0)
	{
		dup2(*saved_stdout, STDOUT_FILENO);
		dup2(*saved_stdin, STDIN_FILENO);
		close(*saved_stdout);
		close(*saved_stdin);
		return (0);
	}
	return (1);
}

void	restore_redirections(int saved_stdout, int saved_stdin)
{
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

void	handle_child_process_inline(t_exec_context *ctx)
{
	wait_and_free_pids(*ctx->pid_list);
	*ctx->pid_list = NULL;
	setup_signals_child();
	setup_child_pipes(ctx->pipe_data.prev_fd, ctx->pipe_data.fd, ctx->current);
	run_child_command(ctx->current, ctx->shell, ctx->all_cmds, ctx->all_tokens);
}

void	handle_parent_process_inline(t_exec_context *ctx, pid_t pid)
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

int	handle_process_creation(t_exec_context *ctx)
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
