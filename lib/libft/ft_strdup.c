/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_strdup.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/14 13:21:29 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/22 10:47:19 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_strdup(const char *s1)
{
	int		i;
	int		size;
	char	*c;

	if (s1 == NULL)
		return (NULL);
	i = 0;
	size = 0;
	if (s1 == NULL)
	{
		write(1, "error rull", 10);
		return (NULL);
	}
	while (s1[size])
		size++;
	c = malloc(sizeof(char) * (size + 1));
	if (!c)
		return (NULL);
	while (s1[i])
	{
		c[i] = s1[i];
		i++;
	}
	c[i] = '\0';
	return (c);
}
