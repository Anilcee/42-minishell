/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_history.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 02:39:23 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 02:39:36 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_to_history_list(t_history **head, char *line)
{
	t_history	*new_node;
	t_history	*temp;

	new_node = malloc(sizeof(t_history));
	if (!new_node)
		return ;
	new_node->line = ft_strdup(line);
	new_node->next = NULL;
	if (!*head)
		*head = new_node;
	else
	{
		temp = *head;
		while (temp->next)
			temp = temp->next;
		temp->next = new_node;
	}
}

void	builtin_history(char *line)
{
	static t_history	*head;
	static int			count;

	head = NULL;
	count = 0;
	if (line == NULL)
	{
		if (head)
		{
			free_history_list(head);
			head = NULL;
			count = 0;
		}
		return ;
	}
	add_to_history_list(&head, line);
	count++;
	add_history(line);
}
