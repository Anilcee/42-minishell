/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:34:44 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:15:22 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	execute_child_process(char *program_path, t_command *cmd,
		char **envp)
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

static int	handle_command_not_found_error(char *command_name, t_shell *shell)
{
	int		check_result;
	char	*path_env;

	if (find_is_path(command_name) || command_name[0] == '.')
	{
		check_result = check_absolute_path_status(command_name);
		return (check_result);
	}
	else
	{
		path_env = get_path_env(shell);
		if (!path_env)
			return (PATH_NOT_SET);
	}
	return (CMD_NOT_FOUND);
}

static int	execute_external_process(char *program_path, t_command *cmd,
		t_shell *shell)
{
	pid_t	pid;
	int		status;

	setup_signals_parent();
	pid = fork();
	if (pid == 0)
		execute_child_process(program_path, cmd, shell->envp);
	else
		waitpid(pid, &status, 0);
	setup_signals();
	return (process_exit_status(status));
}

int	external_commands(t_command *cmd, t_shell *shell)
{
	char	*program_path;
	char	*command_name;
	int		result;

	command_name = cmd->args[0];
	if (!command_name)
		return (-1);
	program_path = resolve_command_path(command_name, shell);
	if (!program_path)
		return (handle_command_not_found_error(command_name, shell));
	result = execute_external_process(program_path, cmd, shell);
	free(program_path);
	return (result);
}

int	find_is_path(char *command_name)
{
	if (ft_strchr(command_name, '/'))
		return (1);
	else
		return (0);
}
