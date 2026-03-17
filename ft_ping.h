/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 11:54:48 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/13 17:14:00 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

#define _GNU_SOURCE  // enables everything

#define STDERR 2

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

typedef struct s_lst
{
	void			*content;
	struct s_lst	*next;
} t_lst;

typedef struct s_icmpping
{
    struct icmphdr      header;
    char                payload[56];
} t_icmpping;

typedef struct s_ctx
{
    int                 socket;
    char                *hostname;
    struct addrinfo     *dest;
    char                source_dest_ip[INET_ADDRSTRLEN];
    t_lst               *times;
    struct timeval      start;
    int                 ping_successes;
    int                 seq;
} t_ctx;

void    parse_args(char **args, char **host);
void	get_readable_ip_str(struct sockaddr *ai_addr, char *ipaddr_str);
t_ctx	*get_context(void);

// LIST FUNCTIONS
t_lst	*ft_lstnew(void *content);
void	ft_lstadd_back(t_lst **lst, t_lst *new);
void	ft_lstclear(t_lst **lst, void (*del)(void *));
float   calculate_avg(t_lst **lst);

#endif