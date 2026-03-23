/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:31:34 by kipouliq          #+#    #+#             */
/*   Updated: 2026/03/13 17:13:54 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void invalid_option(char c)
{
    dprintf(STDERR, "ping: invalid option -- '%c'\n", c);
    dprintf(STDERR, "Try 'ping -?' for more information.\n");
    exit(64);
}

void unrecognized_option(char *str)
{
    dprintf(STDERR, "ping: unrecognized option -- '%s'\n", str);
    dprintf(STDERR, "Try 'ping -?' for more information.\n");
    exit(64);
}

void invalid_value(char *str)
{
    dprintf(STDERR, "ping: invalid value (`%s' near `%s')\n", str, str);
    exit(EXIT_FAILURE);
}

bool isnum(char c)
{
    return (c >= '0' && c <= '9');
}

bool isallnum(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (isnum(str[i]) == false)
            return false;
    }
    return true;
}

void register_ttl(char *str)
{
    t_ctx   *context = get_context();
    char    *value_s = str + 5;
    int     ttl_value = 0;

    if (isallnum(value_s) == false)
        invalid_value(value_s);
    ttl_value = atoi(value_s);
    if (ttl_value > 255)
    {
        dprintf(STDERR, "ping: option value too big: %d\n", ttl_value);
        exit(EXIT_FAILURE);
    }
    context->options.ttl = true;
    context->options.ttl_value = ttl_value;
}

void parse_option(char *opt)
{
    t_ctx   *context = get_context();
    size_t  len = strlen(opt);

    opt += 1;
	for (size_t i = 0; i < len; i++)
	{
		if (opt[0] == '-' && strlen(opt) == 1)
			return ;
		if (opt[0] == '?')
			print_help();
		else if (opt[0] == 'V')
			print_version();
        else if (opt[0] == 'v' || strncmp(opt, "-verbose", strlen(opt)) == 0)
        {
            context->options.verbose = true;
            return ;
        }
        else if (strncmp(opt, "-ttl=", 5) == 0)
        {
            register_ttl(opt);
            return ;
        }
        else if (opt[0] == 'f' || strncmp(opt, "-flood", strlen(opt)) == 0)
        {
            context->options.flood = true;
            return ;
        }
		else
        {
            if (opt[0] == '-')
                unrecognized_option(opt);
            else
			    invalid_option(opt[0]);
            return ;
        }
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