#include <conio.h>
#include <process.h>
#include <sched.h>
#include <sema.h>
#include <string.h>

#if !defined (NULL)
#define NULL 0
#endif

int main( int argc , char ** argv )
{
  int scr_sem;			/* sid of screen semaphore */
  int id1, id2, id3;    	/* ID of child process */
  

  scr_sem    = Create_Semaphore ( "screen" , 1 )  ;
  

  P ( scr_sem ) ;
  Print ("Semtest1 begins\n");
  V ( scr_sem ) ;


  id3 = Spawn_Program ( "/c/p3.exe", "/c/p3.exe" ) ;
  P ( scr_sem ) ;
  Print ("p3 created\n");
  V ( scr_sem ) ;
  id1 = Spawn_Program ( "/c/p1.exe", "/c/p2.exe" ) ;
  id2 = Spawn_Program ( "/c/p2.exe", "/c/p1.exe" ) ;
  

  Wait(id1);
  Wait(id2);
  Wait(id3);

  Destroy_Semaphore(scr_sem);
  return 0;
}

