/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:39 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 14:30:19 by ancengiz         ###   ########.fr       */
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

void	free_paths_array(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
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
