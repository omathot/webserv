/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_memcmp.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/10 14:42:19 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/12 10:13:29 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	int				i;
	unsigned char	*k;
	unsigned char	*p;

	i = 0;
	k = (unsigned char *) s1;
	p = (unsigned char *) s2;
	while (n)
	{
		if (k[i] != p[i])
		{
			return (k[i] - p[i]);
		}
		n--;
		i++;
	}
	return (0);
}
