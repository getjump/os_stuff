#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define CHILD_PROCESS_ID 0
#define INVALID_PID(x) ((x) < CHILD_PROCESS_ID)

using namespace std;

pid_t pid1, pid2;

void child1_trap(int sig);
void child2_trap(int sig);

void child1_trap(int sig)
{
  if(sig >= 100)
  {
    exit(0);
    return;
  }

  cout << sig << endl;

  signal(sig+1, &child2_trap);
  kill(pid2, sig+1);
}

void child2_trap(int sig)
{
  if(sig >= 100)
  {
    exit(0);
    return;
  }

  cout << sig << endl;

  signal(sig+1, &child1_trap);
  kill(pid1, sig+1);
}

int main()
{
  pid1 = fork();

  if(pid1 == CHILD_PROCESS_ID && pid2 != CHILD_PROCESS_ID)
  {
    //cout << "KEK1";
    signal(1, &child1_trap);
  } else if(pid1 > CHILD_PROCESS_ID && pid2 > CHILD_PROCESS_ID)
  {
    pid2 = fork();
    cout << "KEK";
    kill(pid1, 1);
  }

  if(pid2 == CHILD_PROCESS_ID && pid1 != CHILD_PROCESS_ID)
  {
    while(true){}
  }

  exit(EXIT_SUCCESS);
}
