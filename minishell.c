/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:43 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:29:05 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
