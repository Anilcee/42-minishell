#include "minishell.h"

int builtin_cd(char **args) 
{
    if (args[1] == NULL) 
    {
        printf("EKSİK");
    }
    else if ( chdir(args[1])!= 0) 
    {
        printf("YOL BULUNAMADI");
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

void builtin_echo(char** str) 
{
    int i;
    i = 0;
    if(str[0][0] == '$' && str[0][1] == '$')
    {
        printf("%d\n",getpid());
        return ;
    }
    while(str[i])
    {
        printf("%s",str[i]);
        i++;
    }
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

int execute_command(char **args, char ***envp) 
{
    if (!args[0])
        return 1;

    if (strcmp(args[0], "cd") == 0)
        builtin_cd(args);
    else if (strcmp(args[0], "pwd") == 0)
        builtin_pwd();
    else if (strcmp(args[0], "env") == 0)
        builtin_env(*envp);
    else if (strcmp(args[0], "echo") == 0)
        builtin_echo(&args[1]);
    else if (strcmp(args[0], "history") == 0)
        builtin_history(NULL);
    else if (strcmp(args[0], "export") == 0)
        *envp = builtin_export(args[1], *envp);
    else if (strcmp(args[0], "exit") == 0)
    {
        printf("exit\n");
        return 0;
    }
    else
    {
        if (!external_commands(args,*envp))
        {
            printf("minishell: %s: command not found\n", args[0]);
        }
    }
    return 1;
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    printf("\033[0;31m");
    char *input;
    char **args;
    char **env = copy_env(envp);
    while (1) 
    {  
        input = readline("minishell$ ");
        if (!input)
        {
            printf("exit\n");
            break; 
        }
        if (input != NULL)
        {
            builtin_history(input);
        }
        args = ft_split(input,' ');
        if (!execute_command(args, &env))
        {
            break;
        }
    }
    printf("\033[0m");
    return 0;
}

int external_commands(char **args,char ** envp)
{
    pid_t pid;
    char *path_env = getenv("PATH");
    if (!path_env)
        return 0;

    char **paths = ft_split(path_env, ':');
    char *program_path;
    int i = 0;

    while (paths[i])
    {
        char *temp = ft_strjoin(paths[i], "/");
        program_path = ft_strjoin(temp, args[0]);
        free(temp);
        if (access(program_path, X_OK) == 0)
            break;
        free(program_path);
        program_path = NULL;
        i++;
    }
    if (!program_path)
        return 0;
    pid = fork();
    if (pid == 0)
    {
        execve(program_path, args, envp);
        perror("execve");
        exit(1);
    }
    else
    {
        wait(NULL);
    }
    free(program_path);
    return 1;
}
