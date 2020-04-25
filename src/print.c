#include "ft_traceroute.h"

void print_usage(int32_t code)
{
	printf("Usage:\n  traceroute destination\n");
	exit(code);
}

void print_invalid_param(uint8_t param, uint32_t index)
{
	printf("Bad option \'-%c\' (argc %d)\n", param, index);
	exit(EXIT_FAILURE);
}

void print_unknown_dst(char *dst)
{
	printf("ping: %s: Nom ou service inconnu\n", dst);
	exit(EXIT_FAILURE);
}

void print_node(t_env *env, t_packet_node *node, uint32_t index)
{
	node->printed = 1;
	++env->number_printed;
	if (node->src_addr.s_addr)
	{
		char ip[17];
		if (!inet_ntop(env->params.af, &node->src_addr, ip, INET_ADDRSTRLEN))
			ft_exit("Error, unknown host", EXIT_FAILURE);
		ip[16] = '\0';
		char *subname = NULL;
		//struct hostent *hent;
		/*if ((hent = gethostbyaddr((char*)&(node->src_addr.s_addr), sizeof(node->src_addr.s_addr), env->params.af)))
		{
			if (!(subname = malloc(strlen(hent->h_name) + 1)))
				ft_exit("Error, could not malloc subname", EXIT_FAILURE);
			strcpy(subname, hent->h_name);
		}*/
		printf("%2d  %s (%s)", index + 1, subname ? subname : ip, ip);
	}
	else
	{
		printf("%2d ", index + 1);
	}
	for (uint8_t i = 0; i < env->send_per_ttl; ++i)
	{
		if (node->received_timers[i] == MISSED_PING_TIMER)
			printf(" *");
		else
			printf("  %.3f ms", (node->received_timers[i] - node->timers[i]) / 1000.f);
	}
	printf("\n");
	if (node->src_addr.s_addr == env->send_packet_icmp->ip_hdr.ip_dst.s_addr || env->number_printed == env->max_hops)
		exit(EXIT_SUCCESS);
}
