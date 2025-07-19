#include "minishell.h"

int	ft_isspace(int c)
{
	if (c == ' ' || c == '\t')
		return (1);
	else
		return (0);
}

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

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

char	*expand_variable(char *input, int *index, t_env *env_list,
		t_shell *shell)
{
	char	*result;
	int		start;
	int		len;
	char	*key;
	char	*value;

	result = NULL;
	(*index)++;
	if (input[*index] == '?')
	{
		(*index)++;
		result = ft_itoa(shell->last_exit_code);
	}
	else if (ft_isalnum(input[*index]) || input[*index] == '_')
	{
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
	}
	else
		result = ft_strdup("$");
	return (result);
}

char	*process_word_with_expansion(char *input, int start, int end,
		t_shell *shell, char quote_type)
{
	char	*result;
	int		i;
	char	*expanded;
	char	temp_str[2];

	result = ft_strdup("");
	i = start;
	while (i < end)
	{
		if (input[i] == '$' && quote_type != '\'')
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
	part = process_word_with_expansion(input, start, *i, shell, quote);
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
	return (process_word_with_expansion(input, start, *i, shell, '\0'));
}

static char	*process_word_token(char *input, int *i, t_shell *shell)
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

static void	handle_special_chars(char *input, int *i, t_token **head,
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
