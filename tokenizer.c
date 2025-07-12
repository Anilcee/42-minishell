#include "minishell.h"

int ft_isspace(int c)
{
	if(c == ' ' || c == '\t')
		return (1);
	else 
		return 0;
}

int is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

int is_quote(char c)
{
    return (c == '\'' || c == '"');
}

t_token_type get_token_type(char *value)
{
    if (ft_strcmp(value, "|") == 0)
        return T_PIPE;
    else if (ft_strcmp(value, "<") == 0)
        return T_REDIRECT_IN;
    else if (ft_strcmp(value, ">") == 0)
        return T_REDIRECT_OUT;
    else if (ft_strcmp(value, ">>") == 0)
        return T_APPEND;
    else if (ft_strcmp(value, "<<") == 0)
        return T_HEREDOC;
    else
        return T_WORD;
}

t_token *create_token(char *word, char quote_type) 
{
    t_token *token = malloc(sizeof(t_token));
    if (!token) return NULL;
    token->value = word;
    token->t_type = get_token_type(word);
    token->quote_type = quote_type;
    token->next = NULL;
    return token;
}

void add_token_to_list(t_token **head, t_token **tail, char *word, char quote_type)
{
    t_token *new_token = create_token(word,quote_type);
    if (!*head)
        *head = new_token;
    else
        (*tail)->next = new_token;
    *tail = new_token;
}

char *extract_word(char *input, int start, int end)
{
    int len = end - start;
    char *word = malloc(len + 1);
    if (!word) return NULL;
    ft_strlcpy(word, &input[start], len + 1);
    return word;
}

char *expand_variable(char *input, int *index, t_env *env_list, t_shell *shell)
{
    char *result = NULL;
    (*index)++;
    
    if (input[*index] == '$')
    {
        (*index)++;
        result = ft_itoa(getpid());
    }

    else if (input[*index] == '?')
    {
        (*index)++;
        result = ft_itoa(shell->last_exit_code);
    }
    else if (ft_isalnum(input[*index]) || input[*index] == '_')
    {
        int start = *index;
        while (ft_isalnum(input[*index]) || input[*index] == '_')
            (*index)++;
        int len = *index - start;
        char *key = ft_strndup(input + start, len);
        char *value = get_env_value(env_list, key);
        if (value)
            result = ft_strdup(value);
        else
            result = ft_strdup("");
        free(key);
    }
    else
        result = ft_strdup("$");
    return result;
}

char *process_word_with_expansion(char *input, int start, int end, t_env *env_list, t_shell *shell, char quote_type)
{
    char *result = ft_strdup("");
    int i = start;
    
    while (i < end)
    {
        if (input[i] == '$' && quote_type != '\'')
        {
            char *expanded = expand_variable(input, &i, env_list, shell);
            char *temp = ft_strjoin(result, expanded);
            free(result);
            free(expanded);
            result = temp;
        }
        else
        {
            char temp_str[2] = {input[i], '\0'};
            char *temp = ft_strjoin(result, temp_str);
            free(result);
            result = temp;
            i++;
        }
    }
    
    return result;
}

static void handle_special_chars(char *input, int *i, t_token **head, t_token **tail)
{
    int start = *i;
    char *word;

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

t_token *tokenize(char *input, t_env *env_list, t_shell *shell)
{
    t_token *head = NULL;
    t_token *tail = NULL;
    int i = 0;

    while (input[i])
    {
        while (input[i] && ft_isspace(input[i]))
            i++;
        if (!input[i])
            break;

        if (is_special_char(input[i]))
        {
            handle_special_chars(input, &i, &head, &tail);
            continue;
        }

        char *combined_word = ft_strdup("");
        while(input[i] && !ft_isspace(input[i]) && !is_special_char(input[i]))
        {
            char *part;
            int start = i;
            if (is_quote(input[i]))
            {
                char quote = input[i];
                i++;
                start = i;
                while (input[i] && input[i] != quote)
                    i++;
                part = process_word_with_expansion(input, start, i, env_list, shell, quote);
                if (input[i] == quote)
                    i++;
            }
            else
            {
                while (input[i] && !ft_isspace(input[i]) && !is_special_char(input[i]) && !is_quote(input[i]))
                    i++;
                part = process_word_with_expansion(input, start, i, env_list, shell, '\0');
            }
            char *temp = ft_strjoin(combined_word, part);
            free(combined_word);
            free(part);
            combined_word = temp;
        }
        add_token_to_list(&head, &tail, combined_word, '\0');
    }
    return (head);
}