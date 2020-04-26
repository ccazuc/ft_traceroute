#include "ft_traceroute.h"

static void parse_first_ttl(t_env *env, char *arg)
{
	int32_t value = ft_atoi(arg);
	if (value <= 0 || value >= 30)
		ft_exit("first hop out of range", EXIT_FAILURE);
	env->default_ttl = value;
}

static void parse_nqueries(t_env *env, char *arg)
{
	int32_t value = ft_atoi(arg);
	if (value <= 0 || value >= 10)
		ft_exit("no more than 10 probes per hop", EXIT_FAILURE);
	env->send_per_ttl = value;
}

static void parse_max_hops(t_env *env, char *arg)
{
	int32_t value = ft_atoi(arg);
	if (value <= 0)
		ft_exit("first hop out of range", EXIT_FAILURE);
	if (value > 255)
		ft_exit("max hops cannot be more than 255", EXIT_FAILURE);
	env->max_hops = value;
}

static void parse_payload_size(t_env *env, char *arg, int32_t index)
{
	int32_t value = ft_atoi(arg);
	env->params.parsed_payload_size = 1;
	if (value < 0)
	{
		printf("Bad option `%s' (argc %d)\n", arg, index);
		exit(EXIT_FAILURE);
	}
	if (value <= 28)
		env->params.payload_size = 0;
	else
		env->params.payload_size = value - 28;
}

static void parse_wait(t_env *env, char *arg)
{
	int32_t value = ft_atoi(arg);
	env->params.parsed_payload_size = 1;
	if (value < 1)
	{
		printf("bad wait specification `%s' used\n", arg);
		exit(EXIT_FAILURE);
	}
	env->timeout = value * 1000000;
	printf("%ld\n", env->timeout);
}

static void parse_str_arg(t_env *env, char *arg, int32_t *index)
{
	if (!ft_strncmp(arg, "first=", 6))
	{
		parse_first_ttl(env, arg + 6);
	}
	else if (!ft_strncmp(arg, "queries=", 8))
	{
		parse_nqueries(env, arg + 8);
	}
	else if (!ft_strncmp(arg, "max-hops=", 9))
	{
		parse_max_hops(env, arg + 9);
	}
	else if (!ft_strncmp(arg, "max=", 4))
	{
		parse_wait(env, arg + 4);
	}
	else if (!ft_strcmp(arg, "version"))
	{
		print_version();
	}
	else if (!ft_strcmp(arg, "help"))
	{
		print_usage(EXIT_SUCCESS);
	}
	else
	{
		printf("Bad option `--%s' (argc %d)\n", arg, *index);
		exit(EXIT_FAILURE);
	}
}

static void parse_arg(t_env *env, int argc, char **argv, int32_t *index)
{
	char *arg = argv[*index];
	if (!arg[0])
		return;
	if (arg[0] == '-')
	{
		if (arg[1] == '-')
		{
			parse_str_arg(env, arg + 2, index);
			return;
		}
		uint32_t i = 0;
		while (arg[++i])
		{
			if (arg[i] == 'V')
				print_version();
			else if (arg[i] == 'f')
			{
				if (*index + 1 >= argc)
				{
					printf("Option `-f' (argc %d) requires an argument: `-f first_ttl'\n", *index);
					exit(EXIT_FAILURE);
				}
				parse_first_ttl(env, argv[*index + 1]);
				++*index;
			}
			else if (arg[i] == 'q')
			{
				if (*index + 1 >= argc)
				{
					printf("Option `-q' (argc %d) requires an argument: `-q nqueries'\n", *index);
					exit(EXIT_FAILURE);
				}
				parse_nqueries(env, argv[*index + 1]);
				++*index;
			}
			else if (arg[i] == 'm')
			{
				if (*index + 1 >= argc)
				{
					printf("Option `-m' (argc %d) requires an argument: `-m max_ttl'\n", *index);
					exit(EXIT_FAILURE);
				}
				parse_max_hops(env, argv[*index + 1]);
				++*index;
			}
			else if (arg[i] == 'w')
			{
				if (*index + 1 >= argc)
				{
					printf("Option `-w' (argc %d) requires an argument: `-w MAX'\n", *index);
					exit(EXIT_FAILURE);
				}
				parse_wait(env, argv[*index + 1]);
				++*index;
			}
			else
				print_invalid_param(arg[i], *index);
		}
	}
	else if (!env->dst_param)
		env->dst_param = arg;
	else if (!env->params.parsed_payload_size)
		parse_payload_size(env, arg, *index);
	else
	{
		printf("Extra arg `%s' (position 3, argc %d)\n", arg, *index);
		exit(EXIT_FAILURE);
	}
}

void parse_args(t_env *env, int argc, char **argv)
{
	int32_t i = 0;
	while (++i < argc)
		parse_arg(env, argc, argv, &i);
}
