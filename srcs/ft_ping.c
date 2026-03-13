/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 15:41:53 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/13 17:13:49 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// 1. Parse arguments  -> done
// 2. Resolve hostname (if necessary)  -> done
// -. Setup RAW Socket -> done ?
// === PING LOOP === // to do 
// 3. Construct ip + IMCP header -> done
// 4. Wrap it in ip header (automatic ?) -> automatic
// 4. Start timer and send packet
// 5. Receive packet and calculate time
// 6. Print it
// ===  END LOOP ===
// 7. Handle signals for ctrl c (and sigalarm ?)

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
	exit(EXIT_FAILURE);
}

void	receive_packet(t_ctx *context)
{
	struct sockaddr_in 	sender;
	socklen_t			sender_len = sizeof(sender);
	char 				buff[1024] = {0};
	int					bytes_read;

	bytes_read = recvfrom(context->socket, buff, sizeof(buff), 0, 
		(struct sockaddr *)&sender, &sender_len);
	if (bytes_read == -1)
	{
		perror("recvfrom");
		exit_error(context);
	}
}

void	send_packet(t_ctx *context, t_icmpping *ping)
{
	int err = sendto(context->socket, (const void *)ping, sizeof(*ping), 0,
		(struct sockaddr *)context->dest->ai_addr, context->dest->ai_addrlen);
	printf("sizeof packet = %ld\n", sizeof(*ping));
	if (err == -1)
	{
		perror("sendto");
		exit_error(context);
	}
	printf("bytes sent : %d\n", err);
}

void	ping_loop(t_ctx *context)
{
	struct icmphdr 	icmp_header;
	t_icmpping		ping;
	int 			seq = 0;

	// printf("PING %s (%s): 56 data bytes\n", context->hostname, getnameinfo(context->dest));
	while (1)
	{
		fill_icmphdr(&icmp_header, &seq);
		ping.header = icmp_header;
		memset((void *)ping.payload, 0x42, 56);
		calculate_checksum(&ping);
		send_packet(context, &ping);
		receive_packet(context);
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
	parse_args(argv + 1, &context.hostname);
	resolve_host(context.hostname, &context.dest);
	setup_socket(&context.socket, context.dest);
	ping_loop(&context);
}

