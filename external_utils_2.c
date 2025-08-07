/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:34:44 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 15:31:19 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	find_in_path_safe(char *command_name, t_shell *shell,
		char **program_path)
{
	char	*result;
	char	*path_env;

	path_env = get_path_env(shell);
	if (!path_env)
		return (PATH_NOT_SET);
	result = resolve_path(command_name, path_env);
	if (result)
	{
		*program_path = result;
		return (CMD_SUCCESS);
	}
	return (CMD_NOT_FOUND);
}

static int	handle_absolute_path(char *command_name, char **program_path)
{
	int	check_result;

	check_result = check_absolute_path_status(command_name);
	if (check_result != CMD_SUCCESS)
		return (check_result);
	*program_path = ft_strdup(command_name);
	if (!*program_path)
		return (-1);
	return (0);
}

int	external_commands(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	char	*program_path;
	char	*command_name;
	int		status;
	int		result;

	status = 0;
	command_name = cmd->args[0];
	if (!command_name)
		return (-1);
	if (find_is_path(command_name) || command_name[0] == '.')
		result = handle_absolute_path(command_name, &program_path);
	else
		result = find_in_path_safe(command_name, shell, &program_path);
	if (result != 0)
		return (result);
	setup_signals_parent();
	pid = fork();
	if (pid == 0)
		execute_child_process(program_path, cmd, shell->envp);
	else
		waitpid(pid, &status, 0);
	setup_signals();
	free(program_path);
	return (process_exit_status(status));
}

int	find_is_path(char *command_name)
{
	if (ft_strchr(command_name, '/'))
		return (1);
	else
		return (0);
}
