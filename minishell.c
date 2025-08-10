/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/08 21:42:31 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_and_preprocess(t_exec_context *exec)
{
	if (!validate_command(exec->all_cmds, exec->shell))
		return (1);
	if (preprocess_heredocs(exec) < 0)
	{
		exec->shell->last_exit_code = 1;
		return (1);
	}
	return (0);
}

static int	handle_single_command(t_exec_context *exec)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redir_context	ctx;

	ctx.cmds = exec->all_cmds;
	ctx.saved_stdout = &saved_stdout;
	ctx.saved_stdin = &saved_stdin;
	ctx.shell = exec->shell;
	
	if (!handle_redirections_block(&ctx, exec))
		return (1);
	if (!handle_builtin_or_external(exec->all_cmds, exec->shell, 
			saved_stdout, saved_stdin))
		return (0);
	restore_redirections(saved_stdout, saved_stdin);
	return (1);
}

int	execute_command(t_exec_context *exec)
{
	int result;

	result = validate_and_preprocess(exec);
	if (result != 0)
		return (result);
	
	if (handle_pipes(exec->all_cmds, exec->all_tokens, exec->shell))
	{
		cleanup_heredocs(exec->all_cmds);
		return (1);
	}
	
	result = handle_single_command(exec);
	cleanup_heredocs(exec->all_cmds);
	return (result);
}

static void	init_exec(t_exec_context *exec, char **envp)
{
	exec->shell = malloc(sizeof(t_shell));
	if (!exec->shell)
	{
		fprintf(stderr, "minishell: memory allocation failed\n");
		exit(1);
	}
	exec->shell->last_exit_code = 0;
	exec->shell->envp = copy_env(envp);
	exec->shell->env_list = envp_to_list(exec->shell->envp);
}

static int	handle_syntax_error(char *input, t_exec_context *exec)
{
	(void)input;
	printf("minishell: syntax error: unclosed quote\n");
	exec->shell->last_exit_code = 1;
	return (1);
}

static int	process_parsed_commands(t_token *tokens, t_command *cmds, t_exec_context *exec)
{
	int result;

	if (cmds)
	{
		exec->all_cmds = cmds;
		exec->all_tokens = tokens;
		exec->current = cmds;
		result = execute_command(exec);
	}
	else
	{
		result = 1;
		if (g_signal_received == SIGINT)
			exec->shell->last_exit_code = 130;
	}
	free_tokens_and_commands(tokens, cmds);
	return (result);
}

static int	handle_command_flow(char *input, t_exec_context *exec)
{
	t_token		*tokens;
	t_command	*cmds;

	if (check_unclosed_quotes(input))
		return (handle_syntax_error(input, exec));
	
	tokens = tokenize(input, exec->shell);
	cmds = parse_tokens(tokens, exec->shell);
	return (process_parsed_commands(tokens, cmds, exec));
}

static int	process_input(char *input, t_exec_context *exec)
{
	int	result;

	if (!input)
	{
		printf("exit\n");
		return (0);
	}
	handle_signal_interrupt(exec->shell);
	if (input && *input)
		add_history(input);
	result = handle_command_flow(input, exec);
	free(input);
	return (result);
}

int	main(int argc, char **argv, char **envp)
{
	char			*input;
	t_exec_context	exec;
	int				exit_code;

	(void)argc;
	(void)argv;
	init_exec(&exec, envp);
	setup_signals();
	while (1)
	{
		input = readline("minishell$ ");
		if (!process_input(input, &exec))
			break ;
	}
	exit_code = exec.shell->last_exit_code;
	cleanup_history();
	free_exec(&exec);
	rl_clear_history();
	return (exit_code);
}
