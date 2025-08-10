/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:31:17 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:32:22 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_and_preprocess(t_exec_context *exec)
{
	if (!validate_command(exec->all_cmds, exec->shell))
		return (1);
	if (preprocess_heredocs(exec) < 0)
	{
		exec->shell->last_exit_code = 1;
		return (1);
	}
	return (0);
}

static int	handle_single_command(t_exec_context *exec)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redir_context	ctx;

	ctx.cmds = exec->all_cmds;
	ctx.saved_stdout = &saved_stdout;
	ctx.saved_stdin = &saved_stdin;
	ctx.shell = exec->shell;
	if (!handle_redirections_block(&ctx, exec))
		return (1);
	if (!handle_builtin_or_external(exec->all_cmds, exec->shell, saved_stdout,
			saved_stdin))
		return (0);
	restore_redirections(saved_stdout, saved_stdin);
	return (1);
}

int	execute_command(t_exec_context *exec)
{
	int	result;

	result = validate_and_preprocess(exec);
	if (result != 0)
		return (result);
	if (handle_pipes(exec->all_cmds, exec->all_tokens, exec->shell))
	{
		cleanup_heredocs(exec->all_cmds);
		return (1);
	}
	result = handle_single_command(exec);
	cleanup_heredocs(exec->all_cmds);
	return (result);
}
