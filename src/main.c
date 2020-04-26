#include "ft_traceroute.h"

static t_env *create_env(void)
{
	t_env *env;

	if (!(env = malloc(sizeof(*env))))
		ft_exit("Error, could not malloc env", EXIT_FAILURE);
	env->params.count = 3;
	env->params.payload_size = 32;
	env->params.verbose = 0;
	env->params.protocol = IPPROTO_ICMP;
	env->params.af = AF_INET;
	env->params.parsed_payload_size = 0;
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
	env->max_hops = 30;
	env->send_per_loop = 3;
	env->sent_hops = 0;
	env->select_timeout.tv_sec = 0;
	env->select_timeout.tv_usec = 0;
	env->timeout = 1000000;
	env->number_printed = 0;
	env->default_ttl = 1;
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
	create_socket(env);
	build_packet(env);
	create_thread(env);
	printf("traceroute to %s (%s), %d hops max, %ld bytes per packets\n", env->dst_subname ? env->dst_subname : env->dst_name, env->dst_name, env->max_hops, sizeof(env->send_packet_icmp->ip_hdr) + sizeof(env->send_packet_icmp->icmp_hdr) + env->params.payload_size);
	send_packet(env);
	while(env->running)
		;
	free(env->dst_bin);
	free(env->dst_name);
	free(env->dst_sockaddr);
	free(env->dst_subname);
	free(env);
	return EXIT_SUCCESS;
}
