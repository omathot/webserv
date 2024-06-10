/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_memset.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/05 09:36:38 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/12 10:14:43 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*k;

	k = (unsigned char *)b;
	while (len > 0)
	{
		*k++ = c;
		len--;
	}
	return (b);
}
