#include "ft_traceroute.h"

static void check_timeout_packets(t_env *env)
{
	size_t time = get_time();
	uint8_t k = 0;
	for (t_packet_node *iter = env->begin_list; iter; iter = iter->next)
	{
		for (uint8_t i = 0; i < env->send_per_ttl; ++i)
		{
			if (!iter->received_timers[i] && time - iter->timers[i] >= env->timeout)
				iter->received_timers[i] = MISSED_PING_TIMER;
		}
		++k;
	}
	uint32_t i = 0;
	for (t_packet_node *iter = env->begin_list; iter; iter = iter->next)
	{
		for (uint8_t j = 0; j < env->send_per_ttl; ++j)
		{
			if (!iter->received_timers[j])
				return;
		}
		if (!iter->printed)
		{
			print_node(env, iter, i);
			send_packet(env);
		}
		++i;
	}
}

static void *thread_run(void *ptr)
{
	t_env *env = ptr;
	while (1)
	{
		receive_packet(env);
		check_timeout_packets(env);
	}
	return NULL;
}

void create_thread(t_env *env)
{
	if (pthread_create(&env->thread, NULL, thread_run, env))
		ft_exit("Error, could not create thread", EXIT_FAILURE);
}
