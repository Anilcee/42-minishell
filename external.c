#include "minishell.h"

int external_commands(char **args, char **envp)
{
    pid_t pid;
    char *path_env;
    char **paths = NULL;
    char *program_path = NULL;

    if (args[0][0] == '/' || args[0][0] == '.')
    {
        if (access(args[0], X_OK) != 0)
            return 0;
        program_path = strdup(args[0]);
    }
    else
    {
        path_env = getenv("PATH");
        if (!path_env)
            return 0;
        paths = ft_split(path_env, ':');
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
    }

    if (!program_path)
        return 0;

    pid = fork();
    if (pid == 0)
    {
        handle_redirections(args);
        execve(program_path, args, envp);
        perror("execve");
        exit(1);
    }
    else
    {
        wait(NULL);
    }

    free(program_path);
    if (paths)
    {
        int j = 0;
        while (paths[j])
            free(paths[j++]);
        free(paths);
    }
    return 1;
}