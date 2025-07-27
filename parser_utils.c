/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 15:56:08 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 16:20:50 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_redirect_token(t_token **tokens, t_command *current_cmd,
		t_command **head)
{
	t_redirect_type	r_type;

	r_type = -1;
	if ((*tokens)->t_type == T_REDIRECT_IN)
		r_type = REDIR_IN;
	else if ((*tokens)->t_type == T_REDIRECT_OUT)
		r_type = REDIR_OUT;
	else if ((*tokens)->t_type == T_APPEND)
		r_type = REDIR_APPEND;
	else if ((*tokens)->t_type == T_HEREDOC)
		r_type = REDIR_HEREDOC;
	(*tokens) = (*tokens)->next;
	if (*tokens && (*tokens)->t_type == T_WORD)
		add_redirect(current_cmd, r_type, (*tokens)->value);
	else
	{
		write(2, "minishell: syntax error near unexpected token\n", 45);
		free_commands(*head);
		return (0);
	}
	return (1);
}

void	handle_word_token(t_command *current_cmd, t_token *tokens)
{
	add_arg(current_cmd, tokens->value);
}

void	handle_pipe_token(t_command **current_cmd, t_token **tokens)
{
	(*current_cmd)->next = create_new_command();
	(*current_cmd) = (*current_cmd)->next;
	(*tokens) = (*tokens)->next;
}

int	handle_token(t_token **tokens, t_command **current_cmd,
		t_command **head)
{
	if ((*tokens)->t_type == T_PIPE)
	{
		if (!*current_cmd || !(*current_cmd)->args)
		{
			write(2, "minishell: syntax error near unexpected token `|'\n", 49);
			free_commands(*head);
			return (0);
		}
		handle_pipe_token(current_cmd, tokens);
		return (1);
	}
	if (!*current_cmd)
	{
		*current_cmd = create_new_command();
		if (!*head)
			*head = *current_cmd;
	}
	if ((*tokens)->t_type == T_WORD)
		handle_word_token(*current_cmd, *tokens);
	else if ((*tokens)->t_type >= T_REDIRECT_IN
		&& (*tokens)->t_type <= T_HEREDOC)
	{
		if (!handle_redirect_token(tokens, *current_cmd, head))
			return (0);
	}
	*tokens = (*tokens)->next;
	return (1);
}
