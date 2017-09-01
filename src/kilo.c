//
// Author: Ryan Fleming
// Date: 9/1/17
// Description: Version of antirez's kilo text editor written in C.
// Implements basic features as well as syntax highlighting and search.
//
#include <termios.h>
#include <unistd.h>
//
// Description: Enables raw mode.
//
void EnableRawMode() {
  //Read terminal's current attributes into a struct
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  //Turn off ECHO feature in struct
  raw.c_lflag &= ~(ECHO);
  //Pass the modified struct to write the new terminal attributes.
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
int main() {
  EnableRawMode();
  //Read 1 byte from standard input into the variable c
  //until there are no more bytes to read.
  //Quit program when it reads 'q' character.
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}
