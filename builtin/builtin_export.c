/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 10:37:12 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/12 16:32:54 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_export_with_value(char *arg, char ***envp, t_env **env_list)
{
	add_env_list(env_list, arg);
	*envp = add_envp(*envp, arg);
	return (0);
}

static int	handle_export_without_value(char *arg, char ***envp,
		t_env **env_list)
{
	char	*existing_value;
	t_env	*current;

	(void)envp;
	existing_value = get_env_value(*env_list, arg);
	if (existing_value)
		return (0);
	current = malloc(sizeof(t_env));
	if (!current)
		return (0);
	current->key = ft_strdup(arg);
	current->value = NULL;
	current->next = NULL;
	append_env_node(env_list, current);
	return (0);
}

static int	export_single_var(char *arg, char ***envp, t_env **env_list)
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
