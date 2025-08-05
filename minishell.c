/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/05 17:59:14 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	if (!handle_redirections_block(&ctx, shell))
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

static int	handle_command_flow(char *input, t_shell *shell)
{
	t_token		*tokens;
	t_command	*cmds;
	int			result;

	result = 1;
	if (check_unclosed_quotes(input))
	{
		printf("minishell: syntax error: unclosed quote\n");
		tokens = NULL;
		shell->last_exit_code = 1;
	}
	else
	{
		tokens = tokenize(input, shell);
		cmds = parse_tokens(tokens);
		if (cmds)
			result = execute_command(cmds, tokens, shell);
		else
		{
			if (g_signal_received == SIGINT)
				shell->last_exit_code = 130;
		}
		free_tokens_and_commands(tokens, cmds);
	}
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
