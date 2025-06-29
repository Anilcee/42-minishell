#include "minishell.h"

int execute_command(t_command *cmds, t_shell *shell) 
{
    int saved_stdout;
    int saved_stdin;
    if (!cmds || !cmds->args || !cmds->args[0] || !shell || !shell->envp 
        || !shell->env_list)
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
        shell->last_exit_code = 1;
        return 1;
    }
    if (strcmp(cmds->args[0], "cd") == 0)
        shell->last_exit_code = builtin_cd(cmds);
    else if (strcmp(cmds->args[0], "pwd") == 0)
        shell->last_exit_code = builtin_pwd();
    else if (strcmp(cmds->args[0], "env") == 0)
        shell->last_exit_code = builtin_env(shell->envp);
    else if (strcmp(cmds->args[0], "echo") == 0)
    {
        builtin_echo(cmds, shell->env_list, shell);
        shell->last_exit_code = 0;
    }
    else if (strcmp(cmds->args[0], "history") == 0)
    {
        builtin_history(NULL);
        shell->last_exit_code = 0;
    }
    else if (strcmp(cmds->args[0], "unset") == 0)
    {
        shell->envp = builtin_unset(cmds, shell->envp, &shell->env_list);
        shell->last_exit_code = 0;
    }
    else if (strcmp(cmds->args[0], "export") == 0)
    {
        shell->envp = builtin_export(cmds, shell->envp, &shell->env_list);
        shell->last_exit_code = 0;
    }
    else if (strcmp(cmds->args[0], "exit") == 0)
    {
        builtin_exit(cmds);
        return 0;
    }
    else
    {
        if (!external_commands(cmds, shell->envp))
        {
            printf("minishell: %s: command not found\n", cmds->args[0]);
            shell->last_exit_code = 127;
        }
        else
            shell->last_exit_code = 0;
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
    t_shell shell;
    
    shell.last_exit_code = 0;
    shell.envp = copy_env(envp);
    shell.env_list = envp_to_list(shell.envp);
    
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
        
        if (!execute_command(cmds, &shell))
        {
            break;
        }
    }
    printf("\033[0m");
    return 0;
}
