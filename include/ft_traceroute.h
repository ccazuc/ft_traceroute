#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include "../libft/includes/libft.h"
# include <stdlib.h>
# include <stdint.h>
# include <stdio.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

typedef struct s_params
{
	uint32_t count;
	uint32_t payload_size;
	uint8_t verbose;
	int32_t protocol;
	int32_t af;
} t_params;

typedef struct s_icmp_packet
{
	struct ip ip_hdr;
	struct icmp icmp_hdr;
} t_icmp_packet;

typedef struct s_udp_packet
{
	struct ip ip_hdr;
	struct udphdr udp_hdr;
} t_udp_packet;

typedef struct s_env
{
	t_params params;
	char *dst_param;		// host str from argv
	char *dst_bin;			// host bin from getaddrinfo
	char *dst_name;			// host str from inet_ntop
	char *dst_subname;		// host str from gethostbyaddr
	struct sockaddr *dst_sockaddr;	// host sockaddr from getaddrinfo
	uint16_t dst_sockaddrlen;	// len of sockaddr from getaddrinfo
	int32_t socket;
} t_env;

void parse_args(t_env *env, int argc, char **argv);
void print_usage(int32_t code);
void print_invalid_param(uint8_t param, uint32_t index);
void print_unknown_dst(char *dst);
void resolve_host(t_env *env);

#endif
