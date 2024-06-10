/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_tolower.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/10 10:25:34 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/11 11:08:24 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

int	ft_tolower(int c)
{
	unsigned char	i;

	if (c >= 'A' && c <= 'Z')
	{
		i = c + 32;
	}
	else
	{
		return (c);
	}
	return (i);
}
