// Dos Scheduling

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream.h>

#include "server.h"
#include "schedule.h"
#include "status.h"

#define CLOCK_TICKS			1000/FPS
#define EFF_TICKS				CLOCK_TICKS/4

static volatile bool go;

void sighandler(int)
{
 go=true;
 timepos++;
}

void pops_initschedule()
{
 if (signal (SIGALRM,sighandler)==SIG_ERR) {
  perror ("Fehler beim setzen des Signal Handlers:");
  exit(1);
 }
 ualarm (CLOCK_TICKS,CLOCK_TICKS);
}

int pops_shedule()
{
 usleep(CLOCK_TICKS);

 if (go) {
	go=false;
	return 1;
 } else
	return 0;
}

void pops_exitschedule()
{
 ualarm (0,0);
 if (signal (SIGALRM,SIG_DFL)==SIG_ERR) {
  perror ("Fehler beim zurücksetzen des Signal Handlers:");
  exit(1);
 }
}

void exithandler(int num)
{
 pops_exitserver();
 exit(0);
}

void pops_run()
{
 if (signal (SIGINT,exithandler)==SIG_ERR) {
  perror ("Fehler beim setzen des ExitHandlers:");
  exit(1);
 }
 if (signal (SIGQUIT,exithandler)==SIG_ERR) {
  perror ("Fehler beim setzen des ExitHandlers:");
  exit(1);
 }
 if (signal (SIGTERM,exithandler)==SIG_ERR) {
  perror ("Fehler beim setzen des ExitHandlers:");
  exit(1);
 }

 while (1)
  {
   switch (pops_getstatus()) {
    case STATUS_WAIT:
     pops_calcnet();
     usleep (100);
     break;
    case STATUS_RUN:
     // Init Teil!
     pops_allocoutbuf();
     pops_initschedule();

     while (pops_getstatus()==STATUS_RUN) {
      if (pops_shedule()) {
       pops_send();
       pops_calcnet();
       pops_calculate();
       //printf ("f");
       //fflush(stdout);
      } else {
       pops_calcnet();
      }
     }

     // Exit Teil
     pops_exitschedule();
     pops_freeoutbuf();
     break;
    case STATUS_LOAD:
     pops_calcnet();
     pops_calcl();
     usleep (100);
     break;
    default:
     pops_calcnet();
   }
  }
}
