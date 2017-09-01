//
// Author: Ryan Fleming
// Date: 9/1/17
// Description: Version of antirez's kilo text editor written in C.
// Implements basic features as well as syntax highlighting and search.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
  //Sets terminal to original state
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  //Read terminal's current attributes into a struct
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  //Modify copy of struct 
  //Fix ICRNL (Ctrl-M) so it as read as 13 instead of 10
  //disable XOFF (Ctrl-S) and XON (Ctrl-Q) 
  raw.c_iflag &= ~(ICRNL | IXON);
  //Turn off ECHO feature, canonical mode 
  //Disable IEXTEN (Ctrl-V)
  //Turn off SIGINT (Ctrl-C) and SIGTSTP (Ctrl-Z) signals 
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  //Pass the modified struct to write the new terminal attributes.
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();

  //Read 1 byte from standard input into the variable c
  //until there are no more bytes to read.
  //Quit program when it reads 'q' character.
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    //Test whether character is control character and print byte as decimal number
    if (iscntrl(c)) {
      printf("%d\n", c);
    //Else print byte as decimal number and character directly
    } else {
      printf("%d ('%c')\n", c, c);
    }
  }

  return 0;
}
