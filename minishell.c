#include "minishell.h"

int execute_command(t_command *cmds, t_shell *shell)
{



    int saved_stdout;
    int saved_stdin;
    if (!cmds || !shell || !shell->envp || !shell->env_list)
        return 1;
    
    // Check if the command failed in parsing (due to redirection failure)
    if (!cmds->args || !cmds->args[0])
    {
        shell->last_exit_code = 1;
        return 1;
    }
    
    if (has_pipe(cmds)) 
    {
        shell->last_exit_code = execute_piped_commands(cmds, shell->envp);
        return 1; 
    }
    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin = dup(STDIN_FILENO);
    if (handle_redirections(cmds) < 0)
    {
        shell->last_exit_code = 1;
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stdin);
        return 1;
    }
    if (ft_strcmp(cmds->args[0], "cd") == 0)
        shell->last_exit_code = builtin_cd(cmds,shell->env_list);
    else if (ft_strcmp(cmds->args[0], "pwd") == 0)
        shell->last_exit_code = builtin_pwd();
    else if (ft_strcmp(cmds->args[0], "env") == 0)
    {
        shell->last_exit_code = builtin_env(shell->envp);
    }
        
    else if (ft_strcmp(cmds->args[0], "echo") == 0)
    {
        builtin_echo(cmds);
        shell->last_exit_code = 0;
    }
    else if (ft_strcmp(cmds->args[0], "unset") == 0)
    {
        shell->last_exit_code = builtin_unset(cmds, &shell->envp, &shell->env_list);
    }
    else if (ft_strcmp(cmds->args[0], "export") == 0)
    {
        shell->last_exit_code = builtin_export(cmds, &shell->envp, &shell->env_list);
    }
    else if (ft_strcmp(cmds->args[0], "exit") == 0)
    {
        int ret = builtin_exit(cmds);
        if (ret == 1)
        {
            shell->last_exit_code = 1;
            return 1;
        }
    }
    else
    {
       int external_result = external_commands(cmds, shell->envp);
       if (external_result == -1)
        {
            write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
            write(STDERR_FILENO, ": command not found\n", 20);
            shell->last_exit_code = 127;
        }
        else if (external_result == -2)
        {
            write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
            write(STDERR_FILENO, ": No such file or directory\n", 28);
            shell->last_exit_code = 127;
        }
        else if (external_result == -3)
        {
            write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
            write(STDERR_FILENO, ": Is a directory\n", 17);
            shell->last_exit_code = 126;
        }
        else if (external_result == -4)
        {
            write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
            write(STDERR_FILENO, ": Permission denied\n", 20);
            shell->last_exit_code = 126;
        }
        else
        {
            shell->last_exit_code = external_result;
        }
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
        t_token *tokens = tokenize(input, shell.env_list, &shell);
        t_command *cmds = parse_tokens(tokens);
        
        if (!execute_command(cmds, &shell))
        {
            break;
        }
    }

    return 0;
}
