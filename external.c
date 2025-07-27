/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:36 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 01:35:37 by ancengiz         ###   ########.fr       */
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

static char	*get_path_env_value(t_shell *shell)
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

static int	search_command_in_paths(char **paths, char *command_name,
		char **program_path)
{
	int	i;

	i = -1;
	while (paths[++i])
	{
		*program_path = build_path(paths[i], command_name);
		if (access(*program_path, X_OK) == 0)
		{
			free_paths_array(paths);
			return (CMD_SUCCESS);
		}
		free(*program_path);
		*program_path = NULL;
	}
	free_paths_array(paths);
	return (CMD_NOT_FOUND);
}

static int	find_in_path_safe(char *command_name, t_shell *shell,
		char **program_path)
{
	char	*path_env;
	char	**paths;

	path_env = get_path_env_value(shell);
	if (!path_env)
		return (PATH_NOT_SET);
	paths = ft_split(path_env, ':');
	return (search_command_in_paths(paths, command_name, program_path));
}

char	*find_in_path(char *command_name, t_shell *shell)
{
	char	*program_path;

	if (find_in_path_safe(command_name, shell, &program_path) == CMD_SUCCESS)
		return (program_path);
	return (NULL);
}

int	execute_child_process(char *program_path, t_command *cmd, char **envp)
{
	DIR	*dir;

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

int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	else
		return (1);
}

static int	handle_absolute_path(char *command_name, char **program_path)
{
	int	check_result;

	check_result = check_absolute_path(command_name);
	if (check_result != CMD_SUCCESS)
		return (check_result);
	*program_path = ft_strdup(command_name);
	if (!*program_path)
		return (-1);
	return (0);
}

static int	handle_relative_path(char *command_name, char **program_path,
		t_shell *shell)
{
	return (find_in_path_safe(command_name, shell, program_path));
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
	if (command_name[0] == '/' || command_name[0] == '.')
		result = handle_absolute_path(command_name, &program_path);
	else
		result = handle_relative_path(command_name, &program_path, shell);
	if (result != 0)
		return (result);
	pid = fork();
	if (pid == 0)
		execute_child_process(program_path, cmd, shell->envp);
	else
		waitpid(pid, &status, 0);
	free(program_path);
	return (get_exit_status(status));
}
