/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:21:58 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/23 17:22:00 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

int		set_socket_options(int *socket, int *err)
{
	t_ctx	*context = get_context();
	timeval	timeout = {1, 0};

	if (context->options.ttl)
	{
		int ttl = context->options.ttl_value;
		*err = setsockopt(*socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		if (*err == -1)
			return (-1);
	}
	if (context->options.wait)
		timeout.tv_sec = context->options.wait_value;
	*err = setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	if (*err == -1)
		return (-1);
	return (0);
}

void	setup_socket(int *sock, struct addrinfo *res_list)
{
	int	opt_error = 0;

	for (struct addrinfo *curr = res_list; curr != NULL; curr = curr->ai_next)
	{
		*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (*sock != -1)
		{
			set_socket_options(sock, &opt_error);
			break ;
		}
	}
	if (*sock == -1 || opt_error == -1)
	{
		if (*sock == -1)
			perror("socket");
		else if (opt_error == -1)
			perror("setsockopt");
		exit_error(-1);
	}
}

void	get_readable_ip_str(struct sockaddr *ai_addr, char *ipaddr_str)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)ai_addr;
	struct in_addr ipAddr = addr->sin_addr;
	inet_ntop(AF_INET, &ipAddr, ipaddr_str, INET_ADDRSTRLEN);
}

void	resolve_host(char *host, struct addrinfo **dest)
{
	t_ctx				*context = get_context();
	struct addrinfo		hints;
	int					err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_RAW; // raw socket
	err = getaddrinfo(host, NULL, &hints, &(*dest));
	if (err != 0)
	{
		if (err == -2)
			dprintf(STDERR, "ping: unknown host\n");
		else
			perror("getaddrinfo");
		exit_error(-1);
	}
	get_readable_ip_str((*dest)->ai_addr, context->source_dest_ip);
}
