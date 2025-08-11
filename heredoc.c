/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:44:08 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/11 04:05:44 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_heredoc_warning(const char *delimiter)
{
	write(2,
		"minishell: warning: here-document at line x",
		44);
	write(2,
		" delimited by end-of-file (wanted `", 36);
	write(2, delimiter, ft_strlen(delimiter));
	write(2, "')\n", 4);
}

static int	process_heredoc_line(char *line, const char *delimiter,
		int pipefd[2], t_exec_context *exect)
{
	char	*expanded;

	if (!line || ft_strcmp(line, delimiter) == 0)
	{
		if (!line && g_signal_received != SIGINT)
			print_heredoc_warning(delimiter);
		if (line)
			free(line);
		return (0);
	}
	expanded = process_word_with_expansion(line, 0, ft_strlen(line),
			exect->shell);
	free(line);
	write(pipefd[1], expanded, ft_strlen(expanded));
	write(pipefd[1], "\n", 1);
	free(expanded);
	return (1);
}

static void	child_heredoc_routine(const char *delimiter, int pipefd[2],
		t_exec_context *exect)
{
	char	*line;

	setup_signals_heredoc();
	close(pipefd[0]);
	while (1)
	{
		line = readline("> ");
		if (g_signal_received == SIGINT)
		{
			if (line)
				free(line);
			close(pipefd[1]);
			cleanup_and_exit(exect, 130);
		}
		if (!process_heredoc_line(line, delimiter, pipefd, exect))
			break ;
	}
	close(pipefd[1]);
	cleanup_and_exit(exect, 0);
}

static int	create_heredoc_process(const char *delimiter, t_exec_context *exec,
		int pipefd[2])
{
	pid_t	pid;

	setup_signals_parent();
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		child_heredoc_routine(delimiter, pipefd, exec);
	return (pid);
}

int	handle_heredoc(const char *delimiter, t_exec_context *exec)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipefd) == -1)
		return (-1);
	pid = create_heredoc_process(delimiter, exec, pipefd);
	if (pid == -1)
		return (-1);
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
	return (handle_heredoc_signal(status, pipefd, exec));
}
