/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:51 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 16:57:59 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*create_new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirects = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	add_arg(t_command *cmd, char *arg)
{
	int		i;
	char	**new_args;

	i = 0;
	while (cmd->args && cmd->args[i])
		i++;
	new_args = malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return ;
	i = 0;
	while (cmd->args && cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

void	add_redirect(t_command *cmd, t_redirect_type type, char *filename)
{
	t_redirect	*new_redirect;
	t_redirect	*temp;

	new_redirect = malloc(sizeof(t_redirect));
	if (!new_redirect)
		return ;
	new_redirect->type = type;
	new_redirect->filename = ft_strdup(filename);
	new_redirect->next = NULL;
	if (!cmd->redirects)
		cmd->redirects = new_redirect;
	else
	{
		temp = cmd->redirects;
		while (temp->next)
			temp = temp->next;
		temp->next = new_redirect;
	}
}

t_command	*parse_tokens(t_token *tokens)
{
	t_command	*head;
	t_command	*current_cmd;

	head = NULL;
	current_cmd = NULL;
	while (tokens)
	{
		if (!handle_token(&tokens, &current_cmd, &head))
			return (NULL);
	}
	if (current_cmd && !current_cmd->args && !current_cmd->redirects)
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 51);
		free_commands(head);
		return (NULL);
	}
	return (head);
}
