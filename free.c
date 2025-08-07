/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:39 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/07 18:09:26 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_and_exit(t_exec_context *ctx, int exit_code)
{
	if (ctx->shell)
		free_shell(ctx->shell);
	if (ctx->all_cmds)
		free_commands(ctx->all_cmds);
	if (ctx->all_tokens)
		free_tokens(ctx->all_tokens);
	rl_clear_history();
	exit(exit_code);
}

void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

void	free_tokens(t_token *head)
{
	t_token	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->value)
			free(temp->value);
		free(temp);
	}
}

void	free_history_list(t_history *head)
{
	t_history	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->line)
			free(temp->line);
		free(temp);
	}
}
