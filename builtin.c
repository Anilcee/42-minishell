#include "minishell.h"

int builtin_cd(t_command *cmd,t_env *env_list)
{
    char *target_path;
    if (!cmd || !cmd->args)
        return 1;
    if (cmd->args[2])
    {
        write(STDERR_FILENO, "minishell: cd: too many arguments\n", 34);
        return 1;
    }
    if(!cmd->args[1])
        target_path = get_env_value(env_list,"HOME");
    else
        target_path=cmd->args[1];
    if (chdir(target_path) != 0)
    {
        write(STDERR_FILENO, "minishell: cd: No such file or directory\n", 41);
        return 1;
    }
    return 0;
}

int builtin_pwd() 
{
    char cwd[4096]; 

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

int builtin_echo(t_command *cmd)
{
    int i = 1;
    int newline = 1;

    if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }
    while (cmd->args[i])
    {
        printf("%s", cmd->args[i]);
        if (cmd->args[i + 1])
            printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
    return (0);
}

int builtin_env(char** envp) 
{
    int i;

    i = 0;
    while (envp[i]) 
    {
        printf("%s\n", envp[i]);
        i++;
    }
    return (0);
}

void builtin_history(char *line)
{
    static t_history *head = NULL;
    static int count = 0;
    if (line != NULL)
    {
        t_history *new_node = malloc(sizeof(t_history));
        if (!new_node)
            return; 
        new_node->line = ft_strdup(line);
        new_node->next = NULL;
        if (!head)
        {
            head = new_node;
        }
        else
        {
            t_history *temp = head;
            while (temp->next)
                temp = temp->next;
            temp->next = new_node;
        }
        count++;
        add_history(line);
    }
    else
    {
        t_history *temp = head;
        int i = 1;
        while (temp)
        {
            printf("%d %s\n", i++, temp->line);
            temp = temp->next;
        }
    }
}

int is_valid_identifier(const char *str)
{
    int i = 0;
    
    if (!str || !str[0])
        return 0;
    
    // İlk karakter harf veya _ olmalı
    if (!((str[0] >= 'a' && str[0] <= 'z') || 
          (str[0] >= 'A' && str[0] <= 'Z') || 
          str[0] == '_'))
        return 0;
    
    i = 1;
    // Sonraki karakterler harf, rakam veya _ olmalı
    while (str[i] && str[i] != '=')
    {
        if (!((str[i] >= 'a' && str[i] <= 'z') || 
              (str[i] >= 'A' && str[i] <= 'Z') || 
              (str[i] >= '0' && str[i] <= '9') || 
              str[i] == '_'))
            return 0;
        i++;
    }
    return 1;
}

int builtin_export(t_command *cmd, char*** envp, t_env **env_list)
{
    char *input = cmd->args[1];
    if (!input)
    {
        printf("VAR=değer\n");
        return 0;
    }
    
    // Geçerli identifier kontrolü
    if (!is_valid_identifier(input))
    {
        write(STDERR_FILENO, "minishell: export: `", 20);
        write(STDERR_FILENO, input, ft_strlen(input));
        write(STDERR_FILENO, "': not a valid identifier\n", 26);
        return 1;
    }
    
    add_env_list(env_list, input);
    *envp = add_envp(*envp, input); 
    return 0;
}

int builtin_unset(t_command *cmd, char ***envp, t_env **env_list)
{
    char *key = cmd->args[1];
    if (!key)
    {
        return 0;  // Bash behavior: unset without args returns 0
    }
    
    unset_from_env_list(env_list, key);
    *envp = unset_from_envp(*envp, key);
    return 0;
}

char **copy_env(char **envp)
{
    int i = 0;
    int count = 0;
    while (envp[count])
        count++;
    
    char **new_env = malloc(sizeof(char *) * (count + 1));
    while (i < count)
    {
        new_env[i] = ft_strdup(envp[i]);
        i++;
    }
    new_env[i] = NULL;
    return new_env;
}

char **add_envp(char **envp, char *input)
{
    int i;
    char *equal;
    int key_len;
    i = 0;
    equal = strchr(input, '=');
    if (!equal)
        return envp;
    key_len = equal - input;
    while (envp[i])
    {
        if (ft_strncmp(envp[i], input, key_len) == 0 && envp[i][key_len] == '=')
        {
            free(envp[i]);
            envp[i] = ft_strdup(input);
            return envp;
        }
        i++;
    }
    char **new_envp = malloc(sizeof(char*) * (i + 2));
    int j = 0;
    while (j < i)
    {
        new_envp[j] = envp[j];
        j++;
    }
    new_envp[i] = ft_strdup(input);
    new_envp[i + 1] = NULL;
    free(envp);
    return new_envp;
}

t_env *add_env_list(t_env **head, char *input)
{
    char *equal_sign;
    int key_len;
    char *key;
    char *value;

    if (!input || !head)
        return NULL;
    equal_sign = ft_strchr(input, '=');
    if (!equal_sign)
        return NULL;
    key_len = equal_sign - input;
    key = ft_strndup(input, key_len);
    value = ft_strdup(equal_sign + 1);
    if (!key || !value)
        return NULL;
    t_env *current = *head;
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            free(current->value);
            current->value = value;
            free(key);
            return current;
        }
        current = current->next;
    }
    t_env *new_node = malloc(sizeof(t_env));
    if (!new_node)
        return NULL;
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        current = *head;
        while (current->next)
            current = current->next;
        current->next = new_node;
    }
    return new_node;
}

t_env *envp_to_list(char **envp)
{
    t_env *head = NULL;
    t_env *current = NULL;
    int i = 0;
    char *equal_sign;
    int key_len;
    char *key;
    char *value;

    while (envp[i])
    {
        equal_sign = ft_strchr(envp[i], '=');
        if (!equal_sign)
            return NULL; 
        key_len = equal_sign - envp[i];
        key = ft_strndup(envp[i], key_len);
        value = ft_strdup(equal_sign + 1);
        if (!key || !value)
            return NULL;
        t_env *new_node = malloc(sizeof(t_env));
        new_node->key = key;
        new_node->value = value;
        new_node->next = NULL;
        if (!head)
        {
            head = new_node;
            current = new_node;
        }
        else
        {
            current->next = new_node;
            current = new_node;
        }
        i++;
    }
    return head;
}

void unset_from_env_list(t_env **head, const char *key)
{
    t_env *current = *head;
    t_env *prev = NULL;

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
            return;
        }
        prev = current;
        current = current->next;
    }
}

char **unset_from_envp(char **envp, const char *key)
{
    int i = 0;
    int  j = 0;
    int count = 0;
    int key_len = ft_strlen(key);
    char **new_envp;

    while (envp[count])
        count++;
    new_envp = malloc(sizeof(char *) * (count + 1));
    while (envp[i])
    {
        if (!(ft_strncmp(envp[i], key, key_len) == 0 && envp[i][key_len] == '='))
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
    free(envp);
    return new_envp;
}
int builtin_exit(t_command *cmd)
{
    int exit_code;
    printf("exit\n");
    if(!cmd->args[1])
        exit (0);
    
    if (!is_num(cmd->args[1]))
    {
        write(STDERR_FILENO, "minishell: exit: numeric argument required\n", 43);
        exit(2);
    }
    if (cmd->args[2])
    {
        write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
        return 1;
    }
    exit_code = atoi(cmd->args[1]) % 256;
    exit(exit_code);
}