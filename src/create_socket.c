#include "ft_traceroute.h"

void create_socket(t_env *env)
{
	int osef;

	if ((env->send_socket = socket(env->params.af, SOCK_RAW, env->params.protocol)) == -1)
		ft_exit("Error, could not create socket", EXIT_FAILURE);
	osef = 1;
	if (setsockopt(env->send_socket, IPPROTO_IP, IP_HDRINCL, &osef, sizeof(osef)) == -1)
		ft_exit("Error, could not set socket options for send_socket", EXIT_FAILURE);
	if ((env->receive_socket = socket(env->params.af, SOCK_RAW, IPPROTO_ICMP)) == -1)
		ft_exit("Error, could not create receive_socket", EXIT_FAILURE);
	osef = 1;
	if (setsockopt(env->receive_socket, IPPROTO_IP, IP_HDRINCL, &osef, sizeof(osef)) == -1)
		ft_exit("Error, could not set socket options for receive_socket", EXIT_FAILURE);
}
