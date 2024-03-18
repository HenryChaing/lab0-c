#include "me.h"

struct termios orig_termios;

void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

/*** input ***/
void editorProcessKeypress() {}

int process_key()
{
    enableRawMode();
    int signal = 0;
    while (signal == 0) {
        char c = editorReadKey();
        switch (c) {
        case CTRL_KEY('q'):
            return 1;
        case CTRL_KEY('p'):
            signal = 1;
            break;
        }
    }
    disableRawMode();
    return 0;
}