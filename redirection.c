#include "minishell.h"

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

int execute_builtin_in_child(t_command *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		exit(builtin_cd(cmd, shell->env_list));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		exit(builtin_pwd());
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		exit(builtin_env(shell->envp));
	else if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(cmd);
		exit(0);
	}
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		exit(builtin_export(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		exit(builtin_unset(cmd, &shell->envp, &shell->env_list));
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
	{
		int ret = builtin_exit(cmd);
		if (ret == 1)
			exit(1);
		exit(0);
	}
	return 0;
}

int is_builtin(const char *cmd)
{
	return (ft_strcmp(cmd, "cd") == 0 ||
			ft_strcmp(cmd, "pwd") == 0 ||
			ft_strcmp(cmd, "env") == 0 ||
			ft_strcmp(cmd, "echo") == 0 ||
			ft_strcmp(cmd, "export") == 0 ||
			ft_strcmp(cmd, "unset") == 0 ||
			ft_strcmp(cmd, "exit") == 0);
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

int handle_heredoc(const char *delimiter)
{
	int		pipefd[2];
	char	*line;
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}

	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if(line) free(line);
			break;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}

	close(pipefd[1]);
	return pipefd[0];
}


int handle_redirections(t_command *cmd)
{
	t_redirect *redir = cmd->redirects;
	int in_fd = -1;
	int out_fd = -1;


	while (redir)
	{
		if (redir->type == REDIR_IN)
		{
			if (in_fd != -1) close(in_fd);
			in_fd = open(redir->filename, O_RDONLY);
			if (in_fd < 0)
			{
				perror(redir->filename);
				if (out_fd != -1) close(out_fd);
				return -1;
			}
		}
		else if (redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
		{
			if (out_fd != -1) close(out_fd);
			int flags = O_WRONLY | O_CREAT;
			if (redir->type == REDIR_APPEND)
				flags |= O_APPEND;
			else
				flags |= O_TRUNC;
			out_fd = open(redir->filename, flags, 0644);
			if (out_fd < 0)
			{
				perror(redir->filename);
				if (in_fd != -1) close(in_fd);
				return -1;
			}
		}
		else if (redir->type == REDIR_HEREDOC)
		{
			if (in_fd != -1) close(in_fd);
			in_fd = handle_heredoc(redir->filename);
			if (in_fd < 0)
			{
				if (out_fd != -1) close(out_fd);
				return -1;
			}
		}
		redir = redir->next;
	}

	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
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
	
	// Create shell structure for builtin commands
	t_shell shell;
	shell.envp = envp;
	shell.env_list = envp_to_list(envp);
	shell.last_exit_code = 0;

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

			// Check if it's a builtin command first
			if (is_builtin(current->args[0]))
			{
				execute_builtin_in_child(current, &shell);
			}

			// PATH resolution for execve
			char *program_path = NULL;
			char *command_name = current->args[0];
			
			if (command_name[0] == '/' || command_name[0] == '.')
			{
				if (access(command_name, F_OK) != 0)
				{
					write(STDERR_FILENO, command_name, ft_strlen(command_name));
					write(STDERR_FILENO, ": No such file or directory\n", 28);
					exit(127);
				}
				
				// Check if it's a directory using opendir
				DIR *dir = opendir(command_name);
				if (dir != NULL)
				{
					closedir(dir);
					write(STDERR_FILENO, command_name, ft_strlen(command_name));
					write(STDERR_FILENO, ": Is a directory\n", 17);
					exit(126);
				}
				
				if (access(command_name, X_OK) != 0)
				{
					write(STDERR_FILENO, command_name, ft_strlen(command_name));
					write(STDERR_FILENO, ": Permission denied\n", 20);
					exit(126);
				}
				
				program_path = ft_strdup(command_name);
			}
			else
			{
				char *path_env = getenv("PATH");
				if (!path_env)
				{
					write(STDERR_FILENO, command_name, ft_strlen(command_name));
					write(STDERR_FILENO, ": command not found\n", 20);
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
				write(STDERR_FILENO, command_name, ft_strlen(command_name));
				write(STDERR_FILENO, ": command not found\n", 20);
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
		
		// Check for SIGPIPE and show "Broken pipe" error
		if (ft_wifsignaled(status) && ft_wtermsig(status) == SIGPIPE)
		{
			write(STDERR_FILENO, "Broken pipe\n", 12);
		}
		
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

	// Free the environment list
	free_env_list(shell.env_list);

	return final_exit_code;
}

// Helper function to free env list
void free_env_list(t_env *head)
{
	t_env *temp;
	while (head)
	{
		temp = head;
		head = head->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}
