#include "minishell.h"

// Custom wait status checking functions
int ft_wifexited(int status)
{
    return ((status & 0x7f) == 0);
}

int ft_wexitstatus(int status)
{
    return ((status & 0xff00) >> 8);
}

int ft_wifsignaled(int status)
{
    return ((status & 0x7f) != 0 && (status & 0x7f) != 0x7f);
}

int ft_wtermsig(int status)
{
    return (status & 0x7f);
}

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
        return -1;
    }
    char *line = NULL;
    while (1) 
    {
        line = readline("> ");
        if (!line)  
            break;
        if (ft_strcmp(line, delimiter) == 0) 
        {
            free(line);
            break;
        }
        write(pipefd[1], line, ft_strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return 0;
}
void add_pid(t_pid_list **head, pid_t pid)
{
    t_pid_list *new_node = malloc(sizeof(t_pid_list));
    if (!new_node)
        return;
    new_node->pid = pid;
    new_node->next = NULL;

    if (!*head)
        *head = new_node;
    else
    {
        t_pid_list *temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_node;
    }
}
void wait_and_free_pids(t_pid_list *head)
{
    t_pid_list *temp;

    while (head)
    {
        waitpid(head->pid, NULL, 0);
        temp = head;
        head = head->next;
        free(temp);
    }
}


int has_pipe(t_command *cmds) 
{
    return cmds && cmds->next != NULL;
}

int handle_redirections(t_command *cmd)
{
    int input_fd = -1, output_fd = -1;
    
    // Handle input redirection first
    if (cmd->infile)
    {
        if (cmd->heredoc)
        {
            if (handle_heredoc_redirection(cmd) < 0)
                return -1;
        }
        else
        {
            input_fd = open(cmd->infile, O_RDONLY);
            if (input_fd < 0)
            {
                perror(cmd->infile);
                return -1;
            }
        }
    }
    
    // Handle output redirection
    if (cmd->outfile)
    {
        if (cmd->append)
        {
            output_fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            output_fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        
        if (output_fd < 0)
        {
            perror(cmd->outfile);
            if (input_fd >= 0)
                close(input_fd);
            return -1;
        }
    }
    
    // Only apply redirections if all opens succeeded
    if (input_fd >= 0)
    {
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);
    }
    
    if (output_fd >= 0)
    {
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
    }
    
    return 0;
}

int execute_piped_commands(t_command *cmds, char **envp) 
{
    int fd[2];
    int prev_fd = -1;
    pid_t pid;
    t_command *current = cmds;
    t_pid_list *pid_list = NULL;
    int final_exit_code = 0;

    while (current) 
    {
        if (current->next) 
        {
            if (pipe(fd) == -1) 
            {
                perror("pipe");
                return 1;
            }
        }

        pid = fork();
        if (pid == -1) 
        {
            perror("fork");
            return 1;
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

            // Handle redirections and exit immediately if they fail
            if (handle_redirections(current) < 0)
            {
                exit(1);
            }

            // PATH resolution for execve
            char *program_path = NULL;
            char *command_name = current->args[0];
            
            if (command_name[0] == '/' || command_name[0] == '.')
            {
                if (access(command_name, X_OK) != 0)
                {
                    perror("execve");
                    exit(127);
                }
                program_path = ft_strdup(command_name);
            }
            else
            {
                char *path_env = getenv("PATH");
                if (!path_env)
                {
                    perror("execve");
                    exit(127);
                }
                char **paths = ft_split(path_env, ':');
                int i = 0;
                while (paths[i])
                {
                    char *temp = ft_strjoin(paths[i], "/");
                    program_path = ft_strjoin(temp, command_name);
                    free(temp);
                    if (access(program_path, X_OK) == 0)
                        break;
                    free(program_path);
                    program_path = NULL;
                    i++;
                }
                // Free paths array
                int j = 0;
                while (paths[j])
                    free(paths[j++]);
                free(paths);
            }
            
            if (!program_path)
            {
                perror("execve");
                exit(127);
            }

            execve(program_path, current->args, envp);
            free(program_path);
            perror("execve");
            exit(127);
        } 
        else 
        {
            add_pid(&pid_list, pid);

            if (prev_fd != -1)
                close(prev_fd);
            if (current->next) 
            {
                close(fd[1]);
                prev_fd = fd[0];
            }

            current = current->next;
        }
    }

    // Wait for all processes and get the exit code of the last one
    t_pid_list *temp = pid_list;
    while (temp)
    {
        int status;
        waitpid(temp->pid, &status, 0);
        if (temp->next == NULL) // This is the last process
        {
            if (ft_wifexited(status))
                final_exit_code = ft_wexitstatus(status);
            else if (ft_wifsignaled(status))
                final_exit_code = 128 + ft_wtermsig(status);
        }
        temp = temp->next;
    }

    // Free the pid list
    while (pid_list)
    {
        temp = pid_list;
        pid_list = pid_list->next;
        free(temp);
    }

    return final_exit_code;
}
