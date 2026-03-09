/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 15:41:53 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/09 16:40:44 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// 1. Parse arguments
// 2. Resolve hostname (if necessary) && init raw socket
// 3. Construct custom IMCP header
// 4. Wrap it in ip header (automatic ?)
// === PING LOOP ===
// 4. Start timer and send packet
// 5. Receive packet and calculate time
// 6. Print it
// ===  END LOOP ===
// 7. Handle signals for ctrl c (and sigalarm ?)

#include "../ft_ping.h"

void	resolve_host(char *host, struct addrinfo **res)
{
	struct addrinfo	hints;
	int				err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_RAW;
	err = getaddrinfo(host, NULL, &hints, &(*res));
	if (err != 0)
	{
		if (err == -2)
		{
			dprintf(STDERR, "ping: unknown host\n");
			exit(1);
		}
	}
}

int	main(int argc, char **argv)
{
	struct addrinfo *res;
	char			*host;

	if (argc < 2) // Will change if bonuses implemented
	{
		dprintf(STDERR, "ping: missing host operand\n \
			Try 'ping -?' for more information.\n");
			exit(64);
	}
	
	host = NULL;
	parse_args(argv + 1, &host);
	resolve_host(host, &res);
}
