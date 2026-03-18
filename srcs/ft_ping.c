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

// 1. Parse arguments  -> done
// 2. Resolve hostname (if necessary)  -> done
// -. Setup RAW Socket -> done ?
// === PING LOOP === // 
// 3. Construct ip + IMCP header -> done
// 4. Wrap it in ip header (automatic ?) -> automatic, done 
// -. start timer -> done
// 4. send packet -> done
// 5. Receive packet -> done
// -. calculate time -> done but buggy
// 6. Print it -> done
// ===  END LOOP ===
// -. STATS :
// - store individual times -> done
// - calculate : min/avg/max/stddev -> done
// 7. Handle signals for ctrl c (and sigalarm ?) -> done
// ! - VERBOSE

// BUGS
// -. Seq start at 0 on inetutils ffs
// -. Timestamps sometimes wrong
// -. more than 1 ping is messed up

// if sender != dest -> packet error

#include "../ft_ping.h"

void	exit_error(void)
{
	t_ctx *context = get_context();

	freeaddrinfo(context->dest);
	close(context->socket);
	ft_lstclear(&context->times, free);
	exit(EXIT_FAILURE);
}

void	setup_socket(int *sock, struct addrinfo *res_list)
{
	struct timeval	timeout = {1, 0};
	int				opt_error = 0;

	for (struct addrinfo *curr = res_list; curr != NULL; curr = curr->ai_next)
	{
		*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (*sock)
		{
			int ttl = 1;
			opt_error = setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			opt_error = setsockopt(*sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
			break ;
		}
	}
	if (*sock == -1 || opt_error == -1)
	{
		if (*sock == -1)
			perror("socket");
		else if (opt_error == -1)
			perror("setsockopt");
		exit_error();
	}
}

void	resolve_host(char *host, struct addrinfo **dest)
{
	struct addrinfo	hints;
	int				err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_RAW; // raw socket
	err = getaddrinfo(host, NULL, &hints, &(*dest));
	if (err != 0) // handle other cases
	{
		if (err == -2)
			dprintf(STDERR, "ping: unknown host\n");
		else
			perror("getaddrinfo");
		exit_error();
	}
}

void	calculate_checksum(t_icmpping *ping)
{	
	int 		res = 0;
	uint16_t 	*ptr;

	ptr = (uint16_t *)ping;
	for (int i = 0; i < 32; i++)
	{
		res += ptr[i];
		if (res > 0xFFFF)
		{
			res += res >> 16;
			res = 0xFFFF & res;
		}
	}
	ping->header.checksum = ~res;
}

void	fill_icmphdr(struct icmphdr *icmp_header, int *seq)
{
	icmp_header->type 				= ICMP_ECHO;
	icmp_header->code 				= 0;
	icmp_header->checksum 			= 0;
	icmp_header->un.echo.id 		= htons(getpid());
	printf("header id = %d\n", icmp_header->un.echo.id);
	icmp_header->un.echo.sequence 	= htons(*seq);
}

int		receive_packet(t_ctx *context, uint8_t *ttl)
{
	struct sockaddr_in 	sender;
	struct sockaddr_in  *dest;
	socklen_t			sender_len = sizeof(sender);
	char 				buff[1024] = {0};
	int					bytes_read;

	dest = (struct sockaddr_in *)context->dest->ai_addr;
	bytes_read = recvfrom(context->socket, buff, sizeof(buff), 0, 
		(struct sockaddr *)&sender, &sender_len);
	if (bytes_read == -1)
	{
		if (errno == EAGAIN)
			return (-1);
		else
		{
			perror("recvfrom");
			exit_error();
		}
	}
	//////
	struct iphdr *ip = (struct iphdr *)buff;
	int ip_header_len = ip->ihl * 4;  // ihl est en mots de 4 octets
	struct icmphdr *icmp = (struct icmphdr *)(buff + ip_header_len);

	printf("%d\n", icmp->type);
	/////
	if (sender.sin_addr.s_addr != dest->sin_addr.s_addr) // PACKET ERROR
	{
		printf("wrong sender detected\n");
		return (receive_packet(context, ttl));
	}
	if (!context->source_dest_ip[0])
		get_readable_ip_str((struct sockaddr *)&sender, context->source_dest_ip);
	*ttl = ((struct iphdr *)buff)->ttl;
	context->ping_successes += 1;
	return (bytes_read);
}

void	send_packet(t_ctx *context, t_icmpping *ping)
{
	int err = sendto(context->socket, (const void *)ping, sizeof(*ping), 0,
		(struct sockaddr *)context->dest->ai_addr, context->dest->ai_addrlen);
	if (err == -1)
	{
		perror("sendto");
		exit_error();
	}
}

void	get_readable_ip_str(struct sockaddr *ai_addr, char *ipaddr_str)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)ai_addr;
	struct in_addr ipAddr = addr->sin_addr;
	inet_ntop(AF_INET, &ipAddr, ipaddr_str, INET_ADDRSTRLEN);
}

void	prep_ping_packet(t_icmpping *ping_packet, int *seq)
{
	struct icmphdr 	icmp_header;

	fill_icmphdr(&icmp_header, seq);
	ping_packet->header = icmp_header;
	memset((void *)ping_packet->payload, 0x42, 56);
	calculate_checksum(ping_packet);
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

void	store_time(t_ctx *context, float time)
{
	t_lst	*node;
	float	*new_time;

	new_time = malloc(sizeof(float));
	if (!new_time)
		exit_error();
	*new_time = time;
	node = ft_lstnew(new_time);
	if (!node)
		exit_error();
	ft_lstadd_back(&context->times, node);
}

t_ctx	*get_context(void)
{
	static bool	 	init = false;
	static t_ctx 	context;

	if (!init)
	{
		context.hostname = NULL;
		context.times = NULL;
		context.seq = 1;
		memset(context.source_dest_ip, 0, INET_ADDRSTRLEN);
		init = true;
		return (&context);
	}
	return (&context);
}

void	sigint_handler(int code)
{
	t_ctx			*context = get_context();
	int				packet_loss = 100 - (context->ping_successes / context->seq * 100);

	(void) code;
	printf("--- %s ping statistics ---\n", context->hostname);
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
		context->seq, context->ping_successes, packet_loss );
	
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

void	ping_loop(t_ctx *context)
{
	t_icmpping		ping_packet;
	char			ipaddr_str[INET_ADDRSTRLEN];
	struct timeval  start;
	int				bytes_read;
	float			time_elapsed;
	uint8_t			ttl;

	get_readable_ip_str(context->dest->ai_addr, ipaddr_str);
	
	printf("PING %s (%s): %ld bytes of data.\n", 
		context->hostname, ipaddr_str, sizeof(ping_packet.payload));

	while (1)
	{
		prep_ping_packet(&ping_packet, &context->seq);
		gettimeofday(&start, NULL);
		send_packet(context, &ping_packet);
		bytes_read = receive_packet(context, &ttl);
		if (bytes_read > 0)
		{
			time_elapsed = get_time_elapsed(&start);
			store_time(context, time_elapsed);
			printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
				bytes_read - sizeof(struct iphdr), 
				context->source_dest_ip,
				context->seq, 
				ttl, 
				time_elapsed);
				sleep(1);
		}
		context->seq++;
	}
}

void	setup_signal(void)
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
}

int		main(int argc, char **argv)
{
	t_ctx			*context;

	if (argc < 2) // Will change if bonuses implemented
	{
		dprintf(STDERR, "ping: missing host operand\n \
			Try 'ping -?' for more information.\n");
			exit(64);
	}
	context = get_context();
	parse_args(argv + 1, &context->hostname);
	resolve_host(context->hostname, &context->dest);
	setup_socket(&context->socket, context->dest);
	setup_signal();
	ping_loop(context);
}

