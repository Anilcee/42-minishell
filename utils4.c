/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancengiz <ancengiz@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 13:36:40 by ancengiz          #+#    #+#             */
/*   Updated: 2025/07/27 14:27:05 by ancengiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_strings(const char *str, char separator)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (*str != separator && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (*str == separator)
			in_word = 0;
		str++;
	}
	return (count);
}

static char	*ft_word(const char *s, char c)
{
	int		len;
	char	*word;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	word = (char *)malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	ft_strlcpy(word, s, len + 1);
	return (word);
}

char	**ft_split(const char *str, char separator)
{
	char	**srg;
	int		i;

	i = 0;
	srg = (char **)malloc(sizeof(char *) * (count_strings(str, separator) + 1));
	if (!srg)
		return (NULL);
	while (*str != '\0')
	{
		while (*str != '\0' && (*str == separator))
			str++;
		if (*str != '\0')
		{
			srg[i] = ft_word(str, separator);
			if (ft_clean(srg, i))
				return (NULL);
			i++;
		}
		while (*str && !(*str == separator))
			str++;
	}
	srg[i] = 0;
	return (srg);
}
