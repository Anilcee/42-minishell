#include "minishell.h"

int	builtin_cd(t_command *cmd, t_env *env_list)
{
	char	*target_path;

	if (!cmd || !cmd->args)
		return (1);
	if (cmd->args[2])
	{
		write(STDERR_FILENO, "minishell: cd: too many arguments\n", 34);
		return (1);
	}
	if (!cmd->args[1] || ft_strcmp(cmd->args[1], "~") == 0)
		target_path = get_env_value(env_list, "HOME");
	else
		target_path = cmd->args[1];
	if (chdir(target_path) != 0)
	{
		write(STDERR_FILENO, "minishell: cd: No such file or directory\n", 41);
		return (1);
	}
	return (0);
}

int	builtin_pwd(void)
{
	char	cwd[4096];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->args[i])
	{
		write(STDOUT_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
		if (cmd->args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}

int	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}

static void	add_to_history_list(t_history **head, char *line)
{
	t_history	*new_node;
	t_history	*temp;

	new_node = malloc(sizeof(t_history));
	if (!new_node)
		return ;
	new_node->line = ft_strdup(line);
	new_node->next = NULL;
	if (!*head)
		*head = new_node;
	else
	{
		temp = *head;
		while (temp->next)
			temp = temp->next;
		temp->next = new_node;
	}
}

void	builtin_history(char *line)
{
	static t_history	*head;
	static int			count;

	head = NULL;
	if (line != NULL)
	{
		add_to_history_list(&head, line);
		count++;
		add_history(line);
	}
}

static int	is_valid_first_char(char c)
{
	return ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_');
}

static int	is_valid_identifier_char(char c)
{
	return ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			c == '_');
}

int	is_valid_identifier(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (!is_valid_first_char(str[0]))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!is_valid_identifier_char(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_export(t_command *cmd, char ***envp, t_env **env_list)
{
	char	*input;

	input = cmd->args[1];
	if (!input)
	{
		printf("VAR=değer\n");
		return (0);
	}
	if (!is_valid_identifier(input))
	{
		write(STDERR_FILENO, "minishell: export: `", 20);
		write(STDERR_FILENO, input, ft_strlen(input));
		write(STDERR_FILENO, "': not a valid identifier\n", 26);
		return (1);
	}
	add_env_list(env_list, input);
	*envp = add_envp(*envp, input);
	return (0);
}

int	builtin_unset(t_command *cmd, char ***envp, t_env **env_list)
{
	char	*key;

	key = cmd->args[1];
	if (!key)
		return (0);
	unset_from_env_list(env_list, key);
	*envp = unset_from_envp(*envp, key);
	return (0);
}

int	builtin_exit(t_command *cmd)
{
	int	exit_code;

	printf("exit\n");
	if (!cmd->args[1])
		exit(0);
	if (!is_num(cmd->args[1]))
	{
		write(STDERR_FILENO, "minishell: exit: numeric argument required\n",
				43);
		exit(2);
	}
	if (cmd->args[2])
	{
		write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
		return (1);
	}
	exit_code = ft_atoi(cmd->args[1]) % 256;
	exit(exit_code);
}

char	**copy_env(char **envp)
{
	int		i;
	int		count;
	char	**new_env;

	i = 0;
	count = 0;
	while (envp[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 1));
	while (i < count)
	{
		new_env[i] = ft_strdup(envp[i]);
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

static char	**replace_existing_env(char **envp, char *input, int key_len)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], input, key_len) == 0 && envp[i][key_len] == '=')
		{
			free(envp[i]);
			envp[i] = ft_strdup(input);
			return (envp);
		}
		i++;
	}
	return (NULL);
}

static char	**create_new_envp_with_var(char **envp, char *input, int env_count)
{
	char	**new_envp;
	int		j;

	new_envp = malloc(sizeof(char *) * (env_count + 2));
	j = 0;
	while (j < env_count)
	{
		new_envp[j] = envp[j];
		j++;
	}
	new_envp[env_count] = ft_strdup(input);
	new_envp[env_count + 1] = NULL;
	free(envp);
	return (new_envp);
}

char	**add_envp(char **envp, char *input)
{
	int		i;
	char	*equal;
	int		key_len;
	char	**result;

	i = 0;
	equal = ft_strchr(input, '=');
	if (!equal)
		return (envp);
	key_len = equal - input;
	result = replace_existing_env(envp, input, key_len);
	if (result)
		return (result);
	while (envp[i])
		i++;
	return (create_new_envp_with_var(envp, input, i));
}

static int	parse_env_input(char *input, char **key, char **value)
{
	char	*equal_sign;
	int		key_len;

	equal_sign = ft_strchr(input, '=');
	if (!equal_sign)
		return (0);
	key_len = equal_sign - input;
	*key = ft_strndup(input, key_len);
	*value = ft_strdup(equal_sign + 1);
	if (!*key || !*value)
		return (0);
	return (1);
}

static t_env	*update_existing_env_node(t_env *current, char *key, char *value)
{
	free(current->value);
	current->value = value;
	free(key);
	return (current);
}

static t_env	*create_new_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

static void	append_env_node(t_env **head, t_env *new_node)
{
	t_env	*current;

	if (*head == NULL)
		*head = new_node;
	else
	{
		current = *head;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
}

t_env	*add_env_list(t_env **head, char *input)
{
	char	*key;
	char	*value;
	t_env	*current;
	t_env	*new_node;

	if (!input || !head)
		return (NULL);
	if (!parse_env_input(input, &key, &value))
		return (NULL);
	current = *head;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (update_existing_env_node(current, key, value));
		current = current->next;
	}
	new_node = create_new_env_node(key, value);
	if (!new_node)
		return (NULL);
	append_env_node(head, new_node);
	return (new_node);
}

static t_env	*create_env_node_from_envp(char *env_str)
{
	char	*equal_sign;
	int		key_len;
	char	*key;
	char	*value;
	t_env	*new_node;

	equal_sign = ft_strchr(env_str, '=');
	if (!equal_sign)
		return (NULL);
	key_len = equal_sign - env_str;
	key = ft_strndup(env_str, key_len);
	value = ft_strdup(equal_sign + 1);
	if (!key || !value)
		return (NULL);
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

static void	add_env_node_to_list(t_env **head, t_env **current, t_env *new_node)
{
	if (!*head)
	{
		*head = new_node;
		*current = new_node;
	}
	else
	{
		(*current)->next = new_node;
		*current = new_node;
	}
}

t_env	*envp_to_list(char **envp)
{
	t_env	*head;
	t_env	*current;
	t_env	*new_node;
	int		i;

	head = NULL;
	current = NULL;
	i = 0;
	while (envp[i])
	{
		new_node = create_env_node_from_envp(envp[i]);
		if (!new_node)
			return (NULL);
		add_env_node_to_list(&head, &current, new_node);
		i++;
	}
	return (head);
}

void	unset_from_env_list(t_env **head, const char *key)
{
	t_env	*current;
	t_env	*prev;

	current = *head;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*head = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	copy_non_matching_env(char **envp, char **new_envp,
		const char *key, int key_len)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (envp[i])
	{
		if (!(ft_strncmp(envp[i], key, key_len) == 0
				&& envp[i][key_len] == '='))
		{
			new_envp[j] = envp[i];
			j++;
		}
		else
		{
			free(envp[i]);
		}
		i++;
	}
	new_envp[j] = NULL;
}

char	**unset_from_envp(char **envp, const char *key)
{
	int		count;
	int		key_len;
	char	**new_envp;

	count = 0;
	key_len = ft_strlen(key);
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	copy_non_matching_env(envp, new_envp, key, key_len);
	free(envp);
	return (new_envp);
}
