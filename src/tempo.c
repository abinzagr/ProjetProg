#define _GNU_SOURCE
#include <SDL/SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

#include "timer.h"

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;
  
  return tv.tv_sec * 1000000UL + tv.tv_usec;
}


/*handler demon*/
void hand(int s)
{	printf("yolo");
	
}

/*Demon fonction threads */
void *Demon(void *p)
{	
		struct sigaction action;
		action.sa_handler = hand;
		sigset_t my_set;
		sigfillset(&my_set);
		sigdelset(&my_set,SIGALRM);
		sigprocmask(SIG_BLOCK, &my_set,NULL);
		sigaction (SIGALRM, &action, NULL);
		while(1){
			sigsuspend(&(my_set));
		}
}



#ifdef PADAWAN

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{pthread_t pid;
 void *val;	
 pthread_create(&pid,NULL,Demon,NULL);
 pthread_join(pid,&val); 
  return 0; 
  // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
struct itimerval time;
  time.it_interval.tv_sec=0;
  time.it_interval.tv_usec=500000;
  time.it_value.tv_sec=0;
  time.it_value.tv_usec=500000;
  setitimer(ITIMER_REAL,&time,NULL);
}
	
#endif
/*
int main(int argc, char *argv[])
{	
		timer_init();
		return EXIT_SUCCESS;
	}
*/
