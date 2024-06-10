/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_lstmap.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: omathot <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/10/25 11:55:13 by omathot       #+#    #+#                 */
/*   Updated: 2022/10/25 13:18:52 by omathot       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_l;
	t_list	*new_n;

	new_l = NULL;
	if (!lst || !f || !del)
		return (NULL);
	while (lst)
	{
		new_n = ft_lstnew(f(lst -> content));
		if (!new_n)
		{
			ft_lstclear(&new_l, del);
			return (NULL);
		}
		ft_lstadd_back(&new_l, new_n);
		lst = lst -> next;
	}
	return (new_l);
}
