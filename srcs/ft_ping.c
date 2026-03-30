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
// -i --interval=NUMBER interval Float
// -p --pattern=PATTERN pattern (fill payload) hex -> fill all bytes with first 2 chars taken, in hex
// -W --wait for N seconds for response	(INT_MAX)
// --ttl
// -w --timeout=N (MAX_INT) stop after N seconds 

// vagrant@debian12:~/ft_ping$ ping -p --ttl=1  1.1.1.1
// ping: error in pattern near --ttl=1

#include "../ft_ping.h"

void	sigint_handler(int code)
{
	t_ctx			*context = get_context();
	float			packet_loss;

	(void) code;

	if (context->ping_successes == 0)
		packet_loss = 100;
	else
		packet_loss = (float)context->seq / (float)context->ping_successes;

	printf("--- %s ping statistics ---\n", context->hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		context->seq, context->ping_successes, packet_loss == 1 ? 0 : packet_loss);
	
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
	if (context->hostname)
		free(context->hostname);
	if (context->options.pattern_value)
		free(context->options.pattern_value);
	close(context->socket);
	exit(0);
}

void	setup_signal(void)
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGALRM, &act, NULL);
}

void	ping_loop(t_ctx *context)
{
	t_icmpping		request;
	t_icmpping		*reply;
	struct timeval  start;
	float			time_elapsed;

	print_begin_output(&request);
	if (context->options.timeout)
		alarm(context->options.timeout_value);
	while (1)
	{
		gettimeofday(&start, NULL);
		send_packet(&request);
		context->seq++;
		if (receive_packet() == -1) // timeout
			continue;
		reply = (t_icmpping *)(context->current_pkt.raw_content 
			+ context->current_pkt.ip_header->ihl * 4);
		time_elapsed = get_time_elapsed(&reply->timestamp);
		if (context->current_pkt.icmp_header->type == 0) // success
		{
			context->ping_successes += 1;
			store_time(context, time_elapsed);
			print_success_output(&time_elapsed);
		}
		else
			print_error_output();
		if (context->options.interval)
		{
			sleep(context->options.interval_value.tv_sec);
			usleep(context->options.interval_value.tv_usec);
		}
		else
			sleep(1);
	}
}

int		main(int argc, char **argv)
{
	t_ctx			*context;

	(void) argc;
	context = get_context();
	parse_args(argv + 1);
	resolve_host(context->hostname, &context->dest);
	setup_socket(&context->socket, context->dest);
	setup_signal();
	ping_loop(context);
}

