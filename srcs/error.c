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
    dprintf(STDERR, "ping: option value too small: %s\n", value);
    exit(1);   
}

void    err_value_too_big(char *value)
{
    dprintf(STDERR, "ping: option value too big: %s\n", value);
    exit(1);   
}

void    err_pattern(char *value)
{
    dprintf(STDERR, "ping: error in pattern near %c\n", *value);
    exit(1);   
}
