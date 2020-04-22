#include "ft_traceroute.h"

static void build_datas(t_env *env)
{
	if (env->params.protocol == IPPROTO_ICMP)
	{
		env->send_packet_icmp->icmp_hdr.un.echo.sequence = env->count;
		env->send_packet_icmp->icmp_hdr.checksum = 0;
		if (env->params.payload_size >= sizeof(size_t))
		{
			size_t time = get_time();
			memcpy(((unsigned char*)env->send_packet_icmp) + env->packet_len - env->params.payload_size, &time, sizeof(time));
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

void send_packet(t_env *env)
{
	int32_t sent;

	++env->count;
	build_datas(env);
	if (env->params.protocol == IPPROTO_ICMP)
	{
		if ((sent = sendto(env->send_socket, env->send_packet_icmp, env->packet_len, 0, env->dst_sockaddr, env->dst_sockaddrlen)) == -1)
			ft_exit("sendto failed, exiting", EXIT_FAILURE);
		printf("ICMP sent %d bytes\n", sent);
	}
	else if (env->params.protocol == IPPROTO_UDP)
	{
		if ((sent = sendto(env->send_socket, env->send_packet_udp, env->packet_len, 0, env->dst_sockaddr, env->dst_sockaddrlen)) == -1)
			ft_exit("sendto failed, exiting", EXIT_FAILURE);
		printf("UDP sent %d bytes\n", sent);
	}
}
