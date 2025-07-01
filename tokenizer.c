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

char *expand_variable(char *input, int *index, t_env *env_list, t_shell *shell)
{
    char *result = NULL;
    (*index)++;
    
    if (input[*index] == '$')
    {
        (*index)++;
        int pid = getpid();
        result = malloc(12);
        sprintf(result, "%d", pid);
    }
    else if (input[*index] == '?')
    {
        (*index)++;
        result = malloc(12);
        sprintf(result, "%d", shell->last_exit_code);
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

t_token *tokenize(char *input, t_env *env_list, t_shell *shell)
{
    t_token *head = NULL;
    t_token *tail = NULL;
    int start = 0;
    int i = 0;
    char quote;
    char *word;
    char *combined_word = NULL;

    while (input[i])
    {
        while (input[i] && ft_isspace(input[i]))
            i++;
        if (!input[i])
            break;       
        combined_word = NULL;
        char current_quote_type = '\0';
        
        while (input[i] && !ft_isspace(input[i]))
        {
            start = i;
            
            if (is_quote(input[i]))
            {
                quote = input[i];
                current_quote_type = quote;
                i++;
                start = i;
                while (input[i] && input[i] != quote)
                    i++;
                word = process_word_with_expansion(input, start, i, env_list, shell, quote);
                if (combined_word == NULL)
                    combined_word = ft_strdup(word);
                else
                {
                    char *temp = ft_strjoin(combined_word, word);
                    free(combined_word);
                    combined_word = temp;
                }
                free(word);
                if (input[i] == quote)
                    i++;
            }
            else
            {
                while (input[i] && !ft_isspace(input[i]) && !is_quote(input[i]))
                    i++;
                
                if (i > start)
                {
                    word = process_word_with_expansion(input, start, i, env_list, shell, '\0');
                    if (combined_word == NULL)
                        combined_word = ft_strdup(word);
                    else
                    {
                        char *temp = ft_strjoin(combined_word, word);
                        free(combined_word);
                        combined_word = temp;
                    }
                    free(word);
                }
            }
        }
        if (combined_word)
            add_token_to_list(&head, &tail, combined_word, current_quote_type);
    }
    return head;
}