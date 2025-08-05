/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils_1.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:36 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 16:09:09 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_absolute_path(char *command_name)
{
	DIR	*dir;

	if (access(command_name, F_OK) != 0)
		return (FILE_NOT_FOUND);
	dir = opendir(command_name);
	if (dir != NULL)
	{
		closedir(dir);
		return (IS_DIRECTORY);
	}
	if (access(command_name, X_OK) != 0)
		return (PERMISSION_DENIED);
	return (CMD_SUCCESS);
}

char	*get_path_env_value(t_shell *shell)
{
	int	i;

	if (shell->env_list)
		return (get_env_value(shell->env_list, "PATH"));
	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
			return (shell->envp[i] + 5);
		i++;
	}
	return (NULL);
}

int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	else
		return (1);
}

int	execute_child_process(char *program_path, t_command *cmd, char **envp)
{
	DIR	*dir;

	setup_signals_child();
	if (access(program_path, F_OK) != 0)
		exit(127);
	dir = opendir(program_path);
	if (dir != NULL)
	{
		closedir(dir);
		exit(126);
	}
	if (access(program_path, X_OK) != 0)
		exit(126);
	execve(program_path, cmd->args, envp);
	exit(127);
}

int	search_command_in_paths(char **paths, char *command_name,
		char **program_path)
{
	int	i;

	i = -1;
	while (paths[++i])
	{
		*program_path = build_path(paths[i], command_name);
		if (access(*program_path, X_OK) == 0)
		{
			free_array(paths);
			return (CMD_SUCCESS);
		}
		free(*program_path);
		*program_path = NULL;
	}
	free_array(paths);
	return (CMD_NOT_FOUND);
}
