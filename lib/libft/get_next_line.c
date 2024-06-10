/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omathot <omathot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 12:29:04 by oscarmathot       #+#    #+#             */
/*   Updated: 2023/04/10 11:54:42 by omathot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*read_line(int fd, char *str)
{
	int		bytes_read;
	char	buffer[BUFFER_SIZE + 1];

	bytes_read = 1;
	buffer[0] = '\0';
	if (!str)
		str = ft_strdup("\0");
	if (!str)
		return (NULL);
	while (!ft_strchr(buffer, '\n') && bytes_read != 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (free(str), NULL);
		buffer[bytes_read] = '\0';
		str = ft_strjoin(str, buffer);
		if (!str)
			return (NULL);
	}
	if (!str[0])
		return (free(str), NULL);
	return (str);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*buff;

	if (fd < 0 || BUFFER_SIZE < 0 || fd > MAX_FD || BUFFER_SIZE > INT_MAX)
		return (NULL);
	str = read_line(fd, str);
	if (!str)
		return (NULL);
	buff = get_current(str);
	if (!buff)
	{
		free (str);
		str = NULL;
		return (NULL);
	}
	str = manage_buff(str);
	return (buff);
}

char	*ft_strcpy(char *s1, char *s2)
{
	size_t	i;

	i = 0;
	if (!s2)
		return (NULL);
	while (s2[i])
	{
		s1[i] = s2[i];
		i++;
	}
	s1[i] = '\0';
	return (s1);
}
