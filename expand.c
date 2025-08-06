/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 14:32:52 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 14:40:01 by ancengiz         ###   ########.fr       */
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

char	*expand_variable(char *input, int *index, t_env *env_list,
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

char	*process_word_with_expansion(char *input, int start, int end,
		t_shell *shell)
{
	char	*result;
	int		i;
	char	*expanded;
	char	temp_str[2];

	result = ft_strdup("");
	i = start;
	while (i < end)
	{
		if (input[i] == '$')
		{
			expanded = expand_variable(input, &i, shell->env_list, shell);
			result = append_string(result, expanded);
			free(expanded);
		}
		else
		{
			temp_str[0] = input[i];
			temp_str[1] = '\0';
			result = append_string(result, temp_str);
			i++;
		}
	}
	return (result);
}
