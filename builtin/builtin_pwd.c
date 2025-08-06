/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 01:35:32 by ancengiz          #+#    #+#             */
/*   Updated: 2025/08/01 10:47:53 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	update_pwd_vars(t_env **env_list, char *old_pwd_val)
{
	char	*new_pwd_val;
	char	*oldpwd_str;
	char	*pwd_str;

	new_pwd_val = getcwd(NULL, 0);
	if (!new_pwd_val)
		return ;
	oldpwd_str = ft_strjoin("OLDPWD=", old_pwd_val);
	if (oldpwd_str)
	{
		add_env_list(env_list, oldpwd_str);
		free(oldpwd_str);
	}
	pwd_str = ft_strjoin("PWD=", new_pwd_val);
	if (pwd_str)
	{
		add_env_list(env_list, pwd_str);
		free(pwd_str);
	}
	free(new_pwd_val);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd != NULL)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}

int	is_valid_identifier(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (!is_valid_first_char(str[0]))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!is_valid_identifier_char(str[i]))
			return (0);
		i++;
	}
	return (1);
}
