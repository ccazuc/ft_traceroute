#include "ft_traceroute.h"

static void alloc_packet(t_env *env)
{
	if (env->params.protocol == IPPROTO_ICMP)
	{
		if (!(env->send_packet_icmp = malloc(sizeof(env->send_packet_icmp->ip_hdr) + ICMP_MINLEN + env->params.payload_size)))
			ft_exit("Error, could not malloc send_packet", EXIT_FAILURE);
	}
	else if (env->params.protocol == IPPROTO_UDP)
	{
		if (!(env->send_packet_udp = malloc(sizeof(env->send_packet_udp->ip_hdr) + sizeof(env->send_packet_udp->udp_hdr) + env->params.payload_size)))
			ft_exit("Error, coult not malloc send_packet", EXIT_FAILURE);
	}
	else
	{
		ft_exit("Error, protocol not supported", EXIT_FAILURE);
	}
	if (!(env->receive_packet = malloc(sizeof(env->receive_packet->ip_hdr) + ICMP_MINLEN + env->params.payload_size)))
		ft_exit("Error, coult not malloc receive_packet", EXIT_FAILURE);
}

void build_icmp_checksum(t_env *env)
{
	int32_t size = 0;
	uint32_t sum;
	int32_t i;
	uint16_t *packet;

	size = (ICMP_MINLEN + env->params.payload_size) / 2;
	i = -1;
	sum = 0;
	packet = (uint16_t*)&env->send_packet_icmp->icmp_hdr;
	while (++i < size)
		sum += packet[i];
	if ((uint32_t)(i * 2) < ICMP_MINLEN + env->params.payload_size)
		sum += packet[i] & 0x00FF;
	while (sum > 0xFFFF)
		sum = (sum & 0xFFFF) + (sum >> 16);
	env->send_packet_icmp->icmp_hdr.checksum = ~sum;
}

static void build_ip_header(t_env *env)
{
	struct ip *ip_hdr;
	if (env->params.protocol == IPPROTO_ICMP)
		ip_hdr = &env->send_packet_icmp->ip_hdr;
	else if (env->params.protocol == IPPROTO_UDP)
		ip_hdr = &env->send_packet_udp->ip_hdr;
	ip_hdr->ip_v = 4;
	ip_hdr->ip_hl = 5;
	ip_hdr->ip_tos = IPTOS_LOWDELAY;
	ip_hdr->ip_id = 0;
	ip_hdr->ip_off = ntohs(IP_DF);
	ip_hdr->ip_ttl = 4;
	ip_hdr->ip_p = env->params.protocol;
	ip_hdr->ip_sum = 0;
	ip_hdr->ip_src.s_addr = 0;
	ip_hdr->ip_dst = *env->dst_bin;
}

static void build_icmp_packet(t_env *env)
{
	env->send_packet_icmp->icmp_hdr.type = ICMP_ECHO;
	env->send_packet_icmp->icmp_hdr.code = 0;
	env->send_packet_icmp->icmp_hdr.un.echo.id = getpid();
	env->send_packet_icmp->icmp_hdr.un.echo.sequence = env->count;
	build_icmp_checksum(env);
	env->packet_len = sizeof(env->send_packet_icmp->ip_hdr) + ICMP_MINLEN + env->params.payload_size;
}

static void build_udp_packet(t_env *env)
{
	env->send_packet_udp->udp_hdr.uh_sport = htons(55417);
	env->send_packet_udp->udp_hdr.uh_dport = htons(33433);
	env->send_packet_udp->udp_hdr.uh_ulen = htons((sizeof(env->send_packet_udp->udp_hdr) + env->params.payload_size));
	printf("len %d\n", env->send_packet_udp->udp_hdr.uh_ulen);
	//env->send_packet_udp->udp_hdr.uh_sum = 0x1234;
	env->packet_len = sizeof(env->send_packet_udp->ip_hdr) + sizeof(env->send_packet_udp->udp_hdr) + env->params.payload_size;
}

void build_packet(t_env *env)
{
	alloc_packet(env);
	build_ip_header(env);
	if (env->params.protocol == IPPROTO_ICMP)
		build_icmp_packet(env);
	else if (env->params.protocol == IPPROTO_UDP)
		build_udp_packet(env);
}
