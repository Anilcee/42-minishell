#include "minishell.h"

int external_commands(t_command *cmd, char **envp)
{
    pid_t pid;
    char *path_env;
    char **paths = NULL;
    char *program_path = NULL;
    char *command_name = cmd->args[0];
    int status;

    if (!command_name)
        return -1;
    if (command_name[0] == '/' || command_name[0] == '.')
    {
        if (access(command_name, F_OK) != 0)
            return -2; // Return -2 for absolute/relative path not found
        
        // Check if it's a directory using opendir
        DIR *dir = opendir(command_name);
        if (dir != NULL)
        {
            closedir(dir);
            return -3; // Return -3 for directory
        }
            
        if (access(command_name, X_OK) != 0)
            return -4; // Return -4 for permission denied
            
        program_path = ft_strdup(command_name);
    }
    else
    {
        path_env = getenv("PATH");
        if (!path_env)
            return -1;
        paths = ft_split(path_env, ':');
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
    }
    if (!program_path)
        return -1; // Return -1 for command not found in PATH
    pid = fork();
    if (pid == 0)
    {
        execve(program_path, cmd->args, envp);
        if (errno == ENOENT)
            exit(127); // Komut bulunamadı (bash ile uyumlu)
        else if (errno == EACCES)
            exit(126); // Erişim hatası (izin yok)
        else
            exit(1);   // Diğer hatalar
    }
    else
    {
        waitpid(pid, &status, 0);
    }
    free(program_path);
    if (paths)
    {
        int j = 0;
        while (paths[j])
            free(paths[j++]);
        free(paths);
    }
    
    // Return the actual exit code using custom functions
    if (ft_wifexited(status))
        return ft_wexitstatus(status);
    else if (ft_wifsignaled(status))
        return 128 + ft_wtermsig(status);
    else
        return 1;
}
