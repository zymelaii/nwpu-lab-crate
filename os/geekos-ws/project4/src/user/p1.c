#include <conio.h>
#include <process.h>
#include <sched.h>
#include <sema.h>
#include <string.h>

int main( int argc , char ** argv )
{

  int i ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int prod_sem,cons_sem;	

  scr_sem = Create_Semaphore ( "screen" , 1 ) ;   /* register for screen use */
  prod_sem = Create_Semaphore ( "prod_sem" , 0 ) ;   
  cons_sem = Create_Semaphore ( "cons_sem" , 1 ) ;  

  for (i=0; i < 5; i++) {
    P(cons_sem);
    Print ("Produced %d\n",i) ;
    V(prod_sem);
  }
  
  Destroy_Semaphore(scr_sem);
  Destroy_Semaphore(prod_sem);
  Destroy_Semaphore(cons_sem);

  return 0;
}
