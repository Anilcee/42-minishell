#include "minishell.h"

t_command *create_new_command() 
{
    t_command *cmd = malloc(sizeof(t_command));
    cmd->args = NULL;
    cmd->quote_type = NULL;
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->append = 0;
    cmd->heredoc = 0;
    cmd->next = NULL;
    return cmd;
}

void add_arg(t_command *cmd, char *arg, char quote_type) 
{
    int i = 0;
    int j = 0;
    while (cmd->args && cmd->args[i])
        i++;
    char **new_args = malloc(sizeof(char *) * (i + 2));
    char  *new_quotes = malloc(sizeof(char) * (i + 2));
    while (j < i)
    {
        new_args[j] = cmd->args[j];
        new_quotes[j] = cmd->quote_type[j];
        j++;
    }
    new_args[i] = ft_strdup(arg);
    new_quotes[i] = quote_type;
    new_args[i + 1] = NULL;
    new_quotes[i + 1] = '\0';
    free(cmd->args);
    free(cmd->quote_type);
    cmd->args = new_args;
    cmd->quote_type = new_quotes;
}

t_command *parse_tokens(t_token *tokens) 
{
    t_command *head = NULL;
    t_command *current = NULL;

    while (tokens) 
    {
        if (!current) 
        {
            current = create_new_command();
            if (!head)
                head = current;
        }
        if (tokens->t_type == T_WORD) 
        {
            add_arg(current, tokens->value, tokens->quote_type);
        }
        else if (tokens->t_type == T_REDIRECT_IN) 
        {
            tokens = tokens->next;
            if (tokens)
                current->infile = ft_strdup(tokens->value);
        }
        else if (tokens->t_type == T_REDIRECT_OUT) 
        {
            tokens = tokens->next;
            if (tokens) 
            {
                current->outfile = ft_strdup(tokens->value);
                current->append = 0;
            }
        }
        else if (tokens->t_type == T_APPEND) 
        {
            tokens = tokens->next;
            if (tokens) 
            {
                current->outfile = ft_strdup(tokens->value);
                current->append = 1;
            }
        }
        else if (tokens->t_type == T_HEREDOC) 
        {
            tokens = tokens->next;
            if (tokens) 
            {
                current->infile = ft_strdup(tokens->value);
                current->heredoc = 1;
            }
        }
        else if (tokens->t_type == T_PIPE) 
        {
            current->next = create_new_command();
            current = current->next;
        }
        tokens = tokens->next;
    }
    return head;
}
