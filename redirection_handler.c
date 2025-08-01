/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_handler.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:31:17 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 12:27:07 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	setup_redirections(t_command *cmds, int *saved_stdout, int *saved_stdin)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (handle_redirections(cmds) < 0)
	{
		dup2(*saved_stdout, STDOUT_FILENO);
		dup2(*saved_stdin, STDIN_FILENO);
		close(*saved_stdout);
		close(*saved_stdin);
		return (0);
	}
	return (1);
}

void	restore_redirections(int saved_stdout, int saved_stdin)
{
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}
