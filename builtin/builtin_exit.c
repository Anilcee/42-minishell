/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 11:49:22 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/05 17:37:52 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_exit(t_command *cmd, int *real_exit_code)
{
	printf("exit\n");
	if (!cmd->args[1])
		return (EXIT_NO_ARG);
	if (!is_num(cmd->args[1]))
	{
		write(STDERR_FILENO, "minishell: exit: numeric argument required\n",
			43);
		return (EXIT_NOT_NUMERIC);
	}
	if (cmd->args[2])
	{
		write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
		return (EXIT_TOO_MANY_ARGS);
	}
	*real_exit_code = ft_atoi(cmd->args[1]) % 256;
	if (*real_exit_code < 0)
		*real_exit_code += 256;
	return (EXIT_ARG_VALUE);
}
