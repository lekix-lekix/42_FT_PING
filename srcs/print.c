/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:22:14 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/23 17:22:17 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

int	print_help(void)
{
    printf("%s\n", "Usage: ping [OPTION...] HOST ...");
    printf("%s\n", "Send ICMP ECHO_REQUEST packets to network hosts.");
    printf("%s\n", "");
    printf("%s\n", " Valid options:");
    printf("%s\n", "");
    printf("%s\n", "  -v        verbose output");
    printf("%s\n", "  -?        give this help list");
    printf("%s\n", "  -V        print program version");
    printf("%s\n", "");
    printf("%s\n", "Report bugs to <do_not_report_bugs_please@42.fr>.");
    exit_error(0);
	return (0);
}

void print_version(void)
{
    printf("%s\n", "ft_ping (kipouliq's copy of GNU inetutils-ping) 1.0");
    printf("%s\n", "Copyright (C) 2026 Free Software Foundation, Inc.");
    printf("%s\n", "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.");
    printf("%s\n", "This is free software: you are free to change and redistribute it.");
    printf("%s\n", "There is NO WARRANTY, to the extent permitted by law.");
    printf("%s\n", "");
    printf("%s\n", "Written by kipouliq at 42.");
    exit_error(0);
}

char  *icmp_error_str(int type, int code)
{
    if (type == 3) // Destination Unreachable
    {
        switch (code)
        {
            case 0: return "Net Unreachable";
            case 1: return "Host Unreachable";
            case 2: return "Protocol Unreachable";
            case 3: return "Port Unreachable";
            case 4: return "Fragmentation Needed";
            case 5: return "Source Route Failed";
        }
    }
    else if (type == 11) // Time Exceeded
    {
        switch (code)
        {
            case 0: return "Time to live exceeded";
            case 1: return "Fragment reassembly time exceeded";
        }
    }
    else if (type == 12) // Parameter Problem
    {
        switch (code)
        {
            case 0: return "Pointer indicates the error";
            case 1: return "Missing a required option";
        }
    }
    return "Unknown";
}

uint16_t	get_frag_flags(uint16_t frag_off)
{
	frag_off = ntohs(frag_off);
	
	int reserved = (frag_off >> 15) & 1;
	int df = (frag_off >> 14) & 1;
	int mf = (frag_off >> 13) & 1;
	
	return (reserved | df << 1 | mf << 2);
}

uint16_t	get_frag_offset(uint16_t frag_off)
{
	return (ntohs(frag_off) & 0x0f);
}

void	print_success_output(float *time_elapsed)
{	
	t_ctx	*context = get_context();

	printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
		context->current_pkt.bytes_read - sizeof(iphdr),
		context->source_dest_ip,
		context->seq - 1, 
		context->current_pkt.ip_header->ttl, 
		*time_elapsed
	);
}

void	print_iphdr_hexdump(void)
{
	iphdr			*ip_outer = (iphdr *)get_context()->current_pkt.raw_content;
	icmphdr		 	*icmp_hdr = (icmphdr *)(get_context()->current_pkt.raw_content + ip_outer->ihl * 4);
	iphdr			*sent_iphdr = (iphdr *)((char *)icmp_hdr + 8);
	uint8_t 		*content = (uint8_t *)sent_iphdr;
	int				iphdr_size = sent_iphdr->ihl * 4;
	char			src_ip_addr[INET_ADDRSTRLEN];
	char			dest_ip_addr[INET_ADDRSTRLEN];
	
	inet_ntop(AF_INET, &sent_iphdr->saddr, src_ip_addr, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &sent_iphdr->daddr, dest_ip_addr, INET_ADDRSTRLEN);

	printf("%s", "IP Hdr Dump:\n ");
	for (int i = 0; i < iphdr_size; i++)
	{
		printf("%02x", content[i]);
		if (i % 2 != 0)
			printf(" ");
	}
	printf("\n");
	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
	printf(" %d  %d  %02x 00%02x %02x   %d %04d  %02d  %02d %04x %s  %s", 
		sent_iphdr->version, 
		sent_iphdr->ihl, 
		sent_iphdr->tos, 
		ntohs(sent_iphdr->tot_len), 
		ntohs(sent_iphdr->id),
		get_frag_flags(sent_iphdr->frag_off), 
		get_frag_offset(sent_iphdr->frag_off),
		sent_iphdr->ttl,
		sent_iphdr->protocol,
		ntohs(sent_iphdr->check),
		src_ip_addr,
		dest_ip_addr
	);
	printf("\n");
}

void	print_icmp(void)
{
	iphdr			*ip_outer = (iphdr *)get_context()->current_pkt.raw_content;
	icmphdr		 	*icmp_hdr = (icmphdr *)(get_context()->current_pkt.raw_content + ip_outer->ihl * 4);
	iphdr			*sent_iphdr = (iphdr *)((char *)icmp_hdr + 8);
	icmphdr			*sent_icmp = (icmphdr *)((char *)sent_iphdr + sent_iphdr->ihl * 4);

	printf("ICMP: type %d, code %d, size %ld, id 0x%04x, seq 0x%04x\n",
		sent_icmp->type,
		sent_icmp->code,
		sizeof(t_icmpping),
		ntohs(sent_icmp->un.echo.id),
		ntohs(sent_icmp->un.echo.sequence)
	);
}

void	print_error_output(void)
{
	t_ctx		*context = get_context();
	icmphdr		*icmp = context->current_pkt.icmp_header;

	printf("%ld bytes from %s: %s\n", 
		context->current_pkt.bytes_read - sizeof(struct iphdr),
		context->source_dest_ip,
		icmp_error_str(icmp->type, icmp->code)
	);
	if (context->options.verbose)
	{
		print_iphdr_hexdump();
		print_icmp();
	}
}

void	print_begin_output(t_icmpping *ping_packet)
{
	t_ctx	*context = get_context();

	if (context->options.verbose)
		printf("PING %s (%s): %ld data bytes, id 0x%x = %d\n", 
			context->hostname, context->source_dest_ip, sizeof(*ping_packet) - sizeof(icmphdr),
			context->id, context->id
		);
	else
		printf("PING %s (%s): %ld data bytes\n", 
			context->hostname, context->source_dest_ip, sizeof(*ping_packet) - sizeof(icmphdr));
}
