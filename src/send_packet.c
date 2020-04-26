#include "ft_traceroute.h"

static void build_datas(t_env *env)
{
	if (env->params.protocol == IPPROTO_ICMP)
	{
		env->send_packet_icmp->icmp_hdr.un.echo.sequence = env->count + 1;
		env->send_packet_icmp->icmp_hdr.checksum = 0;
		if (env->params.payload_size >= sizeof(size_t))
		{
			size_t time = get_time();
			memcpy(((unsigned char*)env->send_packet_icmp) + sizeof(*env->send_packet_icmp), &time, sizeof(time));
		}
		build_icmp_checksum(env);
	}
	else if (env->params.protocol == IPPROTO_UDP)
	{
		//++env->send_packet_udp->udp_hdr.uh_sport;
		//++env->send_packet_udp->udp_hdr.uh_dport;
		if (env->params.payload_size >= sizeof(size_t))
		{
			size_t time = get_time();
			memcpy(((unsigned char*)env->send_packet_udp) + env->packet_len - env->params.payload_size, &time, sizeof(time));
		}
		env->send_packet_udp->udp_hdr.uh_sum = 0;
	}
}

static void add_node(t_env *env)
{
	uint32_t node_index = env->count / env->send_per_ttl;
	size_t time = get_time();
	if (!env->begin_list)
	{
		if (!(env->begin_list = malloc(sizeof(*env->begin_list))))
			ft_exit("Error, could not malloc begin_list", EXIT_FAILURE);
		if (!(env->begin_list->timers = malloc(sizeof(*env->begin_list->timers) * env->send_per_ttl)))
			ft_exit("Error, could not malloc begin_list->timers", EXIT_FAILURE);
		if (!(env->begin_list->received_timers = malloc(sizeof(*env->begin_list->received_timers) * env->send_per_ttl)))
			ft_exit("Error, could not malloc begin_list->received_timers", EXIT_FAILURE);
		env->begin_list->next = NULL;
		for (uint8_t i = 0; i < env->send_per_ttl; ++i)
			env->begin_list->timers[i] = time;
		memset(env->begin_list->received_timers, 0, sizeof(*env->begin_list->received_timers) * env->send_per_ttl);
		env->begin_list->printed = 0;
		env->begin_list->timers[env->count % env->send_per_ttl] = time;
		env->begin_list->src_addr.s_addr = 0;
	}
	else if (!node_index)
	{
		env->begin_list->timers[env->count % env->send_per_ttl] = get_time();
	}
	else
	{
		uint32_t i = 0;
		t_packet_node *tmp_node = env->begin_list;
		t_packet_node *last_node = NULL;
		while (i <= node_index && tmp_node)
		{
			if (i == node_index)
			{
				tmp_node->timers[env->count % env->send_per_ttl] = get_time();
				return;
			}
			last_node = tmp_node;
			tmp_node = tmp_node->next;
			++i;
		}
		t_packet_node *new_node = NULL;
		if (!(new_node = malloc(sizeof(*new_node))))
			ft_exit("Error, could not malloc new_node", EXIT_FAILURE);
		if (!(new_node->timers = malloc(sizeof(*new_node->timers) * env->send_per_ttl)))
			ft_exit("Error, could not malloc new_node->timers", EXIT_FAILURE);
		if (!(new_node->received_timers = malloc(sizeof(*new_node->received_timers) * env->send_per_ttl)))
			ft_exit("Error, could not malloc new_node->received_timers", EXIT_FAILURE);
		for (uint8_t i = 0; i < env->send_per_ttl; ++i)
			new_node->timers[i] = time;
		memset(new_node->received_timers, 0, sizeof(*new_node->received_timers) * env->send_per_ttl);
		new_node->next = NULL;
		new_node->printed = 0;
		last_node->next = new_node;
		new_node->src_addr.s_addr = 0;
		new_node->timers[env->count % env->send_per_ttl] = get_time();
	}
}

void send_packet(t_env *env)
{
	int32_t sent;
	uint32_t limit = env->sent_hops + env->send_per_loop;

	for (uint8_t i = env->sent_hops; i < limit && i < env->max_hops; ++i)
	{
		for (uint8_t i = 0; i < env->send_per_ttl; ++i)
		{
			build_datas(env);
			add_node(env);
			++env->count;
			if (env->params.protocol == IPPROTO_ICMP)
			{
				if ((sent = sendto(env->send_socket, env->send_packet_icmp, env->packet_len, 0, env->dst_sockaddr, env->dst_sockaddrlen)) == -1)
					ft_exit("sendto failed, exiting", EXIT_FAILURE);
				//printf("ICMP sent %d bytes\n", sent);
			}
			else if (env->params.protocol == IPPROTO_UDP)
			{
				if ((sent = sendto(env->send_socket, env->send_packet_udp, env->packet_len, 0, env->dst_sockaddr, env->dst_sockaddrlen)) == -1)
					ft_exit("sendto failed, exiting", EXIT_FAILURE);
				//printf("UDP sent %d bytes\n", sent);
			}
		}
		++env->send_packet_icmp->ip_hdr.ip_ttl;
		++env->sent_hops;
	}
}
