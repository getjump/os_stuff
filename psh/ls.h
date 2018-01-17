#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

int internal_ls(std::string folder)
{
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(folder.c_str())) == NULL) {
      cout << "Error(" << errno << ") opening " << folder << endl;
      return errno;
  }

  string buf = "";

  while ((dirp = readdir(dp)) != NULL) {
      cout << dirp->d_name << endl;
      // buf += "\n";
  }

  // cout << buf;

  closedir(dp);
  return 0;
}
