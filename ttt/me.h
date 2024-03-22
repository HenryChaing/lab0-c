/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/*** define ***/
#define KILO_VERSION "0.0.1"

#define CTRL_KEY(k) ((k) &0x1f)

int process_key();