/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 11:42:03 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:43:25 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_unset(t_command *cmd, char ***envp, t_env **env_list)
{
	char	*key;

	key = cmd->args[1];
	if (!key)
		return (0);
	unset_from_env_list(env_list, key);
	*envp = unset_from_envp(*envp, key);
	return (0);
}

void	unset_from_env_list(t_env **head, const char *key)
{
	t_env	*current;
	t_env	*prev;

	current = *head;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*head = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	copy_non_matching_env(char **envp, char **new_envp, const char *key,
		int key_len)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (envp[i])
	{
		if (!(ft_strncmp(envp[i], key, key_len) == 0
				&& envp[i][key_len] == '='))
		{
			new_envp[j] = envp[i];
			j++;
		}
		else
		{
			free(envp[i]);
		}
		i++;
	}
	new_envp[j] = NULL;
}

char	**unset_from_envp(char **envp, const char *key)
{
	int		count;
	int		key_len;
	char	**new_envp;

	count = 0;
	key_len = ft_strlen(key);
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	copy_non_matching_env(envp, new_envp, key, key_len);
	free(envp);
	return (new_envp);
}
