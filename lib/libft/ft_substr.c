/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_substr.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/18 09:27:06 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/26 10:19:34 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	size_t		i;
	size_t		l;
	char		*c;

	i = 0;
	l = 0;
	if ((!s || !*s) || ft_strlen(s) <= start)
		return (ft_strdup(""));
	if ((ft_strlen(s) - start) < len)
		len = ft_strlen(s) - start;
	c = (char *)malloc(sizeof(*s) * (len + 1));
	if (!c)
		return (NULL);
	while (s[i])
	{
		if (i >= start && l < len)
		{
			c[l] = s[i];
			l++;
		}
		i++;
	}
	c[l] = '\0';
	return (c);
}
