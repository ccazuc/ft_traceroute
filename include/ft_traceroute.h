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
# include <sys/time.h>
# include <pthread.h>
# include <sys/select.h>

# define MISSED_PING_TIMER 1

struct s_packet_node;

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
	struct icmphdr icmp_hdr;
} t_icmp_packet;

typedef struct s_icmp_tim_ex_packet
{
	struct ip ip_hdr;
	struct icmphdr icmp_hdr;
	struct ip ip_hdr_encaps;
	struct icmphdr icmp_hdr_encaps;
} t_icmp_time_ex_packet;

typedef struct s_udp_packet
{
	struct ip ip_hdr;
	struct udphdr udp_hdr;
} t_udp_packet;

typedef struct s_env
{
	t_params params;
	char *dst_param;		// host str from argv
	struct in_addr *dst_bin;	// host bin from getaddrinfo
	char *dst_name;			// host str from inet_ntop
	char *dst_subname;		// host str from gethostbyaddr
	struct sockaddr *dst_sockaddr;	// host sockaddr from getaddrinfo
	uint16_t dst_sockaddrlen;	// len of sockaddr from getaddrinfo
	int32_t send_socket;
	int32_t receive_socket;
	uint32_t count;
	uint16_t packet_len;
	uint8_t running;
	uint8_t send_per_ttl;
	t_icmp_packet *send_packet_icmp;
	t_udp_packet *send_packet_udp;
	t_icmp_time_ex_packet *receive_packet;
	struct s_packet_node *begin_list;
	uint32_t max_hops;
	uint32_t send_per_loop;
	uint32_t sent_hops;
	pthread_t thread;
	fd_set read_set;
	struct timeval select_timeout;
	size_t timeout;
	uint32_t number_printed;
	uint8_t default_ttl;
} t_env;

typedef struct s_packet_node
{
	struct s_packet_node *next;
	size_t *timers;
	size_t *received_timers;
	struct in_addr src_addr;
	uint8_t printed;
} t_packet_node;

void parse_args(t_env *env, int argc, char **argv);
void print_usage(int32_t code);
void print_invalid_param(uint8_t param, uint32_t index);
void print_unknown_dst(char *dst);
void resolve_host(t_env *env);
void create_socket(t_env *env);
void build_packet(t_env *env);
void build_icmp_checksum(t_env *env);
size_t get_time();
void send_packet(t_env *env);
void receive_packet(t_env *env);
void create_thread(t_env *env);
void print_node(t_env *env, t_packet_node *node, uint32_t index);
void print_version();

#endif
