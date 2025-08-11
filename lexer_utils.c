/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:36:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:36:43 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_quoted_text(char *input, int *i, t_shell *shell)
{
	char	quote;
	int		start;
	char	*part;

	quote = input[*i];
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (quote == '\'')
		part = process_word_no_expansion(input, start, *i);
	else
		part = process_word_with_expansion(input, start, *i, shell);
	if (input[*i] == quote)
		(*i)++;
	return (part);
}

static char	*handle_normal_text(char *input, int *i, t_shell *shell)
{
	int	start;

	start = *i;
	while (input[*i] && !ft_isspace(input[*i]) && !is_special_char(input[*i])
		&& !is_quote(input[*i]))
		(*i)++;
	return (process_word_with_expansion(input, start, *i, shell));
}

char	*process_regular_char(char *input, int *i, char *result)
{
	char	temp_str[2];

	temp_str[0] = input[*i];
	temp_str[1] = '\0';
	result = append_string(result, temp_str);
	(*i)++;
	return (result);
}

char	*process_word_token(char *input, int *i, t_shell *shell)
{
	char	*combined_word;
	char	*part;
	int		from_quote;

	combined_word = ft_strdup("");
	from_quote = 0;
	while (input[*i] && !ft_isspace(input[*i]) && !is_special_char(input[*i]))
	{
		if (is_quote(input[*i]))
		{
			from_quote = 1;
			part = handle_quoted_text(input, i, shell);
		}
		else
			part = handle_normal_text(input, i, shell);
		combined_word = append_string(combined_word, part);
		free(part);
	}
	if (combined_word[0] == '\0' && !from_quote)
	{
		free(combined_word);
		return (NULL);
	}
	return (combined_word);
}
