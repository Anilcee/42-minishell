/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:25 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/05 14:34:56 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*create_token(char *word, char quote_type)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = word;
	token->t_type = get_token_type(word);
	token->quote_type = quote_type;
	token->next = NULL;
	return (token);
}

void	add_token_to_list(t_token **head, t_token **tail, char *word,
		char quote_type)
{
	t_token	*new_token;

	new_token = create_token(word, quote_type);
	if (!*head)
		*head = new_token;
	else
		(*tail)->next = new_token;
	*tail = new_token;
}

t_token	*tokenize(char *input, t_shell *shell)
{
	t_token	*head;
	t_token	*tail;
	char	*word;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (input[i])
	{
		while (input[i] && ft_isspace(input[i]))
			i++;
		if (!input[i])
			break ;
		if (is_special_char(input[i]))
		{
			handle_special_chars(input, &i, &head, &tail);
			continue ;
		}
		word = process_word_token(input, &i, shell);
		if (word)
			add_token_to_list(&head, &tail, word, '\0');
	}
	return (head);
}

int	check_unclosed_quotes(const char *input)
{
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (input[i] == '\"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		i++;
	}
	return (in_single_quote || in_double_quote);
}
