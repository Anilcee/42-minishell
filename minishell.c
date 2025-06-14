#include "minishell.h"

int execute_command(char **args, char ***envp) 
{
    if (!args[0])
        return 1;
    if (has_pipe(args)) 
    {
        char ***commands = split_by_pipe(args);
        execute_piped_commands(commands);
        int i = 0;
        while (commands[i]) 
        {
            free(commands[i]);
            i++;
        }
        free(commands);
        return 1; 
    }
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
            printf("minishell: %s: command not found\n", args[0]);
    }
    return 1;
}

void sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);            
    rl_on_new_line();              
    rl_replace_line("", 0);       
    rl_redisplay();               
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    printf("\033[0;31m");
    char *input;
    char **args;
    char **env = copy_env(envp);
    signal(SIGINT, sigint_handler);
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
        t_token *tokens =tokenize(input);
        while(tokens)
        {
            printf("Token: '%s' - Tip: %d\n", tokens->value, tokens->t_type);
            tokens=tokens->next;
        }
        if (!execute_command(args, &env))
        {
            break;
        }
    }
    printf("\033[0m");
    return 0;
}
