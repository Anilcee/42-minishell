/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:44:08 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/12 14:47:18 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_heredoc_warning(const char *delimiter)
{
	write(2, "minishell: warning: here-document at line x", 44);
	write(2, " delimited by end-of-file (wanted `", 36);
	write(2, delimiter, ft_strlen(delimiter));
	write(2, "')\n", 4);
}

static void	read_heredoc_input_loop(const char *delimiter, char **buffer,
		int write_fd, t_exec_context *exect)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received == SIGINT)
		{
			if (line)
				free(line);
			free(*buffer);
			close(write_fd);
			cleanup_and_exit(exect, 130);
		}
		if (!append_heredoc_line(line, delimiter, buffer, exect))
			break ;
	}
}

static void	child_heredoc_routine(const char *delimiter, int pipefd[2],
		t_exec_context *exect)
{
	char	*buffer;

	setup_signals_heredoc();
	close(pipefd[0]);
	buffer = ft_strdup("");
	if (!buffer)
	{
		close(pipefd[1]);
		cleanup_and_exit(exect, 1);
	}
	read_heredoc_input_loop(delimiter, &buffer, pipefd[1], exect);
	write(pipefd[1], buffer, ft_strlen(buffer));
	free(buffer);
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
