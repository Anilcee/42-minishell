/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:44:08 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/06 17:23:05 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*handle_heredoc_text(char *input, int *i, t_shell *shell)
{
	char	quote;
	int		start;
	char	*part;

	quote = input[*i];
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	part = process_word_with_expansion(input, start, *i, shell);
	if (input[*i] == quote)
		(*i)++;
	return (part);
}

int	process_heredoc_loop(int pipefd[2], const char *delimiter, t_shell *shell)
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received)
		{
			free(line);
			close(pipefd[0]);
			close(pipefd[1]);
			return (-1);
		}
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		expanded = process_word_with_expansion(line, 0, ft_strlen(line), shell);
		free(line);
		write(pipefd[1], expanded, ft_strlen(expanded));
		write(pipefd[1], "\n", 1);
		free(expanded);
	}
	return (0);
}

int	init_heredoc_pipe(int pipefd[2])
{
	if (pipe(pipefd) == -1)
		return (-1);
	setup_signals_heredoc();
	g_signal_received = 0;
	return (0);
}

int	handle_heredoc(const char *delimiter, t_shell *shell)
{
	int	pipefd[2];

	if (init_heredoc_pipe(pipefd) < 0)
		return (-1);
	if (process_heredoc_loop(pipefd, delimiter, shell) < 0)
		return (-1);
	setup_signals();
	close(pipefd[1]);
	return (pipefd[0]);
}
