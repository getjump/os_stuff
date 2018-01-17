#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define CHILD_PROCESS_ID 0
#define INVALID_PID(x) ((x) < CHILD_PROCESS_ID)

using namespace std;

int main()
{
  pid_t pid = fork();

  if(pid == CHILD_PROCESS_ID)
  {
    cout << "Child pid = " << getpid() << "; Parent pid = " << getppid() << endl;
  } else if(INVALID_PID(pid)) {
    cerr << "Failed to fork" << endl;
    exit(1);
  } else {
    cout << "Parent Pid = " << getpid() << "; child = " << pid << endl;
  }

  int status = -1;
  waitpid(pid, &status, WEXITED);

  if(pid == CHILD_PROCESS_ID)
  {
    cout << "Child";
  } else {
    cout << "Parent";
  }

  cout << " exited with return code " << status << endl;
  exit(EXIT_SUCCESS);
}
