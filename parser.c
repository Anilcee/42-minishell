#include "minishell.h"

t_command	*create_new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirects = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	add_arg(t_command *cmd, char *arg)
{
	int		i;
	char	**new_args;

	i = 0;
	while (cmd->args && cmd->args[i])
		i++;
	new_args = malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return ;
	i = 0;
	while (cmd->args && cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

void	add_redirect(t_command *cmd, t_redirect_type type, char *filename)
{
	t_redirect	*new_redirect;
	t_redirect	*temp;

	new_redirect = malloc(sizeof(t_redirect));
	if (!new_redirect)
		return ;
	new_redirect->type = type;
	new_redirect->filename = ft_strdup(filename);
	new_redirect->next = NULL;
	if (!cmd->redirects)
		cmd->redirects = new_redirect;
	else
	{
		temp = cmd->redirects;
		while (temp->next)
			temp = temp->next;
		temp->next = new_redirect;
	}
}

t_command	*parse_tokens(t_token *tokens)
{
	t_command		*head;
	t_command		*current_cmd;
	t_redirect_type	r_type;

	head = NULL;
	current_cmd = NULL;
	while (tokens)
	{
		if (tokens->t_type == T_PIPE)
		{
			current_cmd->next = create_new_command();
			current_cmd = current_cmd->next;
			tokens = tokens->next;
			continue ;
		}
		if (!current_cmd)
		{
			current_cmd = create_new_command();
			if (!head)
				head = current_cmd;
		}
		if (tokens->t_type == T_WORD)
			add_arg(current_cmd, tokens->value);
		else if (tokens->t_type >= T_REDIRECT_IN && tokens->t_type <= T_HEREDOC)
		{
			r_type = -1;
			if (tokens->t_type == T_REDIRECT_IN)
				r_type = REDIR_IN;
			else if (tokens->t_type == T_REDIRECT_OUT)
				r_type = REDIR_OUT;
			else if (tokens->t_type == T_APPEND)
				r_type = REDIR_APPEND;
			else if (tokens->t_type == T_HEREDOC)
				r_type = REDIR_HEREDOC;
			tokens = tokens->next;
			if (tokens && tokens->t_type == T_WORD)
				add_redirect(current_cmd, r_type, tokens->value);
			else
			{
				write(2, "minishell: syntax error near unexpected token\n", 45);
				return (NULL);
			}
		}
		tokens = tokens->next;
	}
	return (head);
}
