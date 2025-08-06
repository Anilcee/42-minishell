/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 15:56:08 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/06 20:35:37 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_redirect_type	get_redirect_type(t_token *token)
{
	if (token->t_type == T_REDIRECT_IN)
		return (REDIR_IN);
	if (token->t_type == T_REDIRECT_OUT)
		return (REDIR_OUT);
	if (token->t_type == T_APPEND)
		return (REDIR_APPEND);
	if (token->t_type == T_HEREDOC)
		return (REDIR_HEREDOC);
	return (-1);
}

static int	print_syntax_error(t_token *token, t_command **head, t_shell *shell)
{
	if (token == NULL)
		write(2, "minishell: syntax error near unexpected token `newline'\n",
			57);
	else
	{
		write(2, "minishell: syntax error near unexpected token `", 48);
		write(2, token->value, ft_strlen(token->value));
		write(2, "'\n", 2);
	}
	shell->last_exit_code = 2;
	free_commands(*head);
	return (0);
}

int	handle_redirect_token(t_token **tokens, t_command *current_cmd,
		t_command **head, t_shell *shell)
{
	t_redirect_type	r_type;

	r_type = get_redirect_type(*tokens);
	*tokens = (*tokens)->next;
	if (*tokens == NULL || (*tokens)->t_type != T_WORD)
		return (print_syntax_error(*tokens, head, shell));
	add_redirect(current_cmd, r_type, (*tokens)->value);
	return (1);
}

void	handle_word_token(t_command *current_cmd, t_token *tokens)
{
	add_arg(current_cmd, tokens->value);
}

int	handle_pipe_token(t_command **current_cmd, t_token **tokens,
		t_command **head, t_shell *shell)
{
	if (!*current_cmd || !(*current_cmd)->args)
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 51);
		shell->last_exit_code = 2;
		free_commands(*head);
		return (0);
	}
	(*current_cmd)->next = create_new_command();
	(*current_cmd) = (*current_cmd)->next;
	(*tokens) = (*tokens)->next;
	return (1);
}
