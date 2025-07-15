#include "minishell.h"

int g_signal_received = 0;

void	sigint_handler(int sig)
{
	(void)sig;
	g_signal_received = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	setup_signals(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	execute_builtin(t_command *cmds, t_shell *shell)
{
	int	ret;

	if (ft_strcmp(cmds->args[0], "cd") == 0)
		shell->last_exit_code = builtin_cd(cmds, shell->env_list);
	else if (ft_strcmp(cmds->args[0], "pwd") == 0)
		shell->last_exit_code = builtin_pwd();
	else if (ft_strcmp(cmds->args[0], "env") == 0)
		shell->last_exit_code = builtin_env(shell->envp);
	else if (ft_strcmp(cmds->args[0], "echo") == 0)
	{
		builtin_echo(cmds);
		shell->last_exit_code = 0;
	}
	else if (ft_strcmp(cmds->args[0], "unset") == 0)
		shell->last_exit_code = builtin_unset(cmds, &shell->envp,
				&shell->env_list);
	else if (ft_strcmp(cmds->args[0], "export") == 0)
		shell->last_exit_code = builtin_export(cmds, &shell->envp,
				&shell->env_list);
	else if (ft_strcmp(cmds->args[0], "exit") == 0)
	{
		ret = builtin_exit(cmds);
		if (ret == 1)
			shell->last_exit_code = 1;
	}
}

void	handle_external_error(t_command *cmds, int result, t_shell *shell)
{
	if (result == -1)
	{
		write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		shell->last_exit_code = 127;
	}
	else if (result == -2)
	{
		write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
		write(STDERR_FILENO, ": No such file or directory\n", 28);
		shell->last_exit_code = 127;
	}
}

void	handle_external_error2(t_command *cmds, int result, t_shell *shell)
{
	if (result == -3)
	{
		write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
		write(STDERR_FILENO, ": Is a directory\n", 17);
		shell->last_exit_code = 126;
	}
	else if (result == -4)
	{
		write(STDERR_FILENO, cmds->args[0], ft_strlen(cmds->args[0]));
		write(STDERR_FILENO, ": Permission denied\n", 20);
		shell->last_exit_code = 126;
	}
}

int	setup_redirections(t_command *cmds, int *saved_stdout, int *saved_stdin)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (handle_redirections(cmds) < 0)
	{
		dup2(*saved_stdout, STDOUT_FILENO);
		dup2(*saved_stdin, STDIN_FILENO);
		close(*saved_stdout);
		close(*saved_stdin);
		return (0);
	}
	return (1);
}

void	restore_redirections(int saved_stdout, int saved_stdin)
{
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

int	execute_external_cmd(t_command *cmds, t_shell *shell)
{
	int	external_result;

	external_result = external_commands(cmds, shell->envp);
	if (external_result < 0)
	{
		handle_external_error(cmds, external_result, shell);
		handle_external_error2(cmds, external_result, shell);
	}
	else
		shell->last_exit_code = external_result;
	return (1);
}

int	validate_command(t_command *cmds, t_shell *shell)
{
	if (!cmds || !shell || !shell->envp || !shell->env_list)
		return (0);
	if (!cmds->args || !cmds->args[0])
	{
		shell->last_exit_code = 1;
		return (0);
	}
	return (1);
}

int	execute_command(t_command *cmds, t_shell *shell)
{
	int	saved_stdout;
	int	saved_stdin;

	if (!validate_command(cmds, shell))
		return (1);
	if (has_pipe(cmds))
	{
		shell->last_exit_code = execute_piped_commands(cmds, shell->envp);
		return (1);
	}
	if (!setup_redirections(cmds, &saved_stdout, &saved_stdin))
	{
		shell->last_exit_code = 1;
		return (1);
	}
	if (is_builtin(cmds->args[0]))
		execute_builtin(cmds, shell);
	else
		execute_external_cmd(cmds, shell);
	restore_redirections(saved_stdout, saved_stdin);
	return (1);
}

void	init_shell(t_shell *shell, char **envp)
{
	shell->last_exit_code = 0;
	shell->envp = copy_env(envp);
	shell->env_list = envp_to_list(shell->envp);
}

int	handle_signal_interrupt(t_shell *shell)
{
	if (g_signal_received == SIGINT)
	{
		shell->last_exit_code = 130;
		g_signal_received = 0;
		return (1);
	}
	return (0);
}

int	process_input(char *input, t_shell *shell)
{
	t_token		*tokens;
	t_command	*cmds;
	int			result;

	if (!input)
	{
		printf("exit\n");
		return (0);
	}
	if (handle_signal_interrupt(shell))
		return (1);
	if (input && *input)
		builtin_history(input);
	tokens = tokenize(input, shell->env_list, shell);
	cmds = parse_tokens(tokens);
	result = execute_command(cmds, shell);
	// TODO: Belleği temizle
	// free_tokens(tokens);
	// free_commands(cmds);
	free(input);
	return (result);
}

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_shell		shell;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	setup_signals();
	while (1)
	{
		input = readline("minishell$ ");
		if (!process_input(input, &shell))
			break ;
	}
	return (shell.last_exit_code);
}
