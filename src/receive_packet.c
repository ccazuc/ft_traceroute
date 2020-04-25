#include "ft_traceroute.h"

static uint8_t check_print(t_env *env, t_packet_node *node, uint32_t index)
{
	for (uint8_t k = 0; k < env->send_per_ttl; ++k)
	{
		if (!node->received_timers[k])
			return 0;
	}
	if (!node->printed)
	{
		print_node(env, node, index);
	}
	return 1;
}

static void check_node(t_env *env, t_packet_node *node, uint8_t offset, size_t time)
{
	node->received_timers[offset] = time;
	node->src_addr = env->receive_packet->ip_hdr.ip_src;
	for (uint8_t j = 0; j < env->send_per_ttl; ++j)
	{
		if (!node->received_timers[j])
			return;
	}
	t_packet_node *iter_node = env->begin_list;
	uint32_t j = 0;
	while (iter_node)
	{
		if (!check_print(env, iter_node, j))
			return;
		++j;
		iter_node = iter_node->next;
	}
}

static void handle_datas(t_env *env, size_t time)
{
	size_t count = env->receive_packet->icmp_hdr_encaps.un.echo.sequence ? env->receive_packet->icmp_hdr_encaps.un.echo.sequence - 1 : env->receive_packet->icmp_hdr.un.echo.sequence - 1;
	uint32_t node_index = count / env->send_per_ttl;
	uint8_t offset = count % env->send_per_ttl;
	t_packet_node *tmp_node = env->begin_list;
	uint32_t i = 0;

	if (offset >= env->send_per_ttl)
		return;
	while (i <= node_index && tmp_node)
	{
		if (i == node_index)
		{
			check_node(env, tmp_node, offset, time);
			return;
		}
		++i;
		tmp_node = tmp_node->next;
	}
}

static void receive_packet_icmp(t_env *env)
{
	int32_t received;

	FD_ZERO(&env->read_set);
	FD_SET(env->receive_socket, &env->read_set);
	select(env->receive_socket + 1, &env->read_set, NULL, NULL, &env->select_timeout);
	int set = FD_ISSET(env->receive_socket, &env->read_set);
	if (!set)
		return;
	if ((received = recvfrom(env->receive_socket, env->receive_packet, sizeof(*env->receive_packet) + env->params.payload_size, 0, NULL, NULL)) <= 0)
	{
		if (!received)
			return;
		if (received == -1)
			ft_exit("Error, could not receive packet", EXIT_FAILURE);
	}
	size_t time = get_time();
	if ((uint32_t)received < sizeof(env->receive_packet->ip_hdr) + sizeof(env->receive_packet->icmp_hdr))
		return;
	if (env->receive_packet->ip_hdr.ip_src.s_addr == env->send_packet_icmp->ip_hdr.ip_dst.s_addr && !env->receive_packet->icmp_hdr.type && !env->receive_packet->icmp_hdr.code && env->receive_packet->icmp_hdr.un.echo.id == getpid())
	{
		handle_datas(env, time);
	}
	//printf("Received %d from icmp, icmp_type %d | icmp_code %d | icmp_seq %d | icmp_id %d | icmp_encaps_type %d | icmp_encaps_id %d | icmp_encaps_seq %d\n", received, env->receive_packet->icmp_hdr.type, env->receive_packet->icmp_hdr.code, env->receive_packet->icmp_hdr.un.echo.sequence, env->receive_packet->icmp_hdr.un.echo.id, env->receive_packet->icmp_hdr_encaps.type, env->receive_packet->icmp_hdr_encaps.un.echo.id, env->receive_packet->icmp_hdr_encaps.un.echo.sequence);
	if ((uint32_t)received < sizeof(env->receive_packet) || env->receive_packet->icmp_hdr.type != ICMP_TIMXCEED || env->receive_packet->icmp_hdr.code || (env->receive_packet->icmp_hdr_encaps.un.echo.id != getpid() && env->receive_packet->icmp_hdr.un.echo.id != getpid()))
		return;
	handle_datas(env, time);
	send_packet(env);
}

static void receive_packet_udp(t_env *env)
{
	int32_t received;

	if ((received = recvfrom(env->receive_socket, env->receive_packet, sizeof(*env->receive_packet) + env->params.payload_size, 0, NULL, NULL)) <= 0)
	{
		if (!received)
			return;
		if (received == -1)
			ft_exit("Error, could not receive packet", EXIT_FAILURE);
	}
	printf("Received %d from udp, icmp_type %d | icmp_code %d | icmp_seq %d | icmp_id %d | icmp_encaps_type %d | icmp_encaps_id %d\n", received, env->receive_packet->icmp_hdr.type, env->receive_packet->icmp_hdr.code, env->receive_packet->icmp_hdr.un.echo.sequence, env->receive_packet->icmp_hdr.un.echo.id, env->receive_packet->icmp_hdr_encaps.type, env->receive_packet->icmp_hdr_encaps.un.echo.id);
	if (env->receive_packet->icmp_hdr.type != ICMP_TIMXCEED || env->receive_packet->icmp_hdr.code)
		return;
}

void receive_packet(t_env *env)
{
	if (env->params.protocol == IPPROTO_ICMP)
		receive_packet_icmp(env);
	else if (env->params.protocol == IPPROTO_UDP)
		receive_packet_udp(env);
}
