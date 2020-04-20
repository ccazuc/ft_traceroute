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
