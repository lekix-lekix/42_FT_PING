/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 15:41:53 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/12 17:08:51 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// 1. Parse arguments  -> done
// 2. Resolve hostname (if necessary)  -> done
// -. Setup RAW Socket -> done ?
// === PING LOOP ===
// 3. Construct ip + IMCP header
// 4. Wrap it in ip header (automatic ?)
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

void	resolve_host(char *host, struct addrinfo **res)
{
	struct addrinfo	hints;
	int				err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_RAW;
	err = getaddrinfo(host, NULL, &hints, &(*res));
	if (err != 0) // handle other cases
	{
		if (err == -2)
		{
			dprintf(STDERR, "ping: unknown host\n");
			freeaddrinfo(*res);
			exit(1);
		}
	}
}

void	calculate_checksum(struct icmphdr *icmp_header)
{	
	int 		res = 0;
	uint16_t 	*ptr;

	ptr = (uint16_t *)icmp_header;
	for (int i = 0; i < 4; i++)
	{
		res += ptr[i];
		if (res > 0xFFFF)
		{
			res += res >> 16;
			res = 0xFFFF & res;
		}
	}
	icmp_header->checksum = ~res;
	printf("checksum = %X\n", icmp_header->checksum);
}

void	fill_icmphdr(struct icmphdr *icmp_header, int *seq)
{
	(*seq) += 1;

	icmp_header->type = ICMP_ECHO;
	icmp_header->code = 0;
	icmp_header->checksum = 0; // to calculate
	icmp_header->un.echo.id = htons(getpid());
	icmp_header->un.echo.sequence = htons(*seq);
	
	calculate_checksum(icmp_header);
}

void	receive_msg(int *socket)
{
	struct sockaddr_in 	sender;
	socklen_t			sender_len = sizeof(sender);
	char 				buff[64] = {0};
	int					bytes_read;

	bytes_read = recvfrom(*socket, buff, sizeof(buff), 0, (struct sockaddr *)&sender, &sender_len);
	if (bytes_read == -1)
		perror("recvfrom");
	printf("bytes read = %d\n", bytes_read);
}

void	ping_loop(int *socket, struct addrinfo *dest)
{
	t_icmpping		ping;
	struct icmphdr 	icmp_header;
	int 			seq = 0;

	(void) socket;
	fill_icmphdr(&icmp_header, &seq);
	ping.header = icmp_header;
	memset(ping.payload, 0, 56);
	int err = sendto(*socket, (const void *)&ping, sizeof(ping), 0, (struct sockaddr *)dest->ai_addr, dest->ai_addrlen);
	if (err == -1)
	{
		perror("sendto");
	}
	printf("bytes sent : %d\n", err);
	receive_msg(socket);
}

int	main(int argc, char **argv)
{
	struct addrinfo *res;
	char			*host;
	int				socket;

	if (argc < 2) // Will change if bonuses implemented
	{
		dprintf(STDERR, "ping: missing host operand\n \
			Try 'ping -?' for more information.\n");
			exit(64);
	}
	
	host = NULL;
	parse_args(argv + 1, &host);
	resolve_host(host, &res);
	setup_socket(&socket, res);
	ping_loop(&socket, res);
	freeaddrinfo(res);
}
