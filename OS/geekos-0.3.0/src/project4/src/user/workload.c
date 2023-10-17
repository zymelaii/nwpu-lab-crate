/**********************************************************************

	    Project 2: Multiprogramming 


  By:
      Jeff Hollingsworth
  e-mail:
      hollings@cs.umd.edu

  File:        proc2.c
  Created on:  3/6/1996
  Contents:    User Processes to be run concurrently
	  1.    Init() : Initial process, creates other ones & terminates
	  2..4  Long() , Ping() , and Pong() are three examplar processes
	  that are to be run concurently.  Long() is a CPU intensive
          job, while Ping() and Pong() bounce back between one another.
	  5.    Wait() : busy wait for a key to be pressed

  Simon Hawkin <cema@cs.umd.edu> 03/16/1998
      - Added progress monitoring output in Long().
  Jeff Hollingsworth <hollings@cs.umd.edu> 2/19/02
      - Re-written for the new project
  David Hovemeyer <daveho@cs.umd.edu> 2/27/04
      - Update for GeekOS 0.2.0


  [2] An enclosed test ("proc2.c", or "encl2.c").

**********************************************************************/

#include <conio.h>
#include <process.h>
#include <sched.h>
#include <sema.h>
#include <string.h>

#if !defined (NULL)
#define NULL 0
#endif

int main(int argc , char ** argv)
{
  int policy = -1;
  int start;
  int elapsed;
  int quantum;
  int scr_sem;			/* sid of screen semaphore */
  int id1, id2, id3;    	/* ID of child process */

  if (argc == 3) {
      if (!strcmp(argv[1], "rr")) {
          policy = 0;
      } else if (!strcmp(argv[1], "mlf")) {
          policy = 1;
      } else {
	  Print("usage: %s [rr|mlf] <quantum>\n", argv[0]);
	  Exit(1);
      }
      quantum = atoi(argv[2]);
      Set_Scheduling_Policy(policy, quantum);
  } else {
      Print("usage: %s [rr|mlf] <quantum>\n", argv[0]);
      Exit(1);
  }

  start = Get_Time_Of_Day();
  scr_sem = Create_Semaphore ("screen" , 1)  ;

  P (scr_sem) ;
  Print ("************* Start Workload Generator *********\n");
  V (scr_sem) ;

  id1 = Spawn_Program ("/c/long.exe", "/c/long.exe"
  ) ;
  P (scr_sem) ;
  Print ("Process Long has been created with ID = %d\n",id1);
  V (scr_sem) ;


  id2 = Spawn_Program ("/c/ping.exe", "/c/ping.exe"
  ) ;

  P (scr_sem) ;
  Print ("Process Ping has been created with ID = %d\n",id2);
  V (scr_sem) ;

  id3 = Spawn_Program ("/c/pong.exe", "/c/pong.exe"
  ) ;
  P (scr_sem) ;
  Print ("Process Pong has been created with ID = %d\n",id3);
  V (scr_sem) ;

  Wait(id1);
  Wait(id2);
  Wait(id3);

  elapsed = Get_Time_Of_Day() - start;
  Print ("\nTests Completed at %d\n", elapsed) ;
  return 0;
}

