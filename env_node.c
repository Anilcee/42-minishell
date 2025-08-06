/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_node.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:26:09 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 13:16:12 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*create_env_node_from_envp(char *env_str)
{
	char	*equal_sign;
	int		key_len;
	t_env	*new_node;

	equal_sign = ft_strchr(env_str, '=');
	if (!equal_sign)
		return (NULL);
	key_len = equal_sign - env_str;
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_strndup(env_str, key_len);
	new_node->value = ft_strdup(equal_sign + 1);
	new_node->next = NULL;
	return (new_node);
}

t_env	*update_existing_env_node(t_env *current, char *key, char *value)
{
	free(current->value);
	current->value = value;
	free(key);
	return (current);
}

t_env	*create_new_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

void	append_env_node(t_env **head, t_env *new_node)
{
	t_env	*current;

	if (*head == NULL)
		*head = new_node;
	else
	{
		current = *head;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
}

void	add_env_node_to_list(t_env **head, t_env **current, t_env *new_node)
{
	if (*head == NULL)
		*head = new_node;
	else
		(*current)->next = new_node;
	*current = new_node;
}
