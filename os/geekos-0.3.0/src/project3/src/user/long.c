
#include "libuser.h"
#include "process.h"

int main(int argc, char **argv)
{
  int i, j ;     	/* loop index */
  int scr_sem;		/* id of screen semaphore */
  int now, start, elapsed; 		

  start = Get_Time_Of_Day();
  scr_sem = Create_Semaphore ("screen" , 1) ;   /* register for screen use */

  for (i=0; i < 200; i++) {
      for (j=0 ; j < 10000 ; j++) ;
      now = Get_Time_Of_Day();
  }
  elapsed = Get_Time_Of_Day() - start;
  P (scr_sem) ;
  Print("Process Long is done at time: %d\n", elapsed) ;
  V(scr_sem);


  return 0;
}

