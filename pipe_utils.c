/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:47:07 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/05 14:37:59 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	has_pipe(t_command *cmds)
{
	return (cmds && cmds->next != NULL);
}

void	setup_child_pipes(int prev_fd, int *fd, t_command *current)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (current->next)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
}

int	setup_pipe_if_needed(t_command *current, int *fd)
{
	if (current->next)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (-1);
		}
	}
	return (0);
}

void	add_pid(t_pid_list **head, pid_t pid)
{
	t_pid_list	*new_node;
	t_pid_list	*temp;

	new_node = malloc(sizeof(t_pid_list));
	if (!new_node)
		return ;
	new_node->pid = pid;
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

void	wait_and_free_pids(t_pid_list *head)
{
	t_pid_list	*temp;

	while (head)
	{
		waitpid(head->pid, NULL, 0);
		temp = head;
		head = head->next;
		free(temp);
	}
}
