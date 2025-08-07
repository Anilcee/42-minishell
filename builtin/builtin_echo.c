/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 02:02:28 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 17:56:33 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	should_print_newline(char **args, int *i)
{
	int	j;
	int	newline;

	newline = 1;
	while (args[++(*i)])
	{
		if (args[*i][0] != '-')
			break ;
		j = 1;
		while (args[*i][j] == 'n')
			j++;
		if (args[*i][j] != '\0')
			break ;
		newline = 0;
	}
	return (newline);
}

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	newline;

	i = 0;
	newline = should_print_newline(cmd->args, &i);
	while (cmd->args[i])
	{
		write(STDOUT_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
		if (cmd->args[++i])
			write(STDOUT_FILENO, " ", 1);
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}
