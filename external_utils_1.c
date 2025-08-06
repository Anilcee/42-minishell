/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils_1.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:36 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/06 17:55:20 by oislamog         ###   ########.fr       */
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

int	execute_child_process(char *program_path, t_command *cmd, char **envp)
{
	int	status;

	setup_signals_child();
	status = check_absolute_path_status(program_path);
	if (status == FILE_NOT_FOUND)
		exit(127);
	else if (status == IS_DIRECTORY)
		exit(126);
	else if (status == PERMISSION_DENIED)
		exit(126);
	execve(program_path, cmd->args, envp);
	exit(127);
}
