/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscarmathot <oscarmathot@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 09:25:57 by omathot           #+#    #+#             */
/*   Updated: 2023/12/04 13:42:55 by oscarmathot      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	free_array(char **array)
{
	size_t	i;

	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

static unsigned long	count_columns(char const *s, char c)
{
	unsigned long	columns;

	columns = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s && *s != c)
		{
			columns++;
			while (*s && *s != c)
				s++;
		}
	}
	return (columns);
}

static char	*allocate_row(const char *s, char c)
{
	size_t	len;
	size_t	i;
	char	*str;

	len = 0;
	i = 0;
	while (s[len] && s[len] != c)
		len++;
	str = malloc(len + 1);
	if (!str)
		return (NULL);
	while (i < len)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

static char	**make_array(const char *s, char **array, char c)
{
	size_t	i;

	i = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s && *s != c)
		{
			array[i] = allocate_row(s, c);
			if (array[i] == NULL)
			{
				free_array(array);
				return (NULL);
			}
			i++;
		}
		while (*s && *s != c)
			s++;
	}
	array[i] = NULL;
	return (array);
}

char	**ft_split(const char *s, char c)
{
	char	**array;

	array = ft_calloc(sizeof(char *), (count_columns(s, c) + 1));
	if (!array)
		return (NULL);
	return (make_array(s, array, c));
}
