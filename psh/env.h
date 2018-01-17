#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>

extern char **environ;

using namespace std;

void internal_env()
{
  string buf;
  for(char **current = environ; *current; current++)
  {
    cout << *current << endl;
  }
}
