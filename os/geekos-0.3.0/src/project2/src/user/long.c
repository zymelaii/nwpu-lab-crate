
#include "libuser.h"
#include "process.h"

int main(int argc, char **argv)
{
  int i, j ;     	/* loop index */
  Print("Start Long\n");

  for (i=0; i < 400; i++) {
      for (j=0 ; j < 10000 ; j++) ;
      Get_PID();
  }
  Print("End Long\n");

  return 0;
}

