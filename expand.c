/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 14:32:52 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 04:08:18 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_exit_code(int *index, t_shell *shell)
{
	(*index)++;
	return (ft_itoa(shell->last_exit_code));
}

static char	*expand_env_variable(char *input, int *index, t_env *env_list)
{
	int		start;
	int		len;
	char	*key;
	char	*value;
	char	*result;

	start = *index;
	while (ft_isalnum(input[*index]) || input[*index] == '_')
		(*index)++;
	len = *index - start;
	key = ft_strndup(input + start, len);
	value = get_env_value(env_list, key);
	if (value)
		result = ft_strdup(value);
	else
		result = ft_strdup("");
	free(key);
	return (result);
}

static char	*expand_variable(char *input, int *index, t_env *env_list,
		t_shell *shell)
{
	(*index)++;
	if (input[*index] == '?')
		return (expand_exit_code(index, shell));
	else if (ft_isalnum(input[*index]) || input[*index] == '_')
		return (expand_env_variable(input, index, env_list));
	else
		return (ft_strdup("$"));
}

static char	*process_expansion(char *input, int *i, t_shell *shell,
		char *result)
{
	char	*expanded;

	expanded = expand_variable(input, i, shell->env_list, shell);
	result = append_string(result, expanded);
	free(expanded);
	return (result);
}

char	*process_word_with_expansion(char *input, int start, int end,
		t_shell *shell)
{
	char	*result;
	int		i;

	result = ft_strdup("");
	i = start;
	while (i < end)
	{
		if (input[i] == '$')
			result = process_expansion(input, &i, shell, result);
		else
			result = process_regular_char(input, &i, result);
	}
	return (result);
}
