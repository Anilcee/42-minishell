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
    if (strcmp(value, "|") == 0)
        return T_PIPE;
    else if (strcmp(value, "<") == 0)
        return T_REDIRECT_IN;
    else if (strcmp(value, ">") == 0)
        return T_REDIRECT_OUT;
    else if (strcmp(value, ">>") == 0)
        return T_APPEND;
    else if (strcmp(value, "<<") == 0)
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
    strncpy(word, &input[start], len);
    word[len] = '\0';
    return word;
}

t_token *tokenize(char *input)
{
    t_token *head = NULL;
    t_token *tail = NULL;
    int start = 0;
    int i = 0;
    char quote;
    char *word;

    while (input[i])
    {
        while (input[i] && ft_isspace(input[i]))
        {
            i++;
        }
        start = i;
        
        if (!input[i])
            break;
        else if (is_quote(input[i]))
        {
            quote = input[i];
            i++;
            start = i;
            while (input[i] && input[i] != quote)
                i++;
            word = extract_word(input, start, i);
            add_token_to_list(&head, &tail, word, quote);
            if (input[i] == quote)
                i++;
        }
        else
        {
            while (input[i] && !ft_isspace(input[i]))
                i++;
            if (i > start)
            {
                char *word = extract_word(input, start, i);
                add_token_to_list(&head, &tail, word, '\0');
            }
        }
    }
    return head;
}
