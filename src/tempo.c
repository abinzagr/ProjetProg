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
 
 typedef struct echeance {
   long t_initial;
   struct itimerval timer;
   void *par;
   bool arme;
 } echeance;
 struct echeance echeancier[100];
 
 
 void traiter_alrm(int sig) {
   long t_actuel = get_time();
   int i = 0;
   while((t_actuel - (echeancier[i].t_initial + echeancier[i].timer.it_value.tv_sec * 1000000UL + echeancier[i].timer.it_value.tv_usec)) > 100 || echeancier[i].arme == false) {
     i++;
   }
   sdl_push_event(echeancier[i].par);
   echeancier[i].arme = false;
 }
 
 void *boucle_infinie(void *p)
 {
   sigset_t secondaire;
   sigfillset(&secondaire);
   sigdelset(&secondaire, SIGALRM);
   struct sigaction sa;
   sa.sa_handler = traiter_alrm;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;
   sigaction(SIGALRM, &sa, NULL);
   while(1) {
     sigsuspend(&secondaire);
   }
 }
 
 // timer_init returns 1 if timers are fully implemented, 0 otherwise
 int timer_init (void)
 {
   for(int i = 0; i < 100; i++) {
     echeancier[i].t_initial = 0;
     echeancier[i].timer.it_interval.tv_sec=0;
     echeancier[i].timer.it_interval.tv_usec=0;
     echeancier[i].timer.it_value.tv_sec=0;
     echeancier[i].timer.it_value.tv_usec=0;
     echeancier[i].par = NULL;
     echeancier[i].arme = false;
   }
   pthread_t tid[1];
   sigset_t principal;
   sigemptyset(&principal);
   sigaddset(&principal, SIGALRM);
   sigprocmask(SIG_SETMASK, &principal, NULL);
   pthread_create(&tid[0], NULL, boucle_infinie, NULL);
   return 1;
 }
 
 void timer_set (Uint32 delay, void *param)
 {
   int i = 0;
   while(echeancier[i].arme != false)
     i++;
   echeancier[i].par = param;
 
   long sec = delay / 1000;
   long usec = (delay - sec*1000) * 1000;
   echeancier[i].timer.it_interval.tv_sec = 0;
   echeancier[i].timer.it_interval.tv_usec = 0;
   echeancier[i].timer.it_value.tv_sec = sec;
   echeancier[i].timer.it_value.tv_usec = usec;
   echeancier[i].arme = true;
   echeancier[i].t_initial = get_time();
   setitimer(ITIMER_REAL,&(echeancier[i].timer),NULL);
 }
 
 #endif
