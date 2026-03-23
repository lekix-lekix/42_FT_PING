/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 15:41:53 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/16 11:44:26 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// BONUSES
// --ttl
// -f flood
// -p pattern (fill payload)
// -i interval
// -w timeout

#include "../ft_ping.h"

#define TIMER_START(t) struct timespec t##_start, t##_end; \
                       clock_gettime(CLOCK_MONOTONIC, &t##_start)

#define TIMER_STOP(t)  clock_gettime(CLOCK_MONOTONIC, &t##_end)

#define TIMER_MS(t)    ((t##_end.tv_sec  - t##_start.tv_sec)  * 1000.0 \
                      + (t##_end.tv_nsec - t##_start.tv_nsec) / 1e6)

void	sigint_handler(int code)
{
	t_ctx			*context = get_context();
	int				packet_loss = 100 - (context->ping_successes / (context->seq) * 100);

	(void) code;
	printf("--- %s ping statistics ---\n", context->hostname);
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
		context->seq, context->ping_successes, packet_loss);
	
	if (context->times)
	{
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			get_min(&context->times),
			calculate_avg(&context->times),
			get_max(&context->times),
			get_standard_deviation(&context->times)
		);
		ft_lstclear(&context->times, free);
	}
	freeaddrinfo(context->dest);
	close(context->socket);
	exit(0);
}

void	setup_signal(void)
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
}

void	ping_loop(t_ctx *context)
{
	t_icmpping		ping_packet;
	struct timeval  start;
	float			time_elapsed;

	print_begin_output(&ping_packet);

	while (1)
	{
		gettimeofday(&start, NULL);
		send_packet(&ping_packet);
		context->seq++;
		if (receive_packet() == -1) // timeout
			continue;
		time_elapsed = get_time_elapsed(&start);
		if (context->current_pkt.icmp_header->type == 0) // success
		{
			context->ping_successes += 1;
			store_time(context, time_elapsed);
			print_success_output(&time_elapsed);
		}
		else
			print_error_output();
		if (context->options.flood == false)
			sleep(1);
	}
}

int		main(int argc, char **argv)
{
	t_ctx			*context;

	if (argc < 2)
	{
		dprintf(STDERR, "ping: missing host operand\nTry 'ping -?' for more information.\n");
			exit(64);
	}
	context = get_context();
	parse_args(argv + 1, &context->hostname);
	resolve_host(context->hostname, &context->dest);
	setup_socket(&context->socket, context->dest);
	setup_signal();
	ping_loop(context);
}

