/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linked_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:21:51 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/23 17:21:53 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void ft_lstdelone(t_lst **lst, t_lst *to_del, void (*del)(void *))
{
    t_lst *prev = NULL;
    t_lst *curr;

    if (!lst || !*lst || !to_del || !del)
        return;

    curr = *lst;
    if (curr == to_del)
    {
        *lst = curr->next;
        del(curr->content);
        del(curr);
        return;
    }
    while (curr)
    {
        if (curr == to_del)
        {
            prev->next = curr->next;
            del(curr->content);
            del(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
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

int	ft_lstsize(t_lst *lst)
{
	int	size;

	size = 0;
	if (!lst)
		return (0);
	while (lst)
	{
		lst = lst->next;
		size++;
	}
	return (size);
}

float   calculate_avg(t_lst **lst)
{
    float   avg	  = 0;
    int     count = 0;

    for (t_lst *curr = *lst; curr; curr = curr->next)
    {
        avg += *(float *)curr->content;
        count++;
    }
    return (avg / count);
}

float	calculate_avg_ftab(float *tab, int size)
{
	float avg = 0;

	for (int i = 0; i < size; i++)
		avg += tab[i];
	return (avg / size);
}

float	get_min(t_lst **lst)
{
	float 	min = *(float*)(*lst)->content;

	for (t_lst *curr = *lst; curr; curr = curr->next)
	{
		if (*(float *)curr->content < min) 
			min = *(float *)curr->content;
	}
	return min;
}

float	get_max(t_lst **lst)
{
	float 	max = *(float*)(*lst)->content;

	for (t_lst *curr = *lst; curr; curr = curr->next)
	{
		if (*(float *)curr->content > max) 
			max = *(float *)curr->content;
	}
	return max;
}

float	get_standard_deviation(t_lst **lst)
{
	float *deviations = NULL;
	float variance = 0;
	float avg = calculate_avg(lst);
	int lst_size = ft_lstsize(*lst);
	
	deviations = malloc(sizeof(float) * (lst_size + 1));
	if (!deviations)
		exit_error(-1);

	t_lst *current = *lst;
	for (int i = 0; i < lst_size; i++)
	{
		float curr = *(float*)current->content; 
		deviations[i] = (curr - avg) * (curr - avg);
		current = current->next;
	}
	variance = calculate_avg_ftab(deviations, lst_size);
	free(deviations);
	return (sqrt(variance));
}

void	store_time(t_ctx *context, float time)
{
	t_lst	*node;
	float	*new_time;

	new_time = malloc(sizeof(float));
	if (!new_time)
		exit_error(-1);
	*new_time = time;
	node = ft_lstnew(new_time);
	if (!node)
		exit_error(-1);
	ft_lstadd_back(&context->times, node);
}
