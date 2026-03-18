#include "../ft_ping.h"

t_lst	*ft_lstnew(void *content)
{
	t_lst	*new_node;

	new_node = malloc(sizeof(t_lst));
	if (!new_node)
		return (NULL);
	new_node->content = content;
	new_node->next = NULL;
	return (new_node);
}

void	ft_lstadd_back(t_lst **lst, t_lst *new)
{
	t_lst	*list;

	list = *lst;
	if (!list)
	{
		*lst = new;
		return ;
	}
	while (list->next)
		list = list->next;
	if (!new)
		list->next = NULL;
	else
		list->next = new;
}

void	ft_lstclear(t_lst **lst, void (*del)(void *))
{
	t_lst	*next;

	if (!del || !lst)
		return ;
	while (*lst)
	{
		next = (*lst)->next;
		del((*lst)->content);
		free(*lst);
		*lst = next;
	}
}

float   calculate_avg(t_lst **lst)
{
    float   avg = 0;
    int     count = 0;

    for (t_lst *curr = *lst; curr; curr = curr->next)
    {
        avg += *(float *)curr->content;
        count++;
    }
    return (avg / count);
}