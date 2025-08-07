/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:48:37 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 17:56:09 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
		i++;
	return (i);
}

static int	check_too_many_args(int argc)
{
	if (argc > 2)
	{
		print_cd_error(NULL, "too many arguments");
		return (1);
	}
	return (0);
}

static char	*resolve_target_path(char *arg, int argc, t_env *env_list)
{
	char	*path;

	if (argc == 1 || ft_strcmp(arg, "~") == 0)
	{
		path = get_env_value(env_list, "HOME");
		if (!path || *path == '\0')
		{
			print_cd_error(NULL, "HOME not set");
			return (NULL);
		}
	}
	else if (ft_strcmp(arg, "-") == 0)
	{
		path = get_env_value(env_list, "OLDPWD");
		if (!path || *path == '\0')
		{
			print_cd_error(NULL, "OLDPWD not set");
			return (NULL);
		}
		printf("%s\n", path);
	}
	else
		path = arg;
	return (path);
}

static int	try_change_directory(char *path)
{
	if (chdir(path) != 0)
	{
		print_cd_error(path, "No such file or directory");
		return (1);
	}
	return (0);
}

int	builtin_cd(t_command *cmd, t_env **env_list)
{
	char	*current_pwd;
	char	*target_path;
	int		argc;

	argc = count_args(cmd->args);
	if (check_too_many_args(argc))
		return (1);
	current_pwd = getcwd(NULL, 0);
	if (!current_pwd)
		return (print_cd_error("getcwd", "error retrieving current directory"),
			1);
	target_path = resolve_target_path(cmd->args[1], argc, *env_list);
	if (!target_path)
	{
		free(current_pwd);
		return (1);
	}
	if (try_change_directory(target_path))
	{
		free(current_pwd);
		return (1);
	}
	update_pwd_vars(env_list, current_pwd);
	free(current_pwd);
	return (0);
}
