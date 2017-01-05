	
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
void* param=NULL;
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
{	//printf("\nSignal %d recu\n", s);
	sdl_push_event(param);
}

/*Demon fonction threads */
void *Demon(void *p)
{		printf("%p: bonjour ! ", pthread_self()); 
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
 pthread_create(&pid,NULL,Demon,/*(void*)p*/NULL);
 printf("pid:%d\n",getpid());
 struct sigaction action;
 action.sa_handler = hand;
 sigset_t pere;
 sigfillset(&pere);
 sigdelset(&pere,SIGALRM);
 sigprocmask(SIG_BLOCK, &pere,NULL);
 sigaction (SIGALRM, &action, NULL); 
 return 1; 
 
}

void timer_set (Uint32 delay, void *parametre)
{
  struct itimerval time;
  int sec=delay/1000;
  int usec=(delay-sec*1000)*1000;
  time.it_interval.tv_sec=sec;
  time.it_interval.tv_usec=usec;
  time.it_value.tv_sec=sec;
  time.it_value.tv_usec=usec;
  setitimer(ITIMER_REAL,&time,NULL);
  param=parametre;	
  //while (1);
  //TODOO
}
	
#endif
