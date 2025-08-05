/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 10:37:12 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/05 17:25:51 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_exported_vars(t_env *env_list)
{
	int		count;
	t_env	*tmp;
	char	**arr;
	int		i;

	count = 0;
	tmp = env_list;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	if (count == 0)
		return ;
	arr = env_list_to_array(env_list, count);
	if (!arr)
		return ;
	bubble_sort(arr, count);
	i = 0;
	while (i < count)
	{
		printf("declare -x %s\n", arr[i]);
		i++;
	}
	free_array(arr);
}

int	handle_export_with_value(char *arg, char ***envp, t_env **env_list)
{
	add_env_list(env_list, arg);
	*envp = add_envp(*envp, arg);
	return (0);
}

int	handle_export_without_value(char *arg, char ***envp, t_env **env_list)
{
	char	*existing_value;
	char	*new_var;

	existing_value = get_env_value(*env_list, arg);
	if (existing_value)
		return (0);
	new_var = ft_strjoin(arg, "=");
	if (!new_var)
		return (0);
	add_env_list(env_list, new_var);
	*envp = add_envp(*envp, new_var);
	free(new_var);
	return (0);
}

int	export_single_var(char *arg, char ***envp, t_env **env_list)
{
	if (!is_valid_identifier(arg))
	{
		print_export_error(arg);
		return (1);
	}
	if (ft_strchr(arg, '='))
		return (handle_export_with_value(arg, envp, env_list));
	else
		return (handle_export_without_value(arg, envp, env_list));
}

int	builtin_export(t_command *cmd, char ***envp, t_env **env_list)
{
	int	i;
	int	exit_code;

	if (!cmd->args[1])
	{
		print_exported_vars(*env_list);
		return (0);
	}
	exit_code = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (export_single_var(cmd->args[i], envp, env_list) != 0)
			exit_code = 1;
		i++;
	}
	return (exit_code);
}
