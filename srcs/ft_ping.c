/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 15:41:53 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/11 17:16:33 by kipouliq         ###   ########.fr       */
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
	uint8_t 	*ptr;

	ptr = (uint8_t *)icmp_header;
	for (int i = 0; i < 5; i++)
		printf("ptr %d = %08X\n", i, ptr[i]);
	for (int i = 0; i < 10; i += 2)
	{
		printf("ptr = %04X, ptr + 1 = %04X, combined = %04X\n", ptr[i], ptr[i + 1], ptr[i] << 8 | ptr[ i + 1]);
		res += (ptr[i] << 8 | ptr[i + 1]);
		printf("res = %04X\n", res);
		if (res > 0xFFFF)
		{
			res += res >> 16;
			
		}
	}
	printf("res = %08X\n", res);
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

void	ping_loop(int *socket)
{
	struct icmphdr 	icmp_header;
	int 			seq = 0;

	(void) socket;
	while (1)
	{
		fill_icmphdr(&icmp_header, &seq);
		break ;
	}
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
	ping_loop(&socket);
	freeaddrinfo(res);
}
