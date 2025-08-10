/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_flow.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 01:30:00 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/11 01:30:00 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_syntax_error(char *input, t_exec_context *exec)
{
	(void)input;
	printf("minishell: syntax error: unclosed quote\n");
	exec->shell->last_exit_code = 1;
	return (1);
}

int	process_parsed_commands(t_token *tokens, t_command *cmds,
		t_exec_context *exec)
{
	int	result;

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

int	handle_command_flow(char *input, t_exec_context *exec)
{
	t_token		*tokens;
	t_command	*cmds;

	if (check_unclosed_quotes(input))
		return (handle_syntax_error(input, exec));
	tokens = tokenize(input, exec->shell);
	cmds = parse_tokens(tokens, exec->shell);
	return (process_parsed_commands(tokens, cmds, exec));
}
