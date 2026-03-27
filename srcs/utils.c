/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:22:22 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/24 10:41:33 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void	exit_error(int code)
{
	t_ctx *context = get_context();
		
	if (context->dest)
		freeaddrinfo(context->dest);
	if (context->socket != -1)
		close(context->socket);
	if (context->hostname)
		free(context->hostname);
	if (context->times)
		ft_lstclear(&context->times, free);
	if (context->args_lst)
		ft_lstclear(&context->times, free);
	exit(code);
}

void	init_options(t_opt *options)
{
	options->interval = false;
	options->pattern = false;
	options->timeout = false;
	options->ttl = false;
	options->verbose = false;
	options->wait = false;

	options->pattern_value = NULL;
	options->ttl_value = -1;
	options->timeout_value = -1;
	options->wait_value = -1;
}

t_ctx	*get_context(void)
{
	static bool	 	init = false;
	static t_ctx 	context;

	if (!init)
	{
		context.hostname = NULL;
		context.dest = NULL;
		context.times = NULL;
		context.args_lst = NULL;
		context.seq = 0;
		context.socket = -1;
		context.id = getpid();
		memset(context.source_dest_ip, 0, INET_ADDRSTRLEN);
		init = true;
		init_options(&context.options);
		return (&context);
	}
	return (&context);
}

float	get_time_elapsed(struct timeval *starting_time)
{
	struct timeval	current_time;
	float			time_elapsed;

	gettimeofday(&current_time, NULL);
	time_elapsed = ((current_time.tv_sec - starting_time->tv_sec) * 1000)
		+ ((current_time.tv_usec - starting_time->tv_usec) * 0.001);
	return (time_elapsed);
}
