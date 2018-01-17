#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stack>
#include <deque>
#include <queue>

using namespace std;

std::string internal_cd(std::string folder, std::string cwd)
{
  deque<string> current_path;
  string buf;

  for(int i = 0; i < cwd.size(); ++i)
  {
    if(cwd[i] != '/')
    {
      buf += cwd[i];
    } else if(buf.size() > 0) {
      current_path.push_back(buf);
      buf.clear();
    }
  }

  if(buf[0] != 0x0)
  {
    current_path.push_back(buf);
  }

  deque<string> relative_path;
  buf.clear();

  if(folder[0] == '/')
  {
    return folder;
  }

  for(int i = 0; i < folder.size(); ++i)
  {
    if(folder[i] != '/')
    {
      buf += folder[i];
    } else if(buf.size() > 0) {
      relative_path.push_back(buf);
      buf.clear();
    }
  }

  relative_path.push_back(buf);

  while(!relative_path.empty())
  {
    if(relative_path.front()=="..")
    {
      if(!current_path.empty())
        current_path.pop_back();
    }
    else if(relative_path.front()==".") {}
    else
        current_path.push_back(relative_path.front());
    relative_path.pop_back();
  }

  string result = "/";

  while(!current_path.empty())
  {
    result += current_path.front();
    result += "/";

    current_path.pop_front();
  }

  // cout << result << endl;

  return result;
}
