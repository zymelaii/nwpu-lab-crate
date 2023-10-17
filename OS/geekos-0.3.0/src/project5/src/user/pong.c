#include <conio.h>
#include <process.h>
#include <sched.h>
#include <sema.h>
#include <string.h>

int main(int argc , char ** argv)
{
  int i,j ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int time; 		/* current and start time */
  int ping,pong;	/* id of semaphores to sync processes b & c */

  time = Get_Time_Of_Day();
  scr_sem = Create_Semaphore ("screen" , 1) ;   /* register for screen use */
  ping = Create_Semaphore ("ping" , 1) ;    
  pong = Create_Semaphore ("pong" , 0) ;   

  for (i=0; i < 5; i++) {
       P(ping);
       for (j=0; j < 35; j++);
       V(pong);
  }

  time = Get_Time_Of_Day() - time;
  P (scr_sem) ;
  Print ("Process Pong is done at time: %d\n", time) ;
  V(scr_sem);





  return (0);
}

