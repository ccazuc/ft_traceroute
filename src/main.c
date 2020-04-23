#include "ft_traceroute.h"

static t_env *create_env(void)
{
	t_env *env;

	if (!(env = malloc(sizeof(*env))))
		ft_exit("Error, could not malloc env", EXIT_FAILURE);
	env->params.count = 3;
	env->params.payload_size = 150;
	env->params.verbose = 0;
	env->params.protocol = IPPROTO_ICMP;
	env->params.af = AF_INET;
	env->packet_len = sizeof(env->send_packet_udp->ip_hdr) + sizeof(env->send_packet_udp->udp_hdr) + env->params.payload_size;
	env->dst_param = NULL;
	env->dst_bin = NULL;
	env->dst_name = NULL;
	env->dst_subname = NULL;
	env->dst_sockaddr = NULL;
	env->dst_sockaddrlen = 0;
	env->send_socket = 0;
	env->receive_socket = 0;
	env->count = 0;
	env->running = 1;
	env->send_per_ttl = 3;
	env->begin_list = NULL;
	return env;
}

int main(int argc, char **argv)
{
	if (argc == 1)
		print_usage(EXIT_FAILURE);
	if (getuid())
		ft_exit("Error, must be used as root or with sudo", EXIT_FAILURE);
	t_env *env = create_env();
	parse_args(env, argc, argv);
	if (!env->dst_param)
		print_usage(EXIT_FAILURE);
	resolve_host(env);
	printf("dst_param %s | dst_name %s | dst_subname %s\n", env->dst_param, env->dst_name, env->dst_subname);
	create_socket(env);
	build_packet(env);
	send_packet(env);
	while (env->running)
		receive_packet(env);
	free(env->dst_bin);
	free(env->dst_name);
	free(env->dst_sockaddr);
	free(env->dst_subname);
	free(env);
	return EXIT_SUCCESS;
}
