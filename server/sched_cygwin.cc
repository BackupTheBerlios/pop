// Dos Scheduling

#include <stdio.h>
#include <unistd.h>

#include "schedule.h"
#include "status.h"

#define CLOCK_TICKS				1000000/FPS

void pops_run()
{
 while (1)
  {
   switch (pops_getstatus()) {
    case STATUS_WAIT:
     pops_calcnet();
     usleep (50000);
     break;
    case STATUS_RUN:
     // Init Teil!
     pops_allocoutbuf();

     while (pops_getstatus()==STATUS_RUN) {
       pops_send();
       pops_calcnet();
       pops_calculate();
       pops_calcnet();
       usleep(CLOCK_TICKS); // Bad Thing, could be various speed
     }

     // Exit Teil
     pops_freeoutbuf();
     break;
    case STATUS_LOAD:
     pops_calcnet();
     pops_calcl();
     usleep (50000);
     break;
    default:
     pops_calcnet();
   }
  }

 pops_exitserver();
}
