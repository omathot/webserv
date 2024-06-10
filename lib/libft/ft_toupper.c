/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_toupper.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/10 10:04:25 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/12 09:42:23 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

int	ft_toupper(int c)
{
	unsigned char	i;

	if (c >= 'a' && c <= 'z')
	{
		i = c - 32;
	}
	else
	{
		return (c);
	}
	return (i);
}
