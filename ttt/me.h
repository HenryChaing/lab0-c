#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) &0x1f)

void die(const char *s);

void disableRawMode();

void enableRawMode();

char editorReadKey();

void editorProcessKeypress();

int process_key();