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
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct editorConfig {
  struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ***/

void die(const char *s) {
  //Clear screen 
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  
  perror(s);
  exit(1);
}

void disableRawMode() {
  //Sets terminal to original state
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  //Read terminal's current attributes into a struct
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;
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

char editorReadKey() {
  //Wait for one keypress and returns it
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  //place the number of columns and rows into winsize struct
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** output ***/

void editorDrawRows() {
  //draw column of '~' on left side like vim
  int y;
  for (y = 0; y < 24; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

void editorRefreshScreen() {
  //Clear screen
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  editorDrawRows();

  //Reposition cursor
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/

void editorProcessKeypress() {
  //Waits for keypress then handles it
  char c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):
      //Clear screen
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
}
 

/*** init ***/

int main() {
  enableRawMode();

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  
  return 0;
}
