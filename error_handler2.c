/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handler2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:36 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 17:49:33 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_exit_status(int status)
{
	int	sig;

	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (sig == SIGQUIT)
			write(STDERR_FILENO, "Quit (core dumped)\n", 20);
		else if (sig == SIGPIPE)
			write(STDERR_FILENO, "Broken pipe\n", 12);
		return (128 + sig);
	}
	else if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

static void	handle_absolute_path_error(t_exec_context *ctx, int status)
{
	write(STDERR_FILENO, "minishell: ", 12);
	if (status == FILE_NOT_FOUND)
		print_error_message(ctx->current->args[0],
			": No such file or directory\n", 127, ctx->shell);
	else if (status == IS_DIRECTORY)
		print_error_message(ctx->current->args[0], ": Is a directory\n",
			126, ctx->shell);
	else if (status == PERMISSION_DENIED)
		print_error_message(ctx->current->args[0],
			": Permission denied\n", 126, ctx->shell);
}

void	handle_command_not_found(t_exec_context *ctx)
{
	int	status;

	if (find_is_path(ctx->current->args[0])
		|| ctx->current->args[0][0] == '.')
	{
		status = check_absolute_path_status(ctx->current->args[0]);
		handle_absolute_path_error(ctx, status);
	}
	else
	{
		print_error_message(ctx->current->args[0], ": command not found\n",
			127, ctx->shell);
	}
	cleanup_and_exit(ctx, ctx->shell->last_exit_code);
}
