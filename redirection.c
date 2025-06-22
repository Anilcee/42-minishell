#include "minishell.h"

int handle_input_redirection(t_command *cmd)
{
    if (!cmd->infile)
        return -1;
    int fd = open(cmd->infile, O_RDONLY);
    if (fd < 0)
    {
        perror(cmd->infile);
        return -1;
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
    return 0;
}

int handle_output_redirection(t_command *cmd)
{
    if (!cmd->outfile)
        return -1;
    int fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror(cmd->outfile);
        return -1;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return 0;
}

int handle_append_redirection(t_command *cmd)
{
    if (!cmd->outfile)
        return -1;
    int fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror(cmd->outfile);
        return -1;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return 0;
}

int handle_heredoc_redirection(t_command *cmd)
{
    if (!cmd->infile || !cmd->heredoc)
        return -1;
    char *delimiter = cmd->infile;
    int pipefd[2];
    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        return -1;
    }
    char *line = NULL;
    while (1) 
    {
        line = readline("> ");
        if (!line)  
            break;
        if (strcmp(line, delimiter) == 0) 
        {
            free(line);
            break;
        }
        write(pipefd[1], line, strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return 0;
}

int has_pipe(t_command *cmds) 
{
    return cmds && cmds->next != NULL;
}

int handle_redirections(t_command *cmd)
{
    if (cmd->infile)
    {
        if (cmd->heredoc)
        {
            if (handle_heredoc_redirection(cmd) < 0)
                return -1;
        }
        else
        {
            if (handle_input_redirection(cmd) < 0)
                return -1;
        }
    }
    if (cmd->outfile)
    {
        if (cmd->append)
        {
            if (handle_append_redirection(cmd) < 0)
                return -1;
        }
        else
        {
            if (handle_output_redirection(cmd) < 0)
                return -1;
        }
    }
    return 0;
}

void execute_piped_commands(t_command *cmds) 
{
    int fd[2];
    int prev_fd = -1;
    pid_t pid;
    t_command *current = cmds;

    while (current) 
    {
        if (current->next) 
        {
            if (pipe(fd) == -1) 
            {
                perror("pipe");
                exit(1);
            }
        }
        pid = fork();
        if (pid == -1) 
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0) 
        {
            if (prev_fd != -1) 
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (current->next) 
            {
                close(fd[0]);         
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            if (handle_redirections(current) < 0)
            {
                perror("Redirection");
                exit(1);
            }
            execvp(current->args[0], current->args);
            perror("execvp");
            exit(1);
        } 
        else 
        {
            if (prev_fd != -1)
                close(prev_fd);
            if (current->next) 
            {
                close(fd[1]);
                prev_fd = fd[0];
            }
            wait(NULL);
            current = current->next;
        }
    }
}
