#include "minishell.h"

void remove_redirection_tokens(char **args, int i) 
{
    while (args[i + 2]) 
    {
        args[i] = args[i + 2];
        i++;
    }
    args[i] = NULL;
    args[i + 1] = NULL;
}

int handle_input_redirection(char **args, int i) 
{
    if (args[i + 1] == NULL) 
        return -1;
    int fd = open(args[i + 1], O_RDONLY);
    if (fd < 0) return -1;
    dup2(fd, STDIN_FILENO);
    close(fd);
    remove_redirection_tokens(args, i);
    return 0;
}

int handle_output_redirection(char **args, int i) 
{
    if (args[i + 1] == NULL) 
        return -1;
    int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) 
        return -1;
    dup2(fd, STDOUT_FILENO);
    close(fd);
    remove_redirection_tokens(args, i);
    return 0;
}

int handle_append_redirection(char **args, int i) 
{
    if (args[i + 1] == NULL) 
        return -1;
    int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) 
        return -1;
    dup2(fd, STDOUT_FILENO);
    close(fd);
    remove_redirection_tokens(args, i);
    return 0;
}

int handle_heredoc_redirection(char **args, int i) 
{
    if (args[i + 1] == NULL) 
        return -1;
    char *delimiter = args[i + 1];
    int pipefd[2];
    if (pipe(pipefd) == -1) 
        return -1;
    char *line = NULL;
    while (1) 
    {
        write(STDOUT_FILENO, "> ", 2);
        line = get_next_line(STDIN_FILENO);
        if (!line) break;
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') 
            line[len - 1] = '\0';
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
    remove_redirection_tokens(args, i);
    return 0;
}

int has_pipe(char **args) 
{
    int i = 0;
    while (args[i]) 
    {
        if (strcmp(args[i], "|") == 0)
            return 1;
        i++;
    }
    return 0;
}

int count_commands(char **args) 
{
    int count = 1;
    int i = 0;
    while (args[i]) 
    {
        if (strcmp(args[i], "|") == 0)
            count++;
        i++;
    }
    return count;
}

int *count_args_per_command(char **args, int command_count) 
{
    int *arg_counts = malloc(sizeof(int) * command_count);
    int i = 0, cmd_idx = 0;
    int count = 0;

    while (args[i]) 
    {
        if (strcmp(args[i], "|") == 0) 
        {
            arg_counts[cmd_idx++] = count;
            count = 0;
        } 
        else 
        {
            count++;
        }
        i++;
    }
    arg_counts[cmd_idx] = count;
    return arg_counts;
}

char ***split_by_pipe(char **args) 
{
    int command_count = count_commands(args);
    int *arg_counts = count_args_per_command(args, command_count);
    int i = 0;
    char ***commands = malloc(sizeof(char **) * (command_count + 1));
    while ( i < command_count) 
    {
        commands[i] = malloc(sizeof(char *) * (arg_counts[i] + 1));
        i++;
    }
    commands[command_count] = NULL;
    i = 0;
    int cmd_idx = 0, arg_idx = 0;
    while (args[i]) 
    {
        if (strcmp(args[i], "|") == 0) 
        {
            commands[cmd_idx][arg_idx] = NULL;
            cmd_idx++;
            arg_idx = 0;
        } 
        else 
        {
            commands[cmd_idx][arg_idx++] = args[i];
        }
        i++;
    }
    commands[cmd_idx][arg_idx] = NULL;

    free(arg_counts);
    return commands;
}

int handle_redirections(char **args) 
{
    int i = 0;
    while (args[i]) {
        if (strcmp(args[i], "<") == 0) 
        {
            if (handle_input_redirection(args, i) < 0) return -1;
            i = 0;
        } 
        else if (strcmp(args[i], ">") == 0) 
        {
            if (handle_output_redirection(args, i) < 0) return -1;
            i = 0;
        } 
        else if (strcmp(args[i], ">>") == 0) 
        {
            if (handle_append_redirection(args, i) < 0) return -1;
            i = 0;
        } 
        else if (strcmp(args[i], "<<") == 0) 
        {
            if (handle_heredoc_redirection(args, i) < 0) return -1;
            i = 0;
        } 
        else 
        {
            i++;
        }
    }
    return 0;
}

void execute_piped_commands(char ***commands) 
{
    int i = 0;
    int fd[2];
    int prev_fd = -1;

    while (commands[i] != NULL) 
    {
        if (commands[i + 1] != NULL) 
        {
            if (pipe(fd) == -1) 
            {
                perror("pipe");
                exit(1);
            }
        }
        pid_t pid = fork();
        if (pid == -1) 
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0) 
        {
            if (prev_fd != -1) 
            {
                dup2(prev_fd, 0); 
                close(prev_fd);
            }
            if (commands[i + 1]) 
            {
                close(fd[0]);         
                dup2(fd[1], 1);
                close(fd[1]);
            }
            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(1);
        } 
        else
        {
            if (prev_fd != -1)
                close(prev_fd);
            if (commands[i + 1]) 
            {
                close(fd[1]);        
                prev_fd = fd[0];     
            }
            wait(NULL);
            i++;
        }
    }
}

