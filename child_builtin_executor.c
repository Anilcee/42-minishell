/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_builtin_executer.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:36:13 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/05 17:36:59 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_builtin_commands(t_command *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		exit(builtin_cd(cmd, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		exit(builtin_pwd());
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		exit(builtin_env(shell->envp));
}

void	execute_more_builtins(t_command *cmd, t_shell *shell)
{
	int	ret;
	int	exit_code;

	if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(cmd);
		exit(0);
	}
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		exit(builtin_export(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		exit(builtin_unset(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
	{
		ret = builtin_exit(cmd, &exit_code);
		if (ret == EXIT_TOO_MANY_ARGS)
			exit(1);
		exit(0);
	}
}

int	is_builtin(const char *cmd)
{
	return (ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "env") == 0 || ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "export") == 0 || ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}
