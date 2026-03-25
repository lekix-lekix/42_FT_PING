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

// ping: missing host operand
// Try 'ping --help' or 'ping --usage' for more information.

void missing_host_operand(void)
{
    dprintf(STDERR, "ping: missing host operand\n");
    dprintf(STDERR, "Try 'ping -?' for more information.\n");
    exit(64);
}


bool isnum(char c)
{
    return (c >= '0' && c <= '9');
}

void invalid_value(char *str)
{
    char *err;

    for (int i = 0; str[i]; i++)
    {
        if (str[i] && !isnum(str[i]))
        {
            err = str + i;
            break;
        }
    }
    dprintf(STDERR, "ping: invalid value (`%s' near `%s')\n", str, err);
    exit(EXIT_FAILURE);
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

long    convert_check_value(char *nb, long limit)
{
    long    value;
    
    if (isallnum(nb) == false)
        invalid_value(nb);
    value = atol(nb);
    if (value > limit)
    {
        dprintf(STDERR, "ping: option value too big: %s\n", nb);
        exit(EXIT_FAILURE);
    }
    else if (value <= 0)
    {
        dprintf(STDERR, "ping: option value too small: %s\n", nb);
        exit(EXIT_FAILURE);
    }
    return (value);
}

void    register_ttl(char *opt)
{
    t_ctx   *context = get_context();
    char    *value_s = opt + 5;
    int     ttl_value = 0;

    ttl_value = convert_check_value(value_s, UINT8_MAX);
    context->options.ttl = true;
    context->options.ttl_value = ttl_value;
}

void register_timeout(char *opt, char *next_arg)
{
    t_ctx   *context = get_context();
    char    *value_s = opt + 9;
    long    timeout = 0;

    if (opt[0] == '-')
        timeout = convert_check_value(value_s, INT32_MAX);
    else
    {
        if (!next_arg)
        {
            dprintf(STDERR, "ping: option requires an argument -- %s\n", opt);
            exit(EXIT_FAILURE);
        }
        timeout = convert_check_value(next_arg, INT32_MAX);
    }
    context->options.timeout = true;
    context->options.timeout_value = (int)timeout;
    printf("timeout value = %d\n", context->options.timeout_value);
}

void parse_option(char *opt, char *next_arg)
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
        else if (opt[0] == 'w' || strncmp(opt, "-timeout=", 9) == 0)
        {
            register_timeout(opt, next_arg);
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

// void parse_args(char **args, char **host)
// {
//     bool host_set = false;
    
// 	for (int i = 0; args[i]; i++)
// 	{
// 		if (args[i][0] == '-' && strlen(args[i]) > 1)
//         {
// 			parse_option(args[i], args[i + 1]);
//             if (!(args[i][1] == '-')) // if option is type -opt and not --opt
//                 i++;
//         }
// 		else if (!host_set)
// 		{
// 			*host = args[i];
//             host_set = true;
// 		}
// 	}
//     if (!host_set)
//         missing_host_operand();
// }

void print_arg_lst(t_lst **args)
{
    for (t_lst *curr = *args; curr; curr = curr->next)
    {
        t_arg *arg = (t_arg *)curr->content;
        printf("arg = %s\n", arg->arg);
    }
}

void create_arg_lst(char **args, t_lst **args_lst)
{
    t_lst   *new_node = NULL;
    t_arg   *new_arg = NULL;

    for (int i = 0; args[i]; i++)
    {
        new_arg = malloc(sizeof(t_arg));
        new_node = malloc(sizeof(t_lst));
        if (!new_arg || !new_node)
            exit_error(); // add clear list
        new_arg->arg = args[i];
        new_arg->value = NULL;
        new_node->content = (void *)new_arg;
        new_node->next = NULL;
        ft_lstadd_back(args_lst, new_node);
    }
}

bool is_valid_opt_char(char c)
{
    char valid[] = "ipvVwW";

    for (int i = 0; valid[i]; i++)
    {
        if (c == valid[i])
            return (true);
    }
    return (false);
}

bool is_valid_opt_str(char *str)
{
    char *valid[] = {"interval", "pattern", "wait", "ttl"};
}

bool check_option(char *arg)
{
    bool double_dash = false;
    bool single_dash = false;

    if (arg[0] == '-' && arg[1] == '-')
        double_dash = true;
    if (arg[0])
}

bool is_an_opt(char *arg)
{
    return (arg[0] == '-');
}

void check_all_opt(t_lst **args_lst)
{
    // bool need_value;

    for (t_lst *curr; curr; curr = curr->next)
    {
        t_arg *curr = (t_arg *)curr->arg;
        if (is_an_opt(curr->arg))
        {
            check_option(curr);

            ft_lstdelone(args_lst, curr, free);
        }
    }
}

void parse_args(char **args, char **host)
{
    t_lst  *args_lst = NULL;

    (void) host;
    create_arg_lst(args, &args_lst);
    print_arg_lst(&args_lst);
    check_all_args(&args_lst);
}
