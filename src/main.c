#include "ft_traceroute.h"

static t_env *create_env(void)
{
	t_env *env;

	if (!(env = malloc(sizeof(*env))))
		ft_exit("Error, could not malloc env", EXIT_FAILURE);
	env->params.count = 3;
	env->params.payload_size = 56;
	env->params.verbose = 0;
	env->params.protocol = IPPROTO_UDP;
	env->params.af = AF_INET;
	env->dst_param = NULL;
	env->dst_bin = NULL;
	env->dst_name = NULL;
	env->dst_subname = NULL;
	env->dst_sockaddr = NULL;
	env->dst_sockaddrlen = 0;
	env->socket = 0;
	return env;
}

int main(int argc, char **argv)
{

	if (argc == 1)
		print_usage(EXIT_FAILURE);
	t_env *env = create_env();
	parse_args(env, argc, argv);
	if (!env->dst_param)
		print_usage(EXIT_FAILURE);
	resolve_host(env);
	printf("dst_param %s | dst_name %s | dst_subname %s\n", env->dst_param, env->dst_name, env->dst_subname);
	free(env->dst_bin);
	free(env->dst_name);
	free(env->dst_sockaddr);
	free(env->dst_subname);
	free(env);
	return EXIT_SUCCESS;
}
