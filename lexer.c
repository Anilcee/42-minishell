/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:36:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:36:43 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token_type	get_token_type(char *value)
{
	if (ft_strcmp(value, "|") == 0)
		return (T_PIPE);
	else if (ft_strcmp(value, "<") == 0)
		return (T_REDIRECT_IN);
	else if (ft_strcmp(value, ">") == 0)
		return (T_REDIRECT_OUT);
	else if (ft_strcmp(value, ">>") == 0)
		return (T_APPEND);
	else if (ft_strcmp(value, "<<") == 0)
		return (T_HEREDOC);
	else
		return (T_WORD);
}

char	*extract_word(char *input, int start, int end)
{
	int		len;
	char	*word;

	len = end - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	ft_strlcpy(word, &input[start], len + 1);
	return (word);
}

char	*process_word_no_expansion(char *input, int start, int end)
{
	char	*result;
	int		i;
	char	temp_str[2];

	result = ft_strdup("");
	i = start;
	while (i < end)
	{
		temp_str[0] = input[i];
		temp_str[1] = '\0';
		result = append_string(result, temp_str);
		i++;
	}
	return (result);
}

void	handle_special_chars(char *input, int *i, t_token **head,
		t_token **tail)
{
	int		start;
	char	*word;

	start = *i;
	if (input[*i] == input[*i + 1])
	{
		word = extract_word(input, start, *i + 2);
		add_token_to_list(head, tail, word, '\0');
		(*i) += 2;
	}
	else
	{
		word = extract_word(input, start, *i + 1);
		add_token_to_list(head, tail, word, '\0');
		(*i)++;
	}
}
