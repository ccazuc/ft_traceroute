#include "ft_traceroute.h"

static void receive_packet_icmp(t_env *env)
{
	int32_t received;
	char ip[16];

	if ((received = recvfrom(env->receive_socket, env->receive_packet, sizeof(env->receive_packet->ip_hdr) + ICMP_MINLEN + env->params.payload_size, 0, NULL, NULL)) <= 0)
	{
		if (!received)
			return;
		if (received == -1)
			ft_exit("Error, could not receive packet", EXIT_FAILURE);
	}
	printf("Received %d from icmp, icmp_type %d | icmp_code %d | icmp_seq %d | icmp_id %d | icmp_encaps_type %d\n", received, env->receive_packet->icmp_hdr.type, env->receive_packet->icmp_hdr.code, env->receive_packet->icmp_hdr.un.echo.sequence, env->receive_packet->icmp_hdr.un.echo.id, env->receive_packet->icmp_hdr_encaps.type);
}

static void receive_packet_udp(t_env *env)
{
	int32_t received;
	char ip[16];

	printf("calling\n");
	if ((received = recvfrom(env->receive_socket, env->receive_packet, sizeof(env->receive_packet->ip_hdr) + ICMP_MINLEN + env->params.payload_size, 0, NULL, NULL)) <= 0)
	{
		printf("receive %d\n", received);
		if (!received)
			return;
		if (received == -1)
			ft_exit("Error, could not receive packet", EXIT_FAILURE);
	}
	printf("Received %d from udp, icmp_type %d | icmp_code %d | icmp_seq %d | icmp_id %d | icmp_encaps_type %d\n", received, env->receive_packet->icmp_hdr.type, env->receive_packet->icmp_hdr.code, env->receive_packet->icmp_hdr.un.echo.sequence, env->receive_packet->icmp_hdr.un.echo.id, env->receive_packet->icmp_hdr_encaps.type);
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
