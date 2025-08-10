/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 01:40:00 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:40:00 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	validate_command(t_command *cmds, t_shell *shell)
{
	if (!cmds || !shell || !shell->envp || !shell->env_list)
		return (0);
	if (!cmds->args || !cmds->args[0])
	{
		shell->last_exit_code = 1;
		return (0);
	}
	return (1);
}

int	handle_pipes(t_command *cmds, t_token *tokens, t_shell *shell)
{
	if (has_pipe(cmds))
	{
		shell->last_exit_code = execute_piped_commands(cmds, tokens, shell);
		return (1);
	}
	return (0);
}

int	handle_redirections_block(t_redir_context *ctx, t_exec_context *exec)
{
	(void)exec;
	if (!setup_redirections(ctx, exec, ctx->saved_stdout,
			ctx->saved_stdin))
	{
		ctx->shell->last_exit_code = 1;
		return (0);
	}
	return (1);
}

int	handle_builtin_or_external(t_command *cmds, t_shell *shell,
		int saved_stdout, int saved_stdin)
{
	if (is_builtin(cmds->args[0]))
	{
		if (execute_builtin(cmds, shell) == -1)
		{
			restore_redirections(saved_stdout, saved_stdin);
			return (0);
		}
	}
	else
		execute_external_cmd(cmds, shell);
	return (1);
}

int	execute_external_cmd(t_command *cmds, t_shell *shell)
{
	int	external_result;

	external_result = external_commands(cmds, shell);
	if (external_result < 0)
	{
		handle_external_error(cmds, external_result, shell);
	}
	else
		shell->last_exit_code = external_result;
	return (1);
} 