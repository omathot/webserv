/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omathot <omathot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 12:37:02 by oscarmathot       #+#    #+#             */
/*   Updated: 2023/04/17 13:10:56 by omathot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*get_current(char *str)
{
	size_t	i;
	char	*current;

	i = 0;
	if (!str)
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		i += 2;
	else
		i += 1;
	current = malloc(i);
	if (!current)
		return (NULL);
	i = 0;
	while (str[i] && str[i] != '\n')
	{
		current[i] = str[i];
		i++;
	}
	if (str[i] == '\n')
		current[i++] = '\n';
	current[i] = '\0';
	return (current);
}

char	*manage_buff(char *str)
{
	size_t	i;
	size_t	j;
	char	*new_str;

	i = 0;
	j = 0;
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		i++;
	j = ft_strlen(&str[i]);
	new_str = malloc(j + 1);
	if (!new_str)
		return (free(str), NULL);
	ft_strcpy(new_str, str + i);
	free(str);
	return (new_str);
}
