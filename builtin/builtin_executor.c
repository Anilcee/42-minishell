/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:31:17 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 12:07:52 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_cd_pwd_env(t_command *cmds, t_shell *shell)
{
	if (ft_strcmp(cmds->args[0], "cd") == 0)
		shell->last_exit_code = builtin_cd(cmds, &shell->env_list);
	else if (ft_strcmp(cmds->args[0], "pwd") == 0)
		shell->last_exit_code = builtin_pwd();
	else if (ft_strcmp(cmds->args[0], "env") == 0)
		shell->last_exit_code = builtin_env(shell->envp);
	else
		return (0);
	return (1);
}

static int	handle_echo_unset_export(t_command *cmds, t_shell *shell)
{
	if (ft_strcmp(cmds->args[0], "echo") == 0)
	{
		builtin_echo(cmds);
		shell->last_exit_code = 0;
	}
	else if (ft_strcmp(cmds->args[0], "unset") == 0)
		shell->last_exit_code = builtin_unset(cmds, &shell->envp,
				&shell->env_list);
	else if (ft_strcmp(cmds->args[0], "export") == 0)
		shell->last_exit_code = builtin_export(cmds, &shell->envp,
				&shell->env_list);
	else
		return (0);
	return (1);
}

static int	handle_exit_builtin(t_command *cmds, t_shell *shell)
{
	int	ret;
	int	exit_code;

	if (ft_strcmp(cmds->args[0], "exit") != 0)
		return (0);
	ret = builtin_exit(cmds, &exit_code);
	if (ret == EXIT_TOO_MANY_ARGS)
		shell->last_exit_code = 1;
	else if (ret == EXIT_NOT_NUMERIC)
		shell->last_exit_code = 2;
	else if (ret == EXIT_NO_ARG)
		return (1);
	else if (ret == EXIT_ARG_VALUE)
	{
		shell->last_exit_code = exit_code;
		return (1);
	}
	return (1);
}

int	execute_builtin(t_command *cmds, t_shell *shell)
{
	if (handle_cd_pwd_env(cmds, shell))
		return (0);
	if (handle_echo_unset_export(cmds, shell))
		return (0);
	if (handle_exit_builtin(cmds, shell))
		return (-1);
	return (0);
}
