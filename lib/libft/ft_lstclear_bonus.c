/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_lstclear.c                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/25 10:50:18 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/26 12:01:49 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*end;

	if (!del)
		return ;
	while (*lst)
	{
		del((*lst)->content);
		end = *lst;
		*lst = end -> next;
		free(end);
	}
	*lst = NULL;
}
