/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:44:08 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/05 22:44:37 by oislamog         ###   ########.fr       */
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

int	handle_heredoc(const char *delimiter, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	char	*expanded;

	if (pipe(pipefd) == -1)
		return (-1);
	setup_signals_heredoc();
	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		expanded = process_word_with_expansion(line, 0, ft_strlen(line), shell);
		free(line);
		write(pipefd[1], expanded, ft_strlen(expanded));
		write(pipefd[1], "\n", 1);
		free(expanded);
	}
	close(pipefd[1]);
	return (pipefd[0]);
}
