/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_strnstr.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/11 09:43:55 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/26 12:02:45 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	c;
	char	*ptr;

	i = 0;
	c = 0;
	if (*needle == 0)
		return ((char *)haystack);
	if (*haystack == 0 || len == 0)
		return ((char *)0);
	while (haystack[i] && i < len)
	{
		while (haystack[i + c] == needle[c] && needle[c] && (i + c) < len)
			c++;
		ptr = (char *)&haystack[i];
		if (needle[c] == 0)
			return (ptr);
		i++;
		c = 0;
	}
	return (NULL);
}
