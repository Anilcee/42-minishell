/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:31:17 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			g_signal_received = 0;

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

static int	handle_cd_pwd_env(t_command *cmds, t_shell *shell)
{
	if (ft_strcmp(cmds->args[0], "cd") == 0)
		shell->last_exit_code = builtin_cd(cmds, &shell->env_list);
	else if (ft_strcmp(cmds->args[0], "pwd") == 0)
		shell->last_exit_code = builtin_pwd();
	else if (ft_strcmp(cmds->args[0], "env") == 0)
		shell->last_exit_code = builtin_env(shell->envp);
	else
		return (0);
	return (1);
}

static int	handle_echo_unset_export(t_command *cmds, t_shell *shell)
{
	if (ft_strcmp(cmds->args[0], "echo") == 0)
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
	else
		return (0);
	return (1);
}

static int	handle_exit_builtin(t_command *cmds, t_shell *shell)
{
	int	ret;
	int	exit_code;

	if (ft_strcmp(cmds->args[0], "exit") != 0)
		return (0);
	ret = builtin_exit(cmds, &exit_code);
	if (ret == EXIT_TOO_MANY_ARGS)
		shell->last_exit_code = 1;
	else if (ret == EXIT_NOT_NUMERIC)
		shell->last_exit_code = 2;
	else if (ret == EXIT_NO_ARG)
		return (1);
	else if (ret == EXIT_ARG_VALUE)
	{
		shell->last_exit_code = exit_code;
		return (1);
	}
	return (1);
}

int	execute_builtin(t_command *cmds, t_shell *shell)
{
	if (handle_cd_pwd_env(cmds, shell))
		return (0);
	if (handle_echo_unset_export(cmds, shell))
		return (0);
	if (handle_exit_builtin(cmds, shell))
		return (-1);
	return (0);
}

void	print_error_message(char *cmd_name, char *error_msg, int exit_code,
		t_shell *shell)
{
	write(STDERR_FILENO, cmd_name, ft_strlen(cmd_name));
	write(STDERR_FILENO, error_msg, ft_strlen(error_msg));
	shell->last_exit_code = exit_code;
}

void	handle_external_error(t_command *cmds, int result, t_shell *shell)
{
	if (result == CMD_NOT_FOUND)
		print_error_message(cmds->args[0], ": command not found\n", 127, shell);
	else if (result == FILE_NOT_FOUND || result == PATH_NOT_SET)
		print_error_message(cmds->args[0], ": No such file or directory\n", 127,
			shell);
}

void	handle_external_error2(t_command *cmds, int result, t_shell *shell)
{
	if (result == IS_DIRECTORY)
		print_error_message(cmds->args[0], ": Is a directory\n", 126, shell);
	else if (result == PERMISSION_DENIED)
		print_error_message(cmds->args[0], ": Permission denied\n", 126, shell);
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

	external_result = external_commands(cmds, shell);
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

static int	handle_pipes(t_command *cmds, t_token *tokens, t_shell *shell)
{
	if (has_pipe(cmds))
	{
		shell->last_exit_code = execute_piped_commands(cmds, tokens, shell);
		return (1);
	}
	return (0);
}

static int	handle_redirections_block(t_redirection_context *ctx)
{
	if (!setup_redirections(ctx->cmds, ctx->saved_stdout, ctx->saved_stdin))
	{
		ctx->shell->last_exit_code = 1;
		return (0);
	}
	return (1);
}

static int	handle_builtin_or_external(t_command *cmds, t_shell *shell,
		int saved_stdout, int saved_stdin)
{
	if (is_builtin(cmds->args[0]))
	{
		if (execute_builtin(cmds, shell) == -1)
		{
			restore_redirections(saved_stdout, saved_stdin);
			return (0);
		}
	}
	else
		execute_external_cmd(cmds, shell);
	return (1);
}

int	execute_command(t_command *cmds, t_token *tokens, t_shell *shell)
{
	int						saved_stdout;
	int						saved_stdin;
	t_redirection_context	ctx;

	if (!validate_command(cmds, shell))
		return (1);
	if (handle_pipes(cmds, tokens, shell))
		return (1);
	ctx.cmds = cmds;
	ctx.saved_stdout = &saved_stdout;
	ctx.saved_stdin = &saved_stdin;
	ctx.shell = shell;
	if (!handle_redirections_block(&ctx))
		return (1);
	if (!handle_builtin_or_external(cmds, shell, saved_stdout, saved_stdin))
		return (0);
	restore_redirections(saved_stdout, saved_stdin);
	return (1);
}

void	init_shell(t_shell *shell, char **envp)
{
	shell->last_exit_code = 0;
	shell->envp = copy_env(envp);
	shell->env_list = envp_to_list(shell->envp);
}

void	handle_signal_interrupt(t_shell *shell)
{
	if (g_signal_received == SIGINT)
	{
		shell->last_exit_code = 130;
		g_signal_received = 0;
	}
}

static int	handle_command_flow(char *input, t_shell *shell)
{
	t_token		*tokens;
	t_command	*cmds;
	int			result;

	tokens = tokenize(input, shell);
	cmds = parse_tokens(tokens);
	if (cmds)
		result = execute_command(cmds, tokens, shell);
	else
	{
		result = 1;
		if (g_signal_received == SIGINT)
			shell->last_exit_code = 130;
	}
	free_tokens(tokens);
	free_commands(cmds);
	return (result);
}

int	process_input(char *input, t_shell *shell)
{
	int	result;

	if (!input)
	{
		printf("exit\n");
		return (0);
	}
	handle_signal_interrupt(shell);
	if (input && *input)
		add_history(input);
	result = handle_command_flow(input, shell);
	free(input);
	return (result);
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	t_shell	shell;
	int		exit_code;

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
	exit_code = shell.last_exit_code;
	cleanup_history();
	free_shell(&shell);
	rl_clear_history();
	return (exit_code);
}
