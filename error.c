/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 10:28:06 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:32:17 by ancengiz         ###   ########.fr       */
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
