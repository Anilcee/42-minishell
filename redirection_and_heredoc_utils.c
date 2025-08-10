/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_and_heredoc_utils.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:49:39 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/08 21:13:54 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	preprocess_heredocs(t_exec_context *exec)
{
	t_command	*cmd;
	t_redirect	*redir;

	cmd = exec->all_cmds;
	while (cmd)
	{
		redir = cmd->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->processed_fd == -1)
			{
				redir->processed_fd = handle_heredoc(redir->filename, exec);
				if (redir->processed_fd < 0)
					return (-1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

int	handle_input_redirect(t_redirect *redir, int *in_fd, int out_fd,
							t_exec_context *exec)
{
	(void)exec;
	if (*in_fd != -1)
		close(*in_fd);
	if (redir->type == REDIR_HEREDOC)
	{
		if (redir->processed_fd != -1)
		{
			*in_fd = dup(redir->processed_fd);
		}
		else
		{
			perror("heredoc not preprocessed");
			return (-1);
		}
	}
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

int	handle_redirections(t_exec_context *exec)
{
	t_redirect	*redir;
	int			in_fd;
	int			out_fd;

	redir = exec->current->redirects;
	in_fd = -1;
	out_fd = -1;
	while (redir)
	{
		if (redir->type == REDIR_IN || redir->type == REDIR_HEREDOC)
		{
			if (handle_input_redirect(redir, &in_fd, out_fd, exec) < 0)
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
