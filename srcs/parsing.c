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

void    err_invalid_option(char c)
{
    dprintf(STDERR, "ping: invalid option -- '%c'\n", c);
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_invalid_value(char *arg, char *err)
{
    dprintf(STDERR, "ping: invalid value (`%s' near `%s')\n", arg, err);
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64); 
}

void    err_unrecognized_option(char *str)
{
    dprintf(STDERR, "ping: unrecognized option '%s'\n", str);
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_missing_host_operand(void)
{
    dprintf(STDERR, "ping: missing host operand\n");
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_requires_argument(char *option)
{
    dprintf(STDERR, "ping: option '%s' requires an argument\n", option);
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_forbid_argument(char *option)
{
    dprintf(STDERR, "ping: option '%s' doesn't allow an argument\n", option);
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_missing_host(void)
{
    dprintf(STDERR, "ping: missing host operand\n");
    dprintf(STDERR, "Try 'ping -?' or 'ping --help' for more information.\n");
    exit(64);
}

void    err_value_too_small(char *value)
{
    dprintf(STDERR, "ping: value too small: %s\n", value);
    exit(1);   
}

bool    isnum(char c)
{
    return (c >= '0' && c <= '9');
}

void    invalid_value(char *str)
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


bool    isallnum(char *str)
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

int    register_ttl(char *value)
{
    t_ctx   *context = get_context();
    int     ttl_value = 0;

    ttl_value = convert_check_value(value, UINT8_MAX);
    context->options.ttl = true;
    context->options.ttl_value = ttl_value;
    return (0);
}

int    register_timeout(char *next_arg)
{
    (void) next_arg;
    // t_ctx   *context = get_context();
    // char    *value_s = opt + 9;
    // long    timeout = 0;
// 
    // if (opt[0] == '-')
        // timeout = convert_check_value(value_s, INT32_MAX);
    // else
    // {
        // if (!next_arg)
        // {
            // dprintf(STDERR, "ping: option requires an argument -- %s\n", opt);
            // exit(EXIT_FAILURE);
        // }
        // timeout = convert_check_value(next_arg, INT32_MAX);
    // }
    // context->options.timeout = true;
    // context->options.timeout_value = (int)timeout;
    // printf("timeout value = %d\n", context->options.timeout_value);
    return (0);
}

void    print_arg_lst(t_lst **args)
{
    for (t_lst *curr = *args; curr; curr = curr->next)
    {
        printf("arg = %s\n", (char*)curr->content);
    }
}

void    create_arg_lst(char **args, t_lst **args_lst)
{
    t_lst   *new_node = NULL;

    for (int i = 0; args[i]; i++)
    {
        // new_arg = malloc(sizeof(t_arg));
        new_node = malloc(sizeof(t_lst));
        if (!new_node)
            exit_error(); // add clear list
        new_node->content = (void *)strdup(args[i]);
        if (!new_node->content)
            exit_error();
        new_node->next = NULL;
        ft_lstadd_back(args_lst, new_node);
    }
}

bool    is_valid_opt_char(char c)
{
    char valid[] = "ipvVwW?";

    for (int i = 0; valid[i]; i++)
    {
        if (c == valid[i])
            return (true);
    }
    return (false);
}

bool    is_valid_opt_str(char *str)
{
    char *valid[] = {"interval", "interval=",
        "pattern", "pattern=", 
        "wait", "wait=", 
        "ttl", "ttl=", 
        "timeout", "timeout=",
        "verbose", "version", "help", NULL};         // verbose= : "doesnt allow an argument"

    str += 2;
    for (int i = 0; valid[i]; i++)
    {
        if (strncmp(valid[i], str, strlen(str)) == 0)
            return (true);
    }
    return (false);
}

int     register_interval(char *value)
{
    t_ctx   *context = get_context();
    char    *error = NULL;

    float  val = strtof(value, &error);
    if (*error)
        err_invalid_value(value, error);
    if (val > 99999.0f || val < 0.1f)
        err_value_too_small(value);
    printf("value = %f\n", val);
    float decimal = modff(val, &val);
    printf("int = %f decimal = %f\n", val, decimal);
    context->options.interval = true;
    // context->options.interval_value = val;
    printf("registered interval, value = %f\n", val);
    return (0);
}

int     register_pattern(char *value)
{
    (void) value;
    // t_ctx   *context = get_context
    return (0);
}

int     register_wait(char *value)
{
    printf("registering wait with value = %s\n", value);
    return (0);
}

int     register_verbose(void)
{
    t_ctx   *context = get_context();

    context->options.verbose = true;
    return (0);
}

bool    option_needs_arg(char *opt)
{
    if (strncmp(opt, "verbose", strlen(opt)) == 0 || strncmp(opt, "help", strlen(opt)) == 0
        || strncmp(opt, "version", strlen(opt)) == 0)
        return (false);
    return (true);
}

bool    option_needs_arg_char(char opt)
{
    if (opt == 'V' || opt == 'v' || opt == '?')
        return (false);
    return (true);
}

int     register_option(char *opt, char *value)
{
    opt += 2;

    if (strncmp(opt, "help", 4) == 0)
        return (print_help());
    if (strncmp(opt, "interval", 8) == 0)
        return (register_interval(value));
    if (strncmp(opt, "pattern", 7) == 0)
        return (register_pattern(value));
    if (strncmp(opt, "wait", 4) == 0)
        return (register_wait(value));
    if (strncmp(opt, "ttl", 3) == 0)
        return (register_ttl(value));
    if (strncmp(opt, "timeout", 7) == 0)
        return (register_timeout(value));
    if (strncmp(opt, "verbose", 7) == 0)
        return (register_verbose());
    return (0);
}

int     register_option_char(char opt, char *value)
{
    printf("opt c = %c\n", opt);
    switch (opt)
    {
        case '?':
            return (print_help());
            break;

        case 'i':
            return (register_interval(value));
            break;
            
        case 'p':
            return (register_pattern(value));
            break;

        case 'v':
            return (register_verbose());

        case 'w':
            return (register_timeout(value));
            break;

        case 'W':
            return (register_wait(value));
            break;
    
        default:
            break;
    }
    return (0);
}

int     d_dashed_option(char *opt, t_lst *curr, t_lst **args_lst)
{
    char *eq = strstr(opt, "=");
    if (eq)
    {        
        char *value = eq + 1;
        *eq = 0;
        if (option_needs_arg(opt))
            err_forbid_argument(opt);
        if (!is_valid_opt_str(opt))
            err_unrecognized_option(opt);
        register_option(opt, value);
        return (true);
    }
    else
    {
        if (option_needs_arg(opt))
        {
            if (!curr->next)
                err_requires_argument(opt);
            register_option(opt, curr->next->content);
            ft_lstdelone(args_lst, curr->next, free);
            curr->next = NULL;
        }
        return (true);
    }
}

int     s_dashed_option(char *opt, t_lst *curr, t_lst **args_lst)
{
    opt += 1;

    printf("sdash\n");
    for (int i = 0; opt[i]; i++)
    {
        if (!is_valid_opt_char(opt[i]))
            err_invalid_option(opt[i]);
        if (!option_needs_arg_char(opt[i]))
            register_option_char(opt[i], NULL);
        else if (opt[i + 1])
        {
            register_option_char(opt[i], opt + (i + 1));
            break ;
        }
        else
        {
            if (!curr->next)
                err_requires_argument(opt);
            register_option_char(opt[i], curr->next->content);
            ft_lstdelone(args_lst, curr->next, free);
            curr->next = NULL;
        }
    }
    return (0);
}

bool    check_option(t_lst **args_lst, t_lst *curr)
{
    char   *opt = (char *)curr->content;

    if (opt[0] == '-' && opt[1] == '-')
        d_dashed_option(opt, curr, args_lst);
    else if (opt[0] == '-')
        s_dashed_option(opt, curr, args_lst);
    return (true);
}

bool    is_an_opt(char *arg)
{
    return (arg[0] == '-');
}

void    check_all_opt(t_lst **args_lst)
{
    for (t_lst *curr = *args_lst; curr; curr = curr->next)
    {
        if (is_an_opt((char *)curr->content))
        {
            check_option(args_lst, curr);
            ft_lstdelone(args_lst, curr, free);
            curr = *args_lst;
            if (!curr)
                break;
        }
    }
}

void    parse_args(char **args)
{
    t_ctx   *context = get_context();
    t_lst   *args_lst = NULL;

    create_arg_lst(args, &args_lst);
    print_arg_lst(&args_lst);
    check_all_opt(&args_lst);
    if (args_lst)
    {
        context->hostname = strdup((char *)args_lst->content);
        if (!context->hostname)
            exit_error();
        ft_lstclear(&args_lst, free);
    }
    else
    {
        ft_lstclear(&args_lst, free);
        err_missing_host_operand();
    }
}
