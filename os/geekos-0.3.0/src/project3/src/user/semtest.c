// A test program for semaphores

#include "libuser.h"
#include "libio.h"

int main( int argc, char ** argv)
{
  int semkey, result;

  Print("Create_Semaphore()...\n");
  semkey = Create_Semaphore("semtest", 3);
  Print("Create_Semaphore() returned %d\n", semkey);

  if (semkey < 0)
    return 0;

  Print("P()...\n");
  result = P(semkey);
  Print("P() returned %d\n", result);

  Print("P()...\n");
  result = P(semkey);
  Print("P() returned %d\n", result);

  Print("V()...\n");
  result = V(semkey);
  Print("V() returned %d\n", result);


  Print("Destroy_Semaphore()...\n");
  result = Destroy_Semaphore(semkey);
  Print("Destroy_Semaphore() returned %d\n", result);

  return 0;
}
