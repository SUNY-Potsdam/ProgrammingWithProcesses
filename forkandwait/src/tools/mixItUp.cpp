/**
 * Demonstate how fork works
 *
 * Program runs with an optional "wait" argument (parent waits for
 * child before running) and a child return code. Then it forks a new
 * process. If all went well, the parent counts up to the number of
 * loops and the child counts down. The parent uses wait to capture
 * the return code of the child process.
 */

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char * argv[]) {
  vector<string> arguments; // command-line arguments translated to strings
  for (int aNdx = 0; aNdx < argc; ++aNdx)
    arguments.push_back(argv[aNdx]);

  bool parent_should_wait_before
      = (arguments.size() > 1) && (arguments[1] == "wait");

  int child_exit_code = (arguments.size() > 2)? stoi(arguments[2]) : 0;

  string display_name = (arguments.size() > 3)? arguments[3] : arguments[0];

  // these could be set from the command-line
  const int loops = 25;

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(4, 25);


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
    while (parent_should_wait_before && terminating_pid != child_pid)
      terminating_pid = wait(&exit_status);

    for (int i = 0; i < loops; ++i) {
      string output = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      for (char ch : output) {
        cout << ch;
        cout.flush();
        usleep(1050*dis(gen));
      }
      cout << endl;
    }

    while (!parent_should_wait_before && terminating_pid != child_pid)
      terminating_pid = wait(&exit_status);

    if (WIFEXITED(exit_status))
      cout << display_name << " sees child (" << child_pid << ") return "
           << WEXITSTATUS(exit_status) << endl;
     else
      cout << display_name << " sees child (" << child_pid << ") return abnormally."
           << endl;
  } else {   // child
    display_name += "_child";
    for (int i = 0; i < loops; ++i) {
      string output = "abcdefghijklmnopqrstuvwxyz";
      for (char ch : output) {
        cout << ch;
        cout.flush();
        usleep(1000*dis(gen));
      }
      cout << endl;
    }
    return child_exit_code;
  }
}
