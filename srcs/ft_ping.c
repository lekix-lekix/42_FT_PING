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
// - store individual times
// - calculate : min/avg/max/stddev
// 7. Handle signals for ctrl c (and sigalarm ?)

// secure ping by dest addr ? check with two instances running

#include "../ft_ping.h"

void	setup_socket(int *sock, struct addrinfo *res_list)
{
	for (struct addrinfo *curr = res_list; curr != NULL; curr = curr->ai_next)
	{
		*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (*sock)
			break ;
	}
	if (*sock == -1)
	{
		perror("socket");
		freeaddrinfo(res_list);
		exit(-1);
	}
}

void	resolve_host(char *host, struct addrinfo **dest)
{
	struct addrinfo	hints;
	int				err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_RAW;
	err = getaddrinfo(host, NULL, &hints, &(*dest));
	if (err != 0) // handle other cases
	{
		if (err == -2)
		{
			dprintf(STDERR, "ping: unknown host\n");
			freeaddrinfo(*dest);
			exit(1);
		}
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
	icmp_header->type = ICMP_ECHO;
	icmp_header->code = 0;
	icmp_header->checksum = 0; // to calculate
	icmp_header->un.echo.id = htons(getpid());
	icmp_header->un.echo.sequence = htons(*seq);
}

void	exit_error(t_ctx *context)
{
	freeaddrinfo(context->dest);
	close(context->socket);
	ft_lstclear(&context->times, free);
	exit(EXIT_FAILURE);
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
	if (sender.sin_addr.s_addr != dest->sin_addr.s_addr)
		return (-1);
	if (bytes_read == -1)
	{
		perror("recvfrom");
		exit_error(context);
	}
	*ttl = ((struct iphdr *)buff)->ttl;
	return (bytes_read);
}

void	send_packet(t_ctx *context, t_icmpping *ping)
{
	int err = sendto(context->socket, (const void *)ping, sizeof(*ping), 0,
		(struct sockaddr *)context->dest->ai_addr, context->dest->ai_addrlen);
	if (err == -1)
	{
		perror("sendto");
		exit_error(context);
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
		exit_error(context);
	*new_time = time;
	node = ft_lstnew(new_time);
	if (!node)
		exit_error(context);
	ft_lstadd_back(&context->times, node);
}

void	ping_loop(t_ctx *context)
{
	t_icmpping		ping_packet;
	char			ipaddr_str[INET_ADDRSTRLEN];
	struct timeval  start;
	int 			seq = 1;
	int				bytes_read;
	float			time_elapsed;
	uint8_t			ttl;

	get_readable_ip_str(context->dest->ai_addr, ipaddr_str);

	printf("PING %s (%s): %ld(%ld) bytes of data.\n", 
		context->hostname, ipaddr_str, sizeof(ping_packet.payload),
		sizeof(ping_packet) + sizeof(struct iphdr));

	while (1)
	{
		prep_ping_packet(&ping_packet, &seq);
		gettimeofday(&start, NULL);
		send_packet(context, &ping_packet);
		bytes_read = receive_packet(context, &ttl);
		time_elapsed = get_time_elapsed(&start);
		store_time(context, time_elapsed);

		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f\n", // refactor and add : real source ip addr from sender
			bytes_read - sizeof(struct iphdr), 
			ipaddr_str,
			seq, 
			ttl, 
			get_time_elapsed(&start));
		
		printf("avg = %.3f\n", calculate_avg(&context->times));

		sleep(1);
		seq++;
	}
}

int	main(int argc, char **argv)
{
	t_ctx			context;

	if (argc < 2) // Will change if bonuses implemented
	{
		dprintf(STDERR, "ping: missing host operand\n \
			Try 'ping -?' for more information.\n");
			exit(64);
	}
	
	context.hostname = NULL;
	context.times = NULL;
	parse_args(argv + 1, &context.hostname);
	resolve_host(context.hostname, &context.dest);
	setup_socket(&context.socket, context.dest);
	ping_loop(&context);
}

