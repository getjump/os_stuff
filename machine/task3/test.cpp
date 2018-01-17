#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main()
{
  char* arg[] = {"/media/sf_machine/task3/test", NULL};
  execv(arg[0], arg);
  return 0;
}
