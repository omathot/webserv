/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscarmathot <oscarmathot@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/07 11:45:28 by omathot           #+#    #+#             */
/*   Updated: 2023/04/07 23:55:52 by oscarmathot      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	n;
	size_t	len;

	n = 0;
	while (dst[n])
	{
		if (n == dstsize)
			return (ft_strlen(src) + dstsize);
		n++;
	}
	if (n == dstsize)
		return (ft_strlen(src) + dstsize);
	len = ft_strlen(dst) + ft_strlen(src);
	dst = dst + n;
	while (n < (dstsize - 1) && *src)
	{
		*dst = *src;
		dst++;
		src++;
		n++;
	}
	*dst = 0;
	return (len);
}
