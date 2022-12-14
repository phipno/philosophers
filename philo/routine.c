/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnolte <pnolte@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/05 14:33:07 by pnolte            #+#    #+#             */
/*   Updated: 2022/10/20 19:12:18 by pnolte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	sleepy_philo(t_philo *phi)
{
	print_manager('s', phi);
	if (phi->timer_eat + phi->timer_sleep >= phi->timer_die)
	{
		my_sleep(phi->timer_die - phi->timer_eat, phi);
		if (death_manager(phi->timer_die + 1, phi) == true)
			return ;
	}
	my_sleep(phi->timer_sleep, phi);
	if (death_manager(phi->timer_sleep, phi) == true)
		return ;
	if (phi->timer_eat * 2 >= phi->timer_die)
	{
		print_manager('t', phi);
		my_sleep(phi->timer_die - phi->curr_run_mili, phi);
		if (death_manager(phi->timer_die + 1, phi) == true)
			return ;
	}
}

static void	forks_and_food(t_philo *phi)
{
	lock_them_mutexes(phi);
	print_manager('e', phi);
	if (phi->timer_eat > phi->timer_die)
		my_sleep(phi->timer_die, phi);
	else
		my_sleep(phi->timer_eat, phi);
	if (death_manager(phi->timer_eat, phi) == true)
		return ;
	phi->count_phi_eat++;
	unlock_them_mutexes(phi);
	sleepy_philo(phi);
}

static void	*dinner_for_one(t_philo *phi)
{
	print_manager('f', phi);
	my_sleep(phi->timer_die, phi);
	printf("%d %d died\n", phi->timer_die, phi->id);
	return (NULL);
}

static void	brainpower(t_philo *phi)
{
	phi->multi = 1;
	print_manager('t', phi);
	if (phi->id == phi->nbr_phi)
		phi->multi = 2;
	my_sleep(phi->timer_eat * phi->multi, phi);
	death_manager(phi->timer_eat * phi->multi, phi);
}

void	*routine(void *arg)
{
	t_philo	*phi;

	phi = (t_philo *)arg;
	phi->curr_run_mili = current_runtime(phi);
	if (phi->nbr_phi == 1)
		return (dinner_for_one(phi));
	if (phi->id % 2 != 0)
		brainpower(phi);
	pthread_mutex_lock(&phi->slk->lock);
	while (phi->slk->death == false)
	{
		pthread_mutex_unlock(&phi->slk->lock);
		forks_and_food(phi);
		print_manager('t', phi);
		if (phi->nbr_phi % 2 != 0)
			my_sleep(phi->timer_eat, phi);
		if (phi->slk->nbr_times_phi_eat != 0
			&& phi->count_phi_eat == phi->slk->nbr_times_phi_eat)
			return (NULL);
		pthread_mutex_lock(&phi->slk->lock);
	}
	pthread_mutex_unlock(&phi->slk->lock);
	return (NULL);
}
