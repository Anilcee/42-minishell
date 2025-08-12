/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:47:24 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 16:24:12 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parse_env_input(char *input, char **key, char **value)
{
	char	*equal_sign;
	int		key_len;

	equal_sign = ft_strchr(input, '=');
	if (!equal_sign)
		return (0);
	key_len = equal_sign - input;
	*key = ft_strndup(input, key_len);
	*value = ft_strdup(equal_sign + 1);
	if (!*key || !*value)
		return (0);
	return (1);
}

t_env	*add_env_list(t_env **head, char *input)
{
	char	*key;
	char	*value;
	t_env	*current;
	t_env	*new_node;

	if (!parse_env_input(input, &key, &value))
		return (*head);
	current = *head;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = value;
			free(key);
			return (current);
		}
		if (!current->next)
			break ;
		current = current->next;
	}
	new_node = create_new_env_node(key, value);
	append_env_node(head, new_node);
	return (new_node);
}

t_env	*envp_to_list(char **envp)
{
	t_env	*head;
	t_env	*current;
	t_env	*new_node;
	int		i;

	head = NULL;
	current = NULL;
	i = 0;
	while (envp[i])
	{
		new_node = create_env_node_from_envp(envp[i]);
		add_env_node_to_list(&head, &current, new_node);
		i++;
	}
	return (head);
}

char	**env_list_to_array(t_env *env_list, int count)
{
	char	**arr;
	int		i;
	char	*key_eq;
	char	*tmp;

	arr = malloc(sizeof(char *) * (count + 1));
	i = 0;
	while (env_list)
	{
		key_eq = ft_strjoin(env_list->key, "=\"");
		tmp = ft_strjoin(key_eq, env_list->value);
		free(key_eq);
		arr[i] = ft_strjoin(tmp, "\"");
		free(tmp);
		env_list = env_list->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

char	*get_path_env(t_shell *shell)
{
	int	i;

	if (shell->env_list)
		return (get_env_value(shell->env_list, "PATH"));
	else
	{
		i = -1;
		while (shell->envp && shell->envp[++i])
		{
			if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
				return (shell->envp[i] + 5);
		}
	}
	return (NULL);
}
