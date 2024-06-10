/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_strtrim.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/18 11:14:41 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/26 10:46:39 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

static int	ft_cutleft(const char *s1, const char *set)
{
	size_t		j;
	char		*char_addr;

	j = 0;
	char_addr = ft_strchr(set, (int)s1[j]);
	j++;
	while (char_addr && j < ft_strlen(s1))
	{
		char_addr = ft_strchr(set, (int)s1[j]);
		j++;
	}
	return (j - 1);
}

static int	ft_cutright(const char *s1, const char *set)
{
	int		i;
	char	*char_addr;

	i = ft_strlen(s1) - 1;
	char_addr = ft_strchr(set, (int)s1[i]);
	while (char_addr && i)
	{
		i--;
		char_addr = ft_strchr(set, (int)s1[i]);
	}
	return (i);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	int		beginning;
	int		end;
	char	*ptr;

	if (!s1)
		return (NULL);
	if (!set)
		return (ft_strdup(s1));
	beginning = ft_cutleft(s1, set);
	end = ft_cutright(s1, set);
	if (beginning > end)
		return (ft_strdup(""));
	ptr = ft_substr(s1, beginning, (end - beginning + 1));
	return (ptr);
}
