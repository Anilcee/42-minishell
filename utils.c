#include"minishell.h"

static int	count_strings(const char *str, char separator)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (str[i] != '\0')
	{
		while (str[i] != '\0' && (str[i] == separator))
			i++;
		if (str[i] != '\0')
			count++;
		while (str[i] != '\0' && !(str[i] == separator))
			i++;
	}
	return (count);
}

static int	ft_strlen_sep(const char *str, char separator)
{
	int	i;

	i = 0;
	while (str[i] && !(str[i] == separator))
		i++;
	return (i);
}

static char	*ft_word(const char *str, char separator)
{
	int		len_word;
	int		i;
	char	*word;

	i = 0;
	len_word = ft_strlen_sep(str, separator);
	word = (char *)malloc(sizeof(char) * (len_word + 1));
	if (!word)
		return (NULL);
	while (i < len_word)
	{
		word[i] = str[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static int	ft_clean(char **a, int i)
{
	if (!a[i])
	{
		while (i > 0)
			free(a[--i]);
		free(a);
		return (1);
	}
	return (0);
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

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = 0;
	str = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (str == NULL)
		return (NULL);
	while (s1[i] != '\0')
	{
		str[i] = s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		str[i + j] = s2[j];
		j++;
	}
	str[i + j] = '\0';
	return (str);
}

char	*ft_strdup(const char *s)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = ft_strlen(s);
	str = (char *)malloc(sizeof(*str) * (j + 1));
	if (!str)
		return (NULL);
	while (i < j)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

size_t ft_strlen(const char *s)
{
    size_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;
	if (n == 0)
		return (0);
	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i] && i < n - 1)
		++i;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char	*ft_strndup(const char *s, size_t n)
{
	size_t	i;
	size_t	len;
	char	*str;

	len = 0;
	while (s[len] && len < n)
		len++;
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (i < len)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
int is_num(char *str)
{
    int i = 0;
    if (!str || !str[0])
        return 0;
    if (str[i] == '-' || str[i] == '+')
        i++;
    if (!str[i])
        return 0;
    while (str[i])
    {
        if (!(str[i] >= '0' && str[i] <= '9'))
            return 0;
        i++;
    }
    return 1;
}

int ft_isalnum(char c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9'))
        return 1;
    return 0;
}
int ft_strcmp(const char *s1, const char *s2)
{
    int i = 0;

    while (s1[i] && s2[i] && s1[i] == s2[i])
        i++;

    return (unsigned char)s1[i] - (unsigned char)s2[i];
}

char	*ft_strchr(const char *s, int i)
{
	while (*s)
	{
		if (*s == (char)i)
			return ((char *)s);
		s++;
	}
	if ((char)i == '\0')
		return ((char *)s);
	return (0);
}
