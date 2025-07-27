/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 16:30:00 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 16:30:00 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	setup_redirections(t_command *cmds, int *saved_stdout, int *saved_stdin)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (handle_redirections(cmds) < 0)
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

int	execute_external_cmd(t_command *cmds, t_shell *shell)
{
	int	external_result;

	external_result = external_commands(cmds, shell);
	if (external_result < 0)
	{
		handle_external_error(cmds, external_result, shell);
		handle_external_error2(cmds, external_result, shell);
	}
	else
		shell->last_exit_code = external_result;
	return (1);
}

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

int	execute_command(t_command *cmds, t_token *tokens, t_shell *shell)
{
	int						saved_stdout;
	int						saved_stdin;
	t_redirection_context	ctx;

	if (!validate_command(cmds, shell))
		return (1);
	if (has_pipe(cmds))
	{
		shell->last_exit_code = execute_piped_commands(cmds, tokens, shell);
		return (1);
	}
	ctx.cmds = cmds;
	ctx.saved_stdout = &saved_stdout;
	ctx.saved_stdin = &saved_stdin;
	ctx.shell = shell;
	if (!setup_redirections(ctx.cmds, ctx.saved_stdout, ctx.saved_stdin))
	{
		ctx.shell->last_exit_code = 1;
		return (1);
	}
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
	restore_redirections(saved_stdout, saved_stdin);
	return (1);
}
