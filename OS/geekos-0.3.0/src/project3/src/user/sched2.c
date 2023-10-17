#include <conio.h>
#include <process.h>
#include <sched.h>
#include <sema.h>
#include <string.h>

int main( int argc , char ** argv )
{

  int i,j ;     	/* loop index */

  for (i=0; i < 20; i++) {
    for(j=0;j<20000;j++);
    Print("2");
  }

  return 0;
}
