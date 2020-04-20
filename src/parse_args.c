#include "ft_traceroute.h"

static void parse_arg(t_env *env, char *arg, int32_t *index)
{
	(void)index;
	if (!arg[0])
		return;
	if (arg[0] == '-')
	{
		uint32_t i = 0;
		while (arg[++i])
		{
			if (arg[i] == 'h')
				print_usage(EXIT_SUCCESS);
			else
				print_invalid_param(arg[i], *index);
		}
	}
	else if (!env->dst_param)
		env->dst_param = arg;
}

void parse_args(t_env *env, int argc, char **argv)
{
	int32_t i = 0;
	while (++i < argc)
		parse_arg(env, argv[i], &i);
}
