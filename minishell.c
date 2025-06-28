#include "minishell.h"

int execute_command(t_command *cmds, char ***envp, t_env **env_list) 
{
    int saved_stdout;
    int saved_stdin;
    if (!cmds || !cmds->args || !cmds->args[0] || !envp || !(*envp) || !env_list || !(*env_list))
        return 1;
    if (has_pipe(cmds)) 
    {
        execute_piped_commands(cmds);
        return 1; 
    }
    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin = dup(STDIN_FILENO);
    if (handle_redirections(cmds) < 0)
    {
        printf("Redirection error\n");
        return 1;
    }
    if (strcmp(cmds->args[0], "cd") == 0)
        builtin_cd(cmds);
    else if (strcmp(cmds->args[0], "pwd") == 0)
        builtin_pwd();
    else if (strcmp(cmds->args[0], "env") == 0)
        builtin_env(*envp);
    else if (strcmp(cmds->args[0], "echo") == 0)
        builtin_echo(cmds, *env_list);
    else if (strcmp(cmds->args[0], "history") == 0)
        builtin_history(NULL);
    else if (strcmp(cmds->args[0], "unset") == 0)
        *envp = builtin_unset(cmds, *envp, env_list);
    else if (strcmp(cmds->args[0], "export") == 0)
        *envp = builtin_export(cmds, *envp, env_list);
    else if (strcmp(cmds->args[0], "exit") == 0)
    {
        printf("exit\n");
        return 0;
    }
    else
    {
        if (!external_commands(cmds, *envp))
            printf("minishell: %s: command not found\n", cmds->args[0]);
    }
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
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
    t_env *env_list = NULL;
    char **env = copy_env(envp);
    env_list = envp_to_list(env);
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
            builtin_history(input);
        t_token *tokens =tokenize(input);
        t_command *cmds = parse_tokens(tokens);
        /*int i = 0;
        while (tokens)
        {
            printf("token %d %s  token type: %d  \n",i,tokens->value,tokens->t_type);
            tokens=tokens->next;
            i++;
        }*/
        
        if (!execute_command(cmds, &env, &env_list))
        {
            break;
        }
    }
    printf("\033[0m");
    return 0;
}
