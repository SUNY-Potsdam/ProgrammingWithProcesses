/**
 * counterA/B
 * Demonstrate filesystem::path data type and the execl function call.
 *
 * This program loops, printing a message at each iteration with a
 * counter value and the name of the executable. The number of
 * iterations and starting value of the counter can be specified on
 * the command line; if they are not, 10 iterations count from 0. When
 * counterA finishes, it executes counterB, setting the start value so
 * that it keeps counting from where counterA left off; counterB is
 * the same but for executing counterA when it is done.
 *
 * filesystem::path is used to make splitting and joining a path (the path to the executable and its partner executable)
 */

#include <iostream>
#include <string>
#include <unistd.h>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

/**
 * Execute the file named in partner, passing loops and start_value as
 * command-line arguments for the newly executed program.
 *
 * @param partner the path to the executable to be run
 * @param loops number of loops or first argument on executed command-line
 * @param start_value where to start the counter or second command-line argument
 * @note should never return
 */
void exec_partner(const fs::path partner, int loops, int start_value) {
  string partner_loops = to_string(loops);
  string partner_start_value = to_string(start_value);
  /*
   * execl(const char *path, const char *arg, ...)
   *       ^                 ^- variable number of null terminated arguments
   *       |                    provided to executed program as argv; the first
   *       |                    arg should be the name of the file executed and
   *       |                    the list must end with a (char *) NULL (cast is
   *       |                    necessary).
   *       |- the path of the executable
   */
  execl(partner.c_str(),
        partner.c_str(), partner_loops.c_str(), partner_start_value.c_str(), (char *) nullptr);
  //    argv[0],         argv[1],               argv[2],                     end of argv
  //    executable path  loops                  start_value                  nullptr

  // execl only returns if there is some problem with the attempt to run
  // the given command. Otherwise, the call changes the process's image
}


int main(int argc, char * argv[]) {
  vector<string> arguments; // command-line arguments translated to strings
  for (int aNdx = 0; aNdx < argc; ++aNdx)
    arguments.push_back(argv[aNdx]);

  // get the path to the running program (the first argument)
  fs::path executable(arguments[0]); // path to the program in this process

  fs::path executable_path = executable.parent_path(); // up to last slash
  fs::path executable_name = executable.stem(); // after slash, ignore extension

  // manipulate the name of the executable: switch an A for a B (or vice versa).
  string partner_str = executable_name.string();
  partner_str.back() = (partner_str.back()=='A')?'B':'A';
  fs::path partner_name(partner_str);

  // append the path (/ overloaded for path objects; composition seems natural)
  fs::path partner = executable_path / partner_name;

  int loops = 10;  // number of loop iterations; defaults to 10
  if (arguments.size() > 1)    // unless a loop count argument is given
    loops = stoi(arguments[1]);

  int counter = 0; // starting value to print; defaults to 0
  if (arguments.size() > 2)    // unless a counter argument is given
    counter = stoi(arguments[2]);

  // name of program to display while printing; defaults to name of executable
  string display_name = executable_name.string();
  if (arguments.size() > 3)   // unless a display name argument is given
    display_name = arguments[3];

  // ***** The main body of the program *****
  for (int i = 0; i < loops; ++i)
    cout << display_name << "(" << getpid() << "): counts " <<  counter++ << endl;
  cout.flush(); // make sure any buffered output is finished

  /* Time to run our partner program. Name of the program is given in the
   * partner variable and it is assumed to be in the same directory where this
   * program lives.
   */
  exec_partner(partner, loops, counter);
  return 0;                        // 0 returned from main means no error
}
