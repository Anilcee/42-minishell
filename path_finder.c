/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_finder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:58:03 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/05 22:23:24 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*validate_absolute_path(char *command_name)
{
	int	status;

	status = check_absolute_path_status(command_name);
	if (status == FILE_NOT_FOUND)
		print_error_and_exit(command_name, ": No such file or directory\n",
			127);
	else if (status == IS_DIRECTORY)
		print_error_and_exit(command_name, ": Is a directory\n", 126);
	else if (status == PERMISSION_DENIED)
		print_error_and_exit(command_name, ": Permission denied\n", 126);
	return (ft_strdup(command_name));
}

char	*search_in_paths(char *command_name, char **paths)
{
	char	*program_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		program_path = build_path(paths[i], command_name);
		if (access(program_path, X_OK) == 0)
			return (program_path);
		free(program_path);
		program_path = NULL;
		i++;
	}
	return (NULL);
}

char	*resolve_path(char *command_name, char *path_env)
{
	char	**paths;
	char	*program_path;

	paths = ft_split(path_env, ':');
	program_path = search_in_paths(command_name, paths);
	free_array(paths);
	return (program_path);
}

char	*resolve_path_command(char *command_name, t_shell *shell)
{
	char	*path_env;
	char	*program_path;

	path_env = get_path_env(shell);
	if (!path_env)
	{
		//write(STDERR_FILENO, "minishell: ", 12);
		print_error_and_exit(command_name, ": No such file or directory\n",
			127);
	}
	program_path = resolve_path(command_name, path_env);
	if (!program_path)
		print_error_and_exit(command_name, ": command not found\n", 127);
	return (program_path);
}

char	*resolve_command_path(char *command_name, t_shell *shell)
{
	if (command_name[0] == '/' || command_name[0] == '.')
		return (validate_absolute_path(command_name));
	else
		return (resolve_path_command(command_name, shell));
}
