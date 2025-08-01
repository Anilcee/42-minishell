/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_and_heredoc_utils.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:49:39 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/01 20:09:00 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_input_redirect(t_redirect *redir, int *in_fd, int out_fd,
		t_shell *shell)
{
	if (*in_fd != -1)
		close(*in_fd);
	if (redir->type == REDIR_HEREDOC)
		*in_fd = handle_heredoc(redir->filename, shell);
	else
		*in_fd = open(redir->filename, O_RDONLY);
	if (*in_fd < 0)
	{
		perror(redir->filename);
		if (out_fd != -1)
			close(out_fd);
		return (-1);
	}
	return (0);
}

int	handle_output_redirect(t_redirect *redir, int *out_fd, int in_fd)
{
	int	flags;

	if (*out_fd != -1)
		close(*out_fd);
	flags = O_WRONLY | O_CREAT;
	if (redir->type == REDIR_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	*out_fd = open(redir->filename, flags, 0644);
	if (*out_fd < 0)
	{
		perror(redir->filename);
		if (in_fd != -1)
			close(in_fd);
		return (-1);
	}
	return (0);
}

void	apply_redirections(int in_fd, int out_fd)
{
	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
}

int	handle_redirections(t_command *cmd, t_shell *shell)
{
	t_redirect	*redir;
	int			in_fd;
	int			out_fd;

	redir = cmd->redirects;
	in_fd = -1;
	out_fd = -1;
	while (redir)
	{
		if (redir->type == REDIR_IN || redir->type == REDIR_HEREDOC)
		{
			if (handle_input_redirect(redir, &in_fd, out_fd, shell) < 0)
				return (-1);
		}
		else if (redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
		{
			if (handle_output_redirect(redir, &out_fd, in_fd) < 0)
				return (-1);
		}
		redir = redir->next;
	}
	apply_redirections(in_fd, out_fd);
	return (0);
}

int	handle_heredoc(const char *delimiter, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	char	*expanded;

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
