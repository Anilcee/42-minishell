#include "minishell.h"

int builtin_cd(t_command *cmd) 
{
    if (!cmd || !cmd->args || !cmd->args[1]) 
    {
        printf("minishell: cd: eksik argüman\n");
        return 1;
    }
    if (chdir(cmd->args[1]) != 0) 
    {
        printf("minishell: cd: %s: No such file or directory\n", cmd->args[1]);
        return 1;
    }
    return 0;
}

void builtin_pwd() 
{
   char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("getcwd hatası");
    }   
}

void builtin_echo(t_command *cmd)
{
    int i = 1;
    int newline = 1;

    if (cmd->args[i] && strcmp(cmd->args[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }
    while (cmd->args[i])
    {
        if (strcmp(cmd->args[i], "$$") == 0)
            printf("%d", getpid());
        else
            printf("%s", cmd->args[i]);
        if (cmd->args[i + 1])
            printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
}

void builtin_env(char** envp) 
{
    int i;

    i = 0;
    while (envp[i]) 
    {
        printf("%s\n", envp[i]);
        i++;
    }
}

void builtin_history(char *line) 
{
    static char *history[1000];
    static int count = 0;
    int i = 0;
    if (line != NULL)
    {
        if (count < 1000)
        {
            history[count] = strdup(line);
            add_history(line);
            count++;
        }
    }
    else
    {
        while(i < count)
        {
            printf("%d %s\n", i + 1, history[i]);
            i++;
        }
    }
}

char **builtin_export(char* input, char** envp)
{
    if (!input)
    {
        printf("VAR=değer\n");
        return envp;
    }
    int count = 0;
    while (envp[count])
        count++;
    char **new_envp = malloc(sizeof(char*) * (count + 2));
    if (!new_envp)
        return envp;
    int i = 0;
    while (i < count)
    {
        new_envp[i] = envp[i];
        i++;
    }
    new_envp[count] = ft_strdup(input); 
    new_envp[count + 1] = NULL;
    free(envp); 
    return new_envp;  
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
        new_env[i] = strdup(envp[i]);
        i++;
    }
    new_env[i] = NULL;
    return new_env;
}