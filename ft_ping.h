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
#include <math.h>
#include <time.h>

typedef struct sockaddr_in  sockaddr_in;
typedef struct addrinfo     addrinfo;
typedef struct iphdr        iphdr;
typedef struct icmphdr      icmphdr;
typedef struct timeval      timeval;

typedef struct s_lst
{
	void			    *content;
	struct s_lst	    *next;
} t_lst;

typedef struct s_icmpping
{
    icmphdr             header;
    char                payload[56];
} t_icmpping;

typedef struct s_opt
{
    bool                interval;
    bool                pattern;
    bool                timeout;
    bool                ttl;
    bool                verbose;
    bool                wait;
    char                *pattern_value;
    timeval             interval_value;
    int                 ttl_value;
    int                 timeout_value;
    int                 wait_value;

} t_opt;

typedef struct t_pkt
{
    int                 bytes_read;
    iphdr               *ip_header;
    icmphdr             *icmp_header;
	unsigned char 		raw_content[1024];
	socklen_t			sender_len;;
	sockaddr_in 	    sender;

} t_pkt;

typedef struct s_ctx
{
    t_opt               options;
    int                 socket;
    int                 id;
    char                *hostname;
    addrinfo            *dest;
    char                source_dest_ip[INET_ADDRSTRLEN];
    t_lst               *times;
    t_pkt               current_pkt;
    int                 ping_successes;
    int                 seq;
} t_ctx;

void    parse_args(char **args);
void	get_readable_ip_str(struct sockaddr *ai_addr, char *ipaddr_str);
t_ctx	*get_context(void);
void	exit_error(void);

// LIST FUNCTIONS
t_lst	*ft_lstnew(void *content);
void	ft_lstadd_back(t_lst **lst, t_lst *new);
void	ft_lstclear(t_lst **lst, void (*del)(void *));
void	ft_lstdelone(t_lst **lst, t_lst *to_del, void (*del)(void *));
float   calculate_avg(t_lst **lst);
float	get_min(t_lst **lst);
float	get_max(t_lst **lst);
float	get_standard_deviation(t_lst **lst);
void	store_time(t_ctx *context, float time);

// UTILS
void	exit_error(void);

// PACKET HANDLING
void	prep_ping_packet(t_icmpping *ping_packet);
void	send_packet(t_icmpping *ping);
int		receive_packet(void);
float	get_time_elapsed(struct timeval *starting_time);

// PRINT
int     print_help(void);
void    print_version(void);
void	print_success_output(float *time_elapsed);
void	print_error_output(void);
void	print_begin_output(t_icmpping *ping_packet);

// NETWORK
void	resolve_host(char *host, struct addrinfo **dest);
void	setup_socket(int *sock, struct addrinfo *res_list);

// ERRORS
void    err_invalid_option(char c);
void    err_invalid_value(char *arg, char *err);
void    err_unrecognized_option(char *str);
void    err_missing_host_operand(void);
void    err_requires_argument(char *option);
void    err_forbid_argument(char *option);
void    err_missing_host(void);
void    err_value_too_small(char *value);
void    err_pattern(char *value);
void    err_value_too_big(char *value);

#endif