/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscarmathot <oscarmathot@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 10:30:28 by omathot           #+#    #+#             */
/*   Updated: 2023/04/07 23:55:38 by oscarmathot      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *str, int c)
{
	int	i;

	i = ft_strlen(str);
	if (!str)
		return (0);
	if (str[i] == (char) c)
		return ((char *) &str[i]);
	while (i--)
		if (str[i] == (char) c)
			return ((char *)&str[i]);
	return (0);
}

char	*ft_strchr2(char *str, int c)
{
	int	i;

	i = ft_strlen(str);
	if (!str)
		return (0);
	if (str[i] == (char) c)
		return ((char *) &str[i]);
	while (i--)
		if (str[i] == (char) c)
			return ((char *)&str[i]);
	return (0);
}
