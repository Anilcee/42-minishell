/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:39 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 16:09:37 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_and_exit(t_shell *shell, t_command *all_cmds,
		t_token *all_tokens, int exit_code)
{
	if (shell)
		free_shell(shell);
	if (all_cmds)
		free_commands(all_cmds);
	if (all_tokens)
		free_tokens(all_tokens);
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

void	cleanup_resources(t_pid_list *pid_list, t_shell shell)
{
	t_pid_list	*temp;

	while (pid_list)
	{
		temp = pid_list;
		pid_list = pid_list->next;
		free(temp);
	}
	if (shell.env_list)
		free_env_list(shell.env_list);
}
