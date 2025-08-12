/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_util.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:32:30 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/12 16:36:10 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	**allocate_env_array(t_env *env_list, int *count)
{
	t_env	*tmp;
	char	**arr;

	tmp = env_list;
	*count = 0;
	while (tmp)
	{
		(*count)++;
		tmp = tmp->next;
	}
	if (*count == 0)
		return (NULL);
	arr = malloc(sizeof(char *) * (*count + 1));
	if (!arr)
		return (NULL);
	return (arr);
}

static void	fill_env_array(t_env *env_list, char **arr)
{
	t_env	*tmp;
	int		i;
	char	*key_eq;
	char	*temp;

	tmp = env_list;
	i = 0;
	while (tmp)
	{
		if (tmp->value != NULL && ft_strlen(tmp->value) > 0)
		{
			key_eq = ft_strjoin(tmp->key, "=\"");
			temp = ft_strjoin(key_eq, tmp->value);
			free(key_eq);
			arr[i] = ft_strjoin(temp, "\"");
			free(temp);
		}
		else
		{
			arr[i] = ft_strdup(tmp->key);
		}
		i++;
		tmp = tmp->next;
	}
	arr[i] = NULL;
}

void	print_exported_vars(t_env *env_list)
{
	int		count;
	char	**arr;
	int		i;

	i = 0;
	arr = allocate_env_array(env_list, &count);
	if (!arr)
		return ;
	fill_env_array(env_list, arr);
	bubble_sort(arr, count);
	while (i < count)
	{
		printf("declare -x %s\n", arr[i]);
		i++;
	}
	free_array(arr);
}
