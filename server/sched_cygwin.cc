// Dos Scheduling

#include <stdio.h>
#include <unistd.h>

#include "schedule.h"
#include "status.h"

#define CLOCK_TICKS				UCLOCKS_PER_SEC/FPS

void pops_run()
{
 while (1)
  {
   switch (pops_getstatus()) {
    case STATUS_WAIT:
     pops_calcnet();
     usleep (500);
     break;
    case STATUS_RUN:
     // Init Teil!
     pops_allocoutbuf();

     while (pops_getstatus()==STATUS_RUN) {
       pops_send();
       pops_calcnet();
       pops_calculate();
       pops_calcnet();
       usleep(2000); // Bad Thing, could be various speed
     }

     // Exit Teil
     pops_freeoutbuf();
     break;
    case STATUS_LOAD:
     pops_calcnet();
     pops_calcl();
     usleep (500);
     break;
    default:
     pops_calcnet();
   }
  }

 pops_exitserver();
}
