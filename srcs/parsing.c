/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:31:34 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/09 16:40:29 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void invalid_option(char c)
{
    dprintf(STDERR, "ping: invalid option -- '%c'\n", c);
    dprintf(STDERR, "Try 'ping -?' for more information.\n");
    exit(64);
}

void print_help(void)
{
    printf("%s\n", "Usage: ping [OPTION...] HOST ...");
    printf("%s\n", "Send ICMP ECHO_REQUEST packets to network hosts.");
    printf("%s\n", "");
    printf("%s\n", " Valid options:");
    printf("%s\n", "");
    printf("%s\n", "  -?        give this help list");
    printf("%s\n", "  -V        print program version");
    printf("%s\n", "");
    printf("%s\n", "Report bugs to <do_not_report_bugs_please@42.fr>.");
    exit(EXIT_SUCCESS);
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
    exit(EXIT_SUCCESS);
}

void parse_option(char *opt)
{
    opt += 1;
	for (int i = 0; opt[i]; i++)  // loop needed foor bonuses
	{
		if (opt[0] == '-' && strlen(opt) == 1)
			return ;
		if (opt[0] == '?')
			print_help();
		else if (opt[0] == 'V')
			print_version();
		else
			invalid_option(opt[0]);
	}
}

void parse_args(char **args, char **host)
{
    bool host_set = false;
    
	for (int i = 0; args[i]; i++)
	{
		if (args[i][0] == '-' && strlen(args[i]) > 1)
			parse_option(args[i]);
		else if (!host_set)
		{
			*host = args[i];
            host_set = true;
		}
	}
}