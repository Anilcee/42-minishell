/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:44:08 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/08 20:49:18 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_heredoc_routine(const char *delimiter, int pipefd[2], t_exec_context *exect)
{
	char	*line;
	char	*expanded;

	setup_signals_heredoc();
	close(pipefd[0]);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if(!line)
			{
				write(2, "minishell: warning: here-document at line x delimited by end-of-file (wanted `", 79);
				write(2, delimiter, ft_strlen(delimiter));
				write(2, "')\n", 4);
			}
			if (line)
				free(line);
			break ;
		}
		expanded = process_word_with_expansion(line, 0, ft_strlen(line), exect->shell);
		free(line);
		write(pipefd[1], expanded, ft_strlen(expanded));
		write(pipefd[1], "\n", 1);
		free(expanded);
	}
	close(pipefd[1]);
	cleanup_and_exit(exect,0);
}

int	handle_heredoc(const char *delimiter, t_exec_context *exec)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipefd) == -1)
		return (-1);
	setup_signals_parent();
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		child_heredoc_routine(delimiter, pipefd, exec);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
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
