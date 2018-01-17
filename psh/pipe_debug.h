#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

int internal_pipe_debug()
{
  std::string command;

  while(getline(cin, command))
  {
    cout << command << endl;
  }

  return 1;
}
