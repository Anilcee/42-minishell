#include "minishell.h"

int	check_absolute_path(char *command_name)
{
	DIR	*dir;

	if (access(command_name, F_OK) != 0)
		return (-2);
	dir = opendir(command_name);
	if (dir != NULL)
	{
		closedir(dir);
		return (-3);
	}
	if (access(command_name, X_OK) != 0)
		return (-4);
	return (0);
}

char	*find_in_path(char *command_name)
{
	char	*path_env;
	char	**paths;
	char	*program_path;
	char	*temp;
	int		i;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	i = -1;
	while (paths[++i])
	{
		temp = ft_strjoin(paths[i], "/");
		program_path = ft_strjoin(temp, command_name);
		free(temp);
		if (access(program_path, X_OK) == 0)
		{
			free_paths_array(paths);
			return (program_path);
		}
		free(program_path);
	}
	free_paths_array(paths);
	return (NULL);
}

int	execute_child_process(char *program_path, t_command *cmd, char **envp)
{
	DIR	*dir;

	if (access(program_path, F_OK) != 0)
		exit(127);
	dir = opendir(program_path);
	if (dir != NULL)
	{
		closedir(dir);
		exit(126);
	}
	if (access(program_path, X_OK) != 0)
		exit(126);
	execve(program_path, cmd->args, envp);
	exit(127);
}

void	free_paths_array(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

int	get_exit_status(int status)
{
	if (ft_wifexited(status))
		return (ft_wexitstatus(status));
	else if (ft_wifsignaled(status))
		return (128 + ft_wtermsig(status));
	else
		return (1);
}

int	external_commands(t_command *cmd, char **envp)
{
	pid_t	pid;
	char	*program_path;
	char	*command_name;
	int		status;
	int		check_result;

	command_name = cmd->args[0];
	if (!command_name)
		return (-1);
	if (command_name[0] == '/' || command_name[0] == '.')
	{
		check_result = check_absolute_path(command_name);
		if (check_result != 0)
			return (check_result);
		program_path = ft_strdup(command_name);
	}
	else
	{
		program_path = find_in_path(command_name);
		if (!program_path)
			return (-1);
	}
	pid = fork();
	if (pid == 0)
		execute_child_process(program_path, cmd, envp);
	else
		waitpid(pid, &status, 0);
	free(program_path);
	return (get_exit_status(status));
}
