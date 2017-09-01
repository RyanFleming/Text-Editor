//
// Author: Ryan Fleming
// Date: 9/1/17
// Description: Version of antirez's kilo text editor written in C.
// Implements basic features as well as syntax highlighting and search.
//
#include <unistd.h>
int main() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1);
  return 0;
}
