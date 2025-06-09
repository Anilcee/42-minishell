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

char ***split_by_pipe(char **args) 
{
    char ***commands = malloc(sizeof(char **) * 10); 
    int cmd_idx = 0;
    //int arg_idx = 0;
    int i = 0;
    commands[cmd_idx] = malloc(sizeof(char *) * 100);
    int inner_idx = 0;
    while (args[i]) 
    {
        if (strcmp(args[i], "|") == 0) 
        {
            commands[cmd_idx][inner_idx] = NULL; 
            cmd_idx++;
            inner_idx = 0;
            commands[cmd_idx] = malloc(sizeof(char *) * 100);
        } 
        else 
        {
            commands[cmd_idx][inner_idx++] = args[i];
        }
        i++;
    }
    commands[cmd_idx][inner_idx] = NULL;
    commands[cmd_idx + 1] = NULL; 
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
