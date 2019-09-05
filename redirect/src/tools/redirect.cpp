/**
 * redirect
 * Demonstrate dup for file descriptors shared between exec'd processes
 *
 * The child process opens a file descriptor for reading the source
 * code of the program using the C language open command. The returned
 * FD index (an index into the operating system's table of file
 * descriptors kept for the child process) is then duplicated to stdin
 * (the function is dup2; read that as a text message: duplicate file
 * descriptor to other index). Then it executes the system sort
 * program which sorts and prints the lines in the source.
 */

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char * argv[]) {
  vector<string> arguments; // command-line arguments translated to strings
  for (int aNdx = 0; aNdx < argc; ++aNdx) {
    arguments.push_back(argv[aNdx]);
  }
  string display_name = arguments[0];

  pid_t child_pid = fork();
  /*| child_pid      | Meaning                                          |
    |----------------|--------------------------------------------------|
    | child_pid < 0  | parent; there error trying to fork a new process |
    | child_pid == 0 | child                                            |
    | child_pid > 0  | parent; child_pid is just that                   |*/

  if (child_pid < 0) {
    cerr << "Failure to fork child process." << endl;
    exit(1);
  }

  if (child_pid) { // parent
    display_name += "_parent";
    cout << display_name << " sees child ("<< child_pid << ")" << endl;


    pid_t terminating_pid = 0;
    int exit_status;

    while (terminating_pid != child_pid)
      terminating_pid = wait(&exit_status);

    if (WIFEXITED(exit_status) && (WEXITSTATUS(exit_status) != 0)) {
      cerr << display_name << " sees error in child: " << WEXITSTATUS(exit_status) << endl;
      return WEXITSTATUS(exit_status);
    }
  } else {   // child
    display_name += "_child";
    int input_file_descriptor = open("src/tools/redirect.cpp", O_RDONLY);
    dup2(input_file_descriptor, STDIN_FILENO);

    string cmd = "/usr/bin/sort";
    execl(cmd.c_str(), cmd.c_str(), (const char *) nullptr);
    // run the sort command
  }
  return 0;                        // 0 returned from main means no error
}
