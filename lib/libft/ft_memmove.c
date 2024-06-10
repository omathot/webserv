/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_memmove.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/06 12:44:36 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/26 09:55:54 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	char	*d;
	char	*s;

	d = (char *)dst;
	s = (char *)src;
	if (!src && !dst)
		return (NULL);
	if (src < dst)
	{
		d = d + len - 1;
		s = s + len - 1;
		while (len--)
		{
			*d-- = *s--;
		}
	}
	else if (src >= dst)
	{
		while (len--)
		{
			*d++ = *s++;
		}
	}
	return (dst);
}
