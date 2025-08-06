/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:28:06 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/06 19:00:31 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_export_error(char *input)
{
	write(STDERR_FILENO, "minishell: export: `", 20);
	write(STDERR_FILENO, input, ft_strlen(input));
	write(STDERR_FILENO, "': not a valid identifier\n", 26);
}

void	print_cd_error(const char *path, const char *msg)
{
	write(STDERR_FILENO, "minishell: cd: ", 15);
	if (path)
	{
		write(STDERR_FILENO, path, ft_strlen(path));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
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
	{
		write(STDERR_FILENO, "minishell: ", 12);
		print_error_message(cmds->args[0], ": No such file or directory\n", 127,
			shell);
	}
	else if (result == IS_DIRECTORY)
	{
		write(STDERR_FILENO, "minishell: ", 12);
		print_error_message(cmds->args[0], ": Is a directory\n", 126, shell);
	}
	else if (result == PERMISSION_DENIED)
	{
		write(STDERR_FILENO, "minishell: ", 12);
		print_error_message(cmds->args[0], ": Permission denied\n", 126, shell);
	}	
}

void	print_error_and_exit(char *cmd, char *msg, int exit_code)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, msg, ft_strlen(msg));
	exit(exit_code);
}
