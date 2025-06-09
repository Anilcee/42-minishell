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

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE];
	char		line[70000];
	static int	buffer_readed;
	static int 	buffer_pos;
	int			i;

	i = 0;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (1)
	{
		if (buffer_pos >= buffer_readed)
		{
			buffer_readed = read(fd, buffer, BUFFER_SIZE);
			buffer_pos = 0;
			if (buffer_readed <= 0)
				break ;
		}
		line[i] = buffer[buffer_pos];
        i++;
        buffer_pos++;
		if (line[i - 1] == '\n')
			break ;
	}
	line[i] = '\0';
	if (i == 0)
		return (NULL);
	return (ft_strdup(line));
}