/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 14:30:24 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 18:10:28 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirects(t_redirect *head)
{
	t_redirect	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->filename)
			free(temp->filename);
		free(temp);
	}
}

void	free_commands(t_command *head)
{
	t_command	*temp;
	int			i;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->args)
		{
			i = 0;
			while (temp->args[i])
			{
				free(temp->args[i]);
				i++;
			}
			free(temp->args);
		}
		if (temp->redirects)
			free_redirects(temp->redirects);
		free(temp);
	}
}

static void	free_env_list(t_env *env_list)
{
	t_env	*temp;

	while (env_list)
	{
		temp = env_list;
		env_list = env_list->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}

void	free_tokens_and_commands(t_token *tokens, t_command *cmds)
{
	free_tokens(tokens);
	free_commands(cmds);
}

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env_list)
		free_env_list(shell->env_list);
	if (shell->envp)
		free_array(shell->envp);
	shell->env_list = NULL;
	shell->last_exit_code = 0;
}
