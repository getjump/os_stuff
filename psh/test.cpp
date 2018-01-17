#include <iostream>
#include <string>
#include <iomanip>

int main(int argc, char** argv)
{
  for(int i = 0; argv[i] != NULL; i++)
  {
    std::cout << argv[i] << "\n";

    for(auto &i: std::string(argv[i]))
    {
      std::cout << std::hex << int(i);
      std::cout << " ";
    }

    std::cout << std::endl;
  }

  std::string line;

  while(std::getline(std::cin, line))
  {
    std::cout << line << "\n";
  }
}
