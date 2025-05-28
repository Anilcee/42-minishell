#ifndef MINISHELL_H
#define MINISHELL_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <readline/readline.h>
    #include <readline/history.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <unistd.h>
    #include <stdio.h>

    char	**ft_split(const char *str, char separator);
    char	*ft_strjoin(char const *s1, char const *s2);
    char	*ft_strdup(const char *s);
    size_t ft_strlen(const char *s);
    void    external();
#endif
