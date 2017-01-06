#define _XOPEN_SOURCE 600

#include <SDL.h>
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

#ifdef PADAWAN

/*timer values */
typedef struct echeancier {
	long t_initial;
	struct itimerval timer;
	void* parametre;
	bool arme;
	} echeancier;

echeancier tab_echeancier[100];

/*traitant alarm*/
void hand(int sig)
{
 long t_actuel = get_time();
 int i = 0;
 while((t_actuel - (tab_echeancier[i].t_initial + tab_echeancier[i].timer.it_value.tv_sec * 1000000UL + tab_echeancier[i].timer.it_value.tv_usec)) > 100 || tab_echeancier[i].arme == false) {
    i++;
  }
  sdl_push_event(tab_echeancier[i].parametre);
  tab_echeancier[i].arme = false;
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


// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  for(int i = 0; i < 100; i++){
    //initialiser echeance[i]
    tab_echeancier[i].t_initial = 0;
    tab_echeancier[i].timer.it_interval.tv_sec = 0;
    tab_echeancier[i].timer.it_interval.tv_usec = 0;
    tab_echeancier[i].timer.it_value.tv_sec = 0;
    tab_echeancier[i].timer.it_value.tv_usec = 0;
    tab_echeancier[i].parametre = NULL;
    tab_echeancier[i].arme = false;
  }	
  pthread_t pid;
  sigset_t pere; 
  sigfillset(&pere);
  sigdelset(&pere,SIGALRM);
  sigprocmask(SIG_BLOCK, &pere,NULL);
  pthread_create(&pid,NULL,Demon,NULL);
  return 1; 
 
}

void timer_set (Uint32 delay, void *param)
{ long sec = delay / 1000;
  long usec = (delay - sec*1000) * 1000;	 
  int i = 0;
  while(tab_echeancier[i].arme != false)
  i++;
  tab_echeancier[i].parametre= param;
  tab_echeancier[i].timer.it_interval.tv_sec=0;
  tab_echeancier[i].timer.it_interval.tv_usec=0;
  tab_echeancier[i].timer.it_value.tv_sec=sec;
  tab_echeancier[i].timer.it_value.tv_usec=usec;
  tab_echeancier[i].parametre = param;
  tab_echeancier[i].arme = true;
  tab_echeancier[i].t_initial = get_time();
  setitimer(ITIMER_REAL, &(tab_echeancier[i].timer), NULL); 
}
	
#endif
