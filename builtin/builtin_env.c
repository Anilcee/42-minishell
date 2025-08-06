/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:35:42 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:43:03 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**copy_env(char **envp)
{
	int		i;
	int		count;
	char	**new_env;

	i = 0;
	count = 0;
	while (envp[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 1));
	while (i < count)
	{
		new_env[i] = ft_strdup(envp[i]);
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

static char	**replace_existing_env(char **envp, char *input, int key_len)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], input, key_len) == 0 && envp[i][key_len] == '=')
		{
			free(envp[i]);
			envp[i] = ft_strdup(input);
			return (envp);
		}
		i++;
	}
	return (NULL);
}

static char	**create_new_envp_with_var(char **envp, char *input, int env_count)
{
	char	**new_envp;
	int		j;

	new_envp = malloc(sizeof(char *) * (env_count + 2));
	j = 0;
	while (j < env_count)
	{
		new_envp[j] = envp[j];
		j++;
	}
	new_envp[env_count] = ft_strdup(input);
	new_envp[env_count + 1] = NULL;
	free(envp);
	return (new_envp);
}

char	**add_envp(char **envp, char *input)
{
	int		i;
	char	*equal;
	int		key_len;
	char	**result;

	i = 0;
	equal = ft_strchr(input, '=');
	if (!equal)
		return (envp);
	key_len = equal - input;
	result = replace_existing_env(envp, input, key_len);
	if (result)
		return (result);
	while (envp[i])
		i++;
	return (create_new_envp_with_var(envp, input, i));
}

int	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}
