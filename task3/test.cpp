// #include <iostream>
// #include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <sys/stat.h>
// #include <fcntl.h>

// using namespace std;

int main()
{
  int out = execl("/Users/getjump/Ucheba/OS/task3/main", "/Users/getjump/Ucheba/OS/task3/main");

  printf("%d\n", out);
}
