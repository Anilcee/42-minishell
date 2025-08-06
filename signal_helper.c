/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 22:30:52 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/06 17:21:52 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler_heredoc(int sig)
{
	(void)sig;
	g_signal_received = 1;
	write(1, "\n", 1);
	close(0);
}

void	setup_signals_heredoc(void)
{
	signal(SIGINT, sigint_handler_heredoc);
	signal(SIGQUIT, SIG_IGN);
}

void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	setup_signals_parent(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
