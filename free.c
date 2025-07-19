#include "minishell.h"

void	free_paths_array(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

void	free_tokens(t_token *head)
{
	t_token	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->value)
			free(temp->value);
		free(temp);
	}
}

void	free_redirects(t_redirect *head)
{
	t_redirect	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->filename)
			free(temp->filename);
		free(temp);
	}
}

void	free_commands(t_command *head)
{
	t_command	*temp;
	int			i;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->args)
		{
			i = 0;
			while (temp->args[i])
			{
				free(temp->args[i]);
				i++;
			}
			free(temp->args);
		}
		if (temp->redirects)
			free_redirects(temp->redirects);
		free(temp);
	}
}

void free_env_list(t_env *env_list)
{
    t_env *temp;

    while (env_list) {
        temp = env_list;
        env_list = env_list->next;
        free(temp->key);    // Anahtarları serbest bırak
        free(temp->value);  // Değerleri serbest bırak
        free(temp);         // Liste elemanlarını serbest bırak
    }
}

void free_envp_array(char **envp)
{
    int i = 0;

    if (envp) {
        while (envp[i]) {
            free(envp[i]);  // Her bir string'i serbest bırak
            i++;
        }
        free(envp);  // envp dizisinin kendisini de serbest bırak
    }
}

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env_list)
		free_env_list(shell->env_list);
	if (shell->envp)
		free_envp_array(shell->envp);
	shell->env_list = NULL;
	shell->last_exit_code = 0;
}

void	free_history_list(t_history *head)
{
	t_history	*temp;

	while (head)
	{
		temp = head;
		head = head->next;
		if (temp->line)
			free(temp->line);
		free(temp);
	}
}
