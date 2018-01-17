// #include "Shell.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <map>
#include <fcntl.h>
#include <ncurses.h>
#include <termios.h>
#include <regex>
#include <signal.h>

#include "ls.h"
#include "cd.h"
#include "env.h"
#include "pipe_debug.h"

#define READ  0
#define WRITE 1

using namespace std;

extern char **environ;
extern char* read_line();

vector<int> running_processes;

void parse(string command);
map<string, string> local_variables;

inline bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

using namespace std;

struct pipe_resolution
{
  int start;
  int end;
};

std::string cwd;
std::string user;
std::string home;

vector<string> paths;

int background_pipe[2] = { -1, -1 };

void update_vars()
{
  cwd = getenv("PWD");

  string env_path = getenv("PATH");

  user = getenv("USER");
  home = getenv("HOME");

  stringstream ss(env_path); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, ':')) {
    paths.push_back(tok);
  }
}

int execve(const string &path, const vector<string>& argv)
{
  vector<char*> av;

  for(auto &i: argv)
  {
    av.push_back((char*)i.c_str());
  }

  av.push_back(0);

  return ::execve(path.c_str(), av.data(), environ);
}

std::vector<std::string> traverse_path(std::string name)
{
  std::vector<std::string> buf;

  for(auto &p: paths)
  {
    if(file_exists( p + "/" + name ))
    {
      buf.push_back(p + "/" + name);
    }
  }

  if(file_exists( name ))
  {
    buf.push_back(name);
  }

  return buf;
}

bool micro_process(int argc, vector<string> argv)
{
  bool processed = false;

  if(argv[0] == "ls")
  {
    if(argc == 1)
    {
      internal_ls(cwd);
      processed = true;
    } else if(argc == 2) {
      internal_ls(argv[1]);
      processed = true;
    }
  }

  if(argv[0] == "run")
  {
    if(argc == 2)
    {
      // cerr << argv[1];

      if(!file_exists(argv[1]))
      {
        cerr << "File not found" << endl;
      }

      ifstream infile(argv[1]);

      string line;

      while (std::getline(infile, line))
      {
        parse(line);
      }

      processed = true;
    }
  }

  if(argv[0] == "vars")
  {
    for(auto &i: local_variables)
    {
      cout << i.first << "=" << i.second << endl;
    }

    processed = true;
  }

  if(argv[0] == "pwd")
  {
    cout << cwd << endl;
    processed = true;
  }

  if(argv[0] == "echo")
  {
    for(auto &a: argv)
    {
      if(a == argv.front())
        continue;
      cout << a << " ";
    }

    cout << endl;
    processed = true;
  }

  if(argv[0] == "env")
  {
    internal_env();
    processed = true;
  }

  if(argv[0] == "pd")
  {
    internal_pipe_debug();
    processed = true;
  }

  if(argv[0] == "ad")
  {
    for(auto &a: argv)
    {
      cout << a << ":" << a.size() << endl;
    }
    processed = true;
  }

  if(argv[0] == "where")
  {
    if(argc == 2)
    {
      auto p = traverse_path(argv[1]);

      if(p.size() > 0)
      {
        for(auto &i: p)
        {
          cout << i << endl;
        }
      } else {
        cout << argv[1] << " not found in PATH " << endl;
      }

      processed = true;
    }
  }

  if(processed)
  {
    exit(0);
  }

  auto path = traverse_path(argv[0]);

  if(path.size() > 0)
  {
    execve(path[0], argv);
    processed = true;
  }

  if(!processed)
  {
    cout << "Unrecognized command: " << argv[0] << endl;
  }

  exit(EXIT_FAILURE);

  return true;
}

int parallel_pipe(vector<string> argv, int input, int first, int last, bool background, int background_pipe)
{
  if(argv[0] == "exit")
  {
    for(auto &i: running_processes)
    {
      kill(i, SIGKILL);
    }
    exit(0);
  }

  if(argv[0] == "export")
  {
    if(argv.size() == 2)
    {
      setenv(argv[1].c_str(), local_variables[argv[1]].c_str(), true);
    }

    update_vars();
    return true;
  }

  if(argv[0] == "cd")
  {
    if(argv.size() == 2)
    {
      cwd = internal_cd(argv[1], cwd);
      return true;
    }
  }

  int pipettes[2];

  /* Invoke pipe */
  pipe( pipettes );
  int pid = fork();

  /*
   SCHEME:
    STDIN --> O --> O --> O --> STDOUT
  */

  if (pid == 0) {
    if(background)
    {
      setsid();
      signal (SIGHUP, SIG_IGN);

      freopen( "/dev/null", "r", stdin);
      freopen( "/dev/null", "w", stdout);
      freopen( "/dev/null", "w", stderr);
    }

    if (first == 1 && last == 0 && input == 0) {
      // First command
      dup2( pipettes[WRITE], STDOUT_FILENO );
    } else if (first == 0 && last == 0 && input != 0) {
      // Middle command
      dup2(input, STDIN_FILENO);
      dup2(pipettes[WRITE], STDOUT_FILENO);
    } else {
      // Last command
      dup2( input, STDIN_FILENO );
      if(background)
      {
        dup2(background_pipe, STDOUT_FILENO);
      }
    }

    // execve(argv[0], argv);
    // exit(EXIT_FAILURE);
    micro_process(argv.size(), argv);
  } else {
    signal (SIGINT, SIG_IGN);
    if(background)
    {
      cout << pid << endl;
      running_processes.push_back(pid);
    }
  }

  if (input != 0)
    close(input);

  // Nothing more needs to be written
  close(pipettes[WRITE]);

  // If it's the last command, nothing more needs to be read
  if (last == 1 && !background)
    close(pipettes[READ]);

  return pipettes[READ];
}

string get_var(string name)
{
  string buf = name;

  if(local_variables.find(buf) != local_variables.end())
  {
    return local_variables[buf];
  }

  const char* local_buf = getenv(buf.c_str());

  if(local_buf != NULL)
  {
    buf = local_buf;
  }

  return buf;
}

void set_var(string name, string value)
{
  local_variables[name] = value;
}

string piped_parse(string command)
{
  int pipettes[2];

  pipe(pipettes);

  int flags = fcntl(pipettes[READ], F_GETFL, 0);
  fcntl(pipettes[READ], F_SETFL, flags | O_NONBLOCK);

  flags = fcntl(pipettes[WRITE], F_GETFL, 0);
  fcntl(pipettes[WRITE], F_SETFL, flags | O_NONBLOCK);

  std::string string_buffer;

  int pid = fork();

  if(pid == 0)
  {
    dup2(pipettes[WRITE], STDOUT_FILENO);
    parse(command);
    close(pipettes[READ]);
    close(pipettes[WRITE]);
    exit(0);
  } else {
    wait(NULL);

    char buffer[1024];

    bool flag = false;

    while(read(pipettes[READ], buffer, sizeof(buffer)) != 0 && !flag)
    {
      string_buffer = buffer;
      flag = true;
    }
  }

  close(pipettes[READ]);
  close(pipettes[WRITE]);

  return string_buffer;
}

void parse(string command)
{
  string buf;

  int argc = 0;
  vector<string> argv;

  bool pipeing = false;
  vector<pipe_resolution> pipe_pos;

  bool background = false;

  bool reading_variable = false;

  bool substitution = false;

  bool assign = false;
  string variable_name, value;

  for(int i = 0; i < command.size(); ++i)
  {
    if(substitution && !reading_variable && command[i] != '}' && command[i] != '$')
    {
      buf += command[i];
      continue;
    }

    if( (command[i] >= 'a' && command[i] <= 'z') || ( command[i] >= 'A' && command[i] <= 'Z' ) ||
        (command[i] >= '0' && command[i] <= '9') || (command[i] == '/' || command[i] == '.' ||
          command[i] == '-' || command[i] == '\'' || command[i] == '"' || command[i] == '_' )
        || command[i] == '?' || command[i] == '\\' || command[i] == '+' || command[i] == '(' || command[i] == ')' )
    {
      buf += command[i];
    } else if(command[i] == '&') {
      background = true;
    } else if(command[i] == '|') {
      pipeing = true;

      if(pipe_pos.size() != 0)
      {
        auto& p = pipe_pos.back();

        if(p.start == p.end)
        {
          p.end = argc;
        }
      }

      pipe_resolution p;

      p.start = argc;
      p.end = argc;

      pipe_pos.push_back(p);
    } else {

      if(command[i] == '$')
        reading_variable = true;

      if(command[i] == '{')
        substitution = true;

      if(command[i] == '}')
      {
        if(substitution)
        {
          substitution = false;
          if(reading_variable)
          {
            reading_variable = false;
            buf = string(get_var(buf));
          } else {
            // cout << " ";
            buf = piped_parse(buf);
            // cout << buf;
          }
        }
        continue;
      }

      if(command[i] == '=')
      {
        assign = true;
        variable_name = buf;
        buf = "";
      }


      if(buf.size() == 0)
        continue;

      if(assign)
      {
        value = buf;
        buf = "";

        assign = false;

        set_var(variable_name, value);

        continue;
      }

      if(reading_variable)
      {
        reading_variable = false;
        buf = get_var(buf);
      }

      ++argc;
      argv.push_back(buf);

      buf = "";
    }
  }

  if(buf[0] != 0x0)
  {
    if(reading_variable)
    {
      reading_variable = false;
      buf = get_var(buf);
    }

    if(assign)
    {
      value = buf;
      buf = "";

      assign = false;

      set_var(variable_name, value);
    } else {
      ++argc;
      argv.push_back(buf);
    }
  }

  if(argv.size() == 0) {
    return;
  }

  int j = argv.size();

  for(int i = 0; i < j; i++)
  {

    if(argv[i] == "if")
    {
      string lhs = argv[i+1];
      string rhs = argv[i+3];
      string op = argv[i+2];

      bool satisfied = false;

      if(op == "neq")
      {
        if(lhs != rhs)
        {
          satisfied = true;
        }
      }

      if(op == "eq")
      {
        if(lhs == rhs)
        {
          satisfied = true;
        }
      }

      if(op == "lt")
      {
        double lhs_d = stod(argv[i+1]);
        double rhs_d = stod(argv[i+3]);
        if(lhs_d < rhs_d)
        {
          satisfied = true;
        }
      }

      if(op == "gt")
      {
        double lhs_d = stod(argv[i+1]);
        double rhs_d = stod(argv[i+3]);
        if(lhs_d > rhs_d)
        {
          satisfied = true;
        }
      }

      if(op == "match")
      {
        regex r(rhs);

        smatch match;

        if(regex_search(lhs, match, r))
        {
          satisfied = true;
          int it = 0;
          for(auto &i: match)
          {
            set_var(to_string(it++), i);
          }
        }
      }

      if(satisfied)
      {
        argv.erase(argv.begin() + i, argv.begin() + i + 4);
        j -= 3;
        argc -= 4;
      } else {
        argv.erase(argv.begin() + i, argv.end());
        argc = i;
        break;
      }

    }
  }

  if(argv.size() == 0)
    return;

  if(pipe_pos.size() != 0)
  {
    auto& p = pipe_pos.back();

    if(p.start == p.end)
    {
      p.end = argc;
    }
  }

  pipe_resolution lp;

  int input = 0;
  int first = 1;

  lp.end = argv.size();
  lp.start = 0;

  for(int i = 0; i < pipe_pos.size(); ++i)
  {
    auto &p = pipe_pos[i];

    if(lp.start >= argc || p.start >= argc)
    {
      break;
    }

    std::vector<string> argv_n(argv.begin() + lp.start, argv.begin() + p.start);

    input = parallel_pipe(argv_n, input, first, 0, background, background_pipe[WRITE]);

    first = 0;
    lp = p;
  }

  std::vector<string> argv_n(argv.begin() + lp.start, argv.begin() + lp.end);

  input = parallel_pipe(argv_n, input, first, 1, background, background_pipe[WRITE]);

  for (int i = 0; i < pipe_pos.size() + 1 && !background; ++i)
    wait(NULL);

  // signal (SIGHUP, SIG_DFL);
}

void process(int sig)
{
  string command;

  // std::cout << unitbuf;

  while(true)
  {
    string disp = cwd + "@" + user + " $> ";
    cout << disp;
    fflush(stdout);

    command = read_line();

    // getline(cin, command);
    parse(command);
  }
}

void int_handler(int singal)
{
  cout << endl;
  process(singal);
}

void zombie_handler(int signal)
{
  int status;
  pid_t pid;

  while (1) {
    pid = wait3(&status, WNOHANG, (struct rusage *)NULL);
    if (pid == 0)
      return;
    else if (pid == -1)
      return;
    else ;
      //printf ("%d exited.\n", pid);
  }
}

int main()
{
  update_vars();

  signal(SIGCHLD, zombie_handler);
  // signal(SIGINT, int_handler);

  process(0);
}
