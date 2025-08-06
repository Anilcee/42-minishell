/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oislamog <oislamog@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 00:00:00 by oislamog          #+#    #+#             */
/*   Updated: 2025/08/06 00:00:00 by oislamog         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_absolute_path_status(char *command_name)
{
	DIR	*dir;

	if (access(command_name, F_OK) != 0)
		return (FILE_NOT_FOUND);
	dir = opendir(command_name);
	if (dir != NULL)
	{
		closedir(dir);
		return (IS_DIRECTORY);
	}
	if (access(command_name, X_OK) != 0)
		return (PERMISSION_DENIED);
	return (CMD_SUCCESS);
}
