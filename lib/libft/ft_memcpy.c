/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_temcpy.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/06 11:53:46 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/22 10:11:16 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t		i;
	char		*c;
	char		*k;

	i = 0;
	c = dst;
	k = (char *)src;
	if (!n || dst == src)
		return (dst);
	while (i < n)
	{
		c[i] = k[i];
		i++;
	}
	return (dst);
}
