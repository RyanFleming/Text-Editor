//
// Author: Ryan Fleming
// Date: 9/1/17
// Description: Version of antirez's kilo text editor written in C.
// Implements basic features as well as syntax highlighting and search.
//


/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  //Sets terminal to original state
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  //Read terminal's current attributes into a struct
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  //Modify copy of struct 
  //Fix ICRNL (Ctrl-M) so it as read as 13 instead of 10
  //disable XOFF (Ctrl-S) and XON (Ctrl-Q) 
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  //Turn off all output processing ("\n" to "\r\n")
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  //Turn off ECHO feature, canonical mode 
  //Disable IEXTEN (Ctrl-V)
  //Turn off SIGINT (Ctrl-C) and SIGTSTP (Ctrl-Z) signals 
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  //set minimum bytes of input before read() to 0 so all input is read
  raw.c_cc[VMIN] = 0;
  //set timeout of read() to 100 milliseconds
  raw.c_cc[VTIME] = 1;

  //Pass the modified struct to write the new terminal attributes.
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main() {
  enableRawMode();

  //Read 1 byte from standard input into the variable c
  //until there are no more bytes to read.
  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    //Test whether character is control character and print byte as decimal number
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    //Else print byte as decimal number and character directly
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    //If Ctrl-Q is pressed then quit
    if (c == CTRL_KEY('q')) break;
  }

  return 0;
}
