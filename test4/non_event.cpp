#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

#define CHILD_PROCESS_ID 0
#define INVALID_PID(x) ((x) < CHILD_PROCESS_ID)

using namespace std;

ifstream input;
ofstream output;

void odd()
{
  // int in_fd, out_fd;

  // in_fd = open ("test.txt", O_RDONLY);
  // out_fd = open("test.txt", O_WRONLY | O_APPEND | O_CREAT, 0777);

  input.open("test.txt");
  output.open("test.txt", fstream::app);

  while(true)
  {
    input.clear( );
    input.seekg( 0, std::ios::beg );

    unsigned int local;
    while(!input.eof())
    {
      input >> local;
    }

    if(local >= 100)
    {
      exit(EXIT_SUCCESS);
      return;
    }

    if(local % 2 == 0)
    {
      output << local + 1 << endl;
    }

    usleep(300);
  }
}

void even()
{
  // int in_fd, out_fd;

  // in_fd = open ("test.txt", O_RDONLY);
  // out_fd = open("test.txt", O_WRONLY | O_APPEND | O_CREAT, 0777);

  input.open("test.txt");
  output.open("test.txt", fstream::app);

  while(true)
  {
    input.clear( );
    input.seekg( 0, std::ios::beg );

    unsigned int local;
    while(!input.eof())
    {
      input >> local;
    }

    if(local >= 100)
    {
      exit(EXIT_SUCCESS);
      return;
    }

    if(local % 2 != 0)
    {
      output << local+1 << endl;
    }

    usleep(300);
  }
}

int main()
{
  int in_fd, out_fd;

  in_fd = open ("test.txt", O_RDONLY);
  out_fd = open("test.txt", O_WRONLY | O_CREAT, 0777);

  write(out_fd, "1\n", 2);

  pid_t pid1 = fork();
  pid_t pid2 = fork();

  if(pid1 == CHILD_PROCESS_ID && pid2 != CHILD_PROCESS_ID)
  {
    odd();
  }

  if(pid2 == CHILD_PROCESS_ID && pid1 != CHILD_PROCESS_ID)
  {
    even();
  }

  exit(EXIT_SUCCESS);
}
