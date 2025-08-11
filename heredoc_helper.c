/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 03:53:48 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 04:00:40 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_heredoc_signal(int status, int pipefd[2],
		t_exec_context *exec)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
		{
			write(1, "\n", 1);
			g_signal_received = SIGINT;
			handle_signal_interrupt(exec->shell);
			close(pipefd[0]);
			return (-1);
		}
	}
	return (pipefd[0]);
}

void	cleanup_heredocs(t_command *cmds)
{
	t_command	*cmd;
	t_redirect	*redir;

	cmd = cmds;
	while (cmd)
	{
		redir = cmd->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->processed_fd != -1)
			{
				close(redir->processed_fd);
				redir->processed_fd = -1;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
