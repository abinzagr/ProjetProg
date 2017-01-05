	
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
static pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
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

/*timer values */
typedef struct echeancier {
	struct itimerval timer;
	void* param;
	bool expire;
	} echeancier;

echeancier tab_echeancier[200];

/*handler demon*/
void hand(int s)
{
  printf("\nSignal %d recu\n", s);
  //printf ("sdl_push_event(%p) appelée au temps %ld\n", param, get_time ());
  
  /*unsigned long t = get_time();
  bool all_event_finished = false;
  struct itimerval t_event = tab_echeancier[0].timer;
  int i_event = 0;
  while(all_event_finished == false){
    all_event_finished = true;
    //on recupere l'indice de l'evenement le plus recent et qui n'a pas encore ete effectue
    for(int i = 0; i < 200; i++){
      if(tab_echeancier[i].timer.it_value.tv_sec < t_event.it_value.tv_sec && tab_echeancier[i].expire == false){
	t_event = tab_echeancier[i].timer;
	i_event = i;
      }
    }
    //on traite l'evenement le plus recent
    sdl_push_event(tab_echeancier[i].param);
    tab_echeancier[i_event].expire = true;
    //si tous les evenements n'ont pas ete effectues, on permet a la boucle while
    //de continuer
    for(int i = 0; i < 200; i++){
      if(tab_echeancier[i].expire == false)
	all_event_finished = false;
    }
  }
  */
  sdl_push_event(param); //  a mettre avant
  //recuperer le premier indice ou expire =false
  //sdl_push_event
  //
}

/*Demon fonction threads */
void *Demon(void *p)
{
  printf("%p: bonjour ! ", pthread_self()); 
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
{
  pthread_t pid;
  pthread_create(&pid,NULL,Demon,/*(void*)p*/NULL);
  printf("pid:%d\n",getpid());
  struct sigaction action;
  action.sa_handler = hand;
  sigset_t pere; 
  sigfillset(&pere);
  sigdelset(&pere,SIGALRM);
  sigprocmask(SIG_BLOCK, &pere,NULL);
  sigaction(SIGALRM, &action, NULL);
  for(int i = 0; i < 200; i++){
    //initialiser echeance[i]
    tab_echeancier[i].timer.it_interval.tv_sec = 0;
    tab_echeancier[i].timer.it_interval.tv_usec = 0;
    tab_echeancier[i].timer.it_value.tv_sec = 0;
    tab_echeancier[i].timer.it_value.tv_usec = 0;
    //init expire ? param ?
    tab_echeancier[i].expire = true;
  }
 
  return 1; 
 
}

void timer_set (Uint32 delay, void *parametre)
{
  struct itimerval time;
  int sec=delay/1000;
  int usec=(delay-sec*1000)*1000; /* for()!!  */
  /*time.it_interval.tv_sec=sec; //echeancier[i].timer.time.it_interval.tv_sec=sec;
  time.it_interval.tv_usec=usec;
  time.it_value.tv_sec=sec;
  time.it_value.tv_usec=usec;
  setitimer(ITIMER_REAL,&time,NULL);
  param=parametre;*/
  int i = 0;
  tab_echeancier[i].timer.it_interval.tv_sec=0;
  tab_echeancier[i].timer.it_interval.tv_usec=0;
  tab_echeancier[i].timer.it_value.tv_sec=sec;
  tab_echeancier[i].timer.it_value.tv_usec=usec;
  tab_echeancier[i].param = parametre;
  tab_echeancier[i].expire = false;
  setitimer(ITIMER_REAL, &(tab_echeancier[i].timer), NULL); 
  	
  //while (1);
  //TODOO
}
	
#endif
