/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 03:53:48 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/12 14:45:18 by oislamog         ###   ########.fr       */
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

static int	append_and_expand_to_buffer(char *line, char **buffer,
		t_exec_context *exect)
{
	char	*expanded;
	char	*with_newline;
	char	*new_buffer;

	expanded = process_word_with_expansion(line, 0, ft_strlen(line),
			exect->shell);
	with_newline = ft_strjoin(expanded, "\n");
	free(line);
	if (!with_newline)
	{
		free(expanded);
		return (0);
	}
	new_buffer = ft_strjoin(*buffer, with_newline);
	free(expanded);
	free(with_newline);
	if (!new_buffer)
		return (0);
	free(*buffer);
	*buffer = new_buffer;
	return (1);
}

int	append_heredoc_line(char *line, const char *delimiter,
		char **buffer, t_exec_context *exect)
{
	if (!line || ft_strcmp(line, delimiter) == 0)
	{
		if (!line && g_signal_received != SIGINT)
			print_heredoc_warning(delimiter);
		free(line);
		return (0);
	}
	return (append_and_expand_to_buffer(line, buffer, exect));
}
