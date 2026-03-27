/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packet_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:22:03 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/27 11:57:28 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void		calculate_checksum(t_icmpping *ping)
{	
	int 		res = 0;
	uint16_t 	*ptr;

	ptr = (uint16_t *)ping;
	for (int i = 0; i < (int)sizeof(*ping) / 2; i++)
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

void		fill_icmphdr(struct icmphdr *icmp_header, int *seq, int *id)
{
	icmp_header->type 				= ICMP_ECHO;
	icmp_header->code 				= 0;
	icmp_header->checksum 			= 0;
	icmp_header->un.echo.id 		= htons(*id);
	icmp_header->un.echo.sequence 	= htons(*seq);
}

void		cast_packet(void)
{
	t_ctx			*context = get_context();
	t_pkt			*current_pkt = &context->current_pkt;

	current_pkt->ip_header = (iphdr *)current_pkt->raw_content;
	current_pkt->icmp_header = (icmphdr *)(current_pkt->raw_content + current_pkt->ip_header->ihl * 4);
}

int			receive_packet(void)
{
	t_ctx			*context = get_context();
	unsigned char	*raw_content = context->current_pkt.raw_content;
	int				bytes_read;

	memset(raw_content, 0, 1024);
	bytes_read = recvfrom(context->socket, raw_content, 1024, 0, 
		(struct sockaddr *)&context->current_pkt.sender, &context->current_pkt.sender_len);
	if (bytes_read == -1)
	{
		if (errno == EAGAIN)
			return (-1);
		else
		{
			perror("recvfrom");
			exit_error(-1);
		}
	}
	context->current_pkt.bytes_read = bytes_read;
	cast_packet();
	if (ntohs(context->current_pkt.icmp_header->un.echo.id) != context->id 
		&& context->current_pkt.icmp_header->type == 0) // == packet destination is another instance
	{
		return (receive_packet());
	}
	return (0);
}

void		send_packet(t_icmpping *ping)
{
	t_ctx	*context = get_context();

	prep_ping_packet(ping);

	int err = sendto(context->socket, (const void *)ping, sizeof(*ping), 0,
		(struct sockaddr *)context->dest->ai_addr, context->dest->ai_addrlen);
	if (err == -1)
	{
		perror("sendto");
		exit_error(-1);
	}
}

uint8_t		hex_to_int(char c)
{
	char	hex[] = "0123456789abcdef";

	for (int i = 0; hex[i]; i++)
	{
		if (hex[i] == c)
			return (i);
	}
	return (-1);
}

void		fill_pkt_payload(t_icmpping *packet)
{
	char		*pattern = get_context()->options.pattern_value;
	int			pattern_size = strlen(pattern);
	uint8_t		*ptr = (uint8_t *)packet->payload;
	int			payload_size = sizeof(packet->payload);
	int			j = 0;
	
	for (int i = 0; i < payload_size; i++)
	{
		uint8_t a = hex_to_int(pattern[j]);
		uint8_t b = hex_to_int(pattern[j + 1]);
		if (!pattern[j + 1])
			*(ptr + i) = (0x00 << 4 | a);
		else
			*(ptr + i) = (a << 4 | b);
		j += 2;
		if ((pattern_size % 2 == 0 && j >= pattern_size - 1)
			|| (pattern_size % 2 != 0 && j > pattern_size))
			j = 0;
	}
}

void		prep_ping_packet(t_icmpping *ping_packet)
{
	t_ctx 			*context = get_context();
	icmphdr 		icmp_header;

	fill_icmphdr(&icmp_header, &context->seq, &context->id);
	gettimeofday(&ping_packet->timestamp, NULL);
	ping_packet->header = icmp_header;
	if (context->options.pattern)
		fill_pkt_payload(ping_packet);
	else
		memset((void *)ping_packet->payload, 0x42, 56);
	calculate_checksum(ping_packet);
}

