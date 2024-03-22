#include "me.h"

/*** data ***/
struct termios orig_termios;

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;
/*** terminal ***/

void die(const char *s)
{
    if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
        return;
    if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
        return;

    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
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

int getCursorPosition(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;

    editorReadKey();
    return -1;
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void editorProcessKeypress()
{
    char c = editorReadKey();
    switch (c) {
    case CTRL_KEY('q'):
        if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
            return;
        if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
            return;
        exit(0);
        break;
    }
}

/*** append buffer ***/

struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }

void abAppend(struct abuf *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);
    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab)
{
    free(ab->b);
}

/*** output ***/

void editorDrawRows(struct abuf *ab)
{
    time_t now = time(NULL);
    struct tm *currtime;
    currtime = localtime(&now);
    char r_status[80];
    int r_len = snprintf(r_status, sizeof(r_status), "[ %2d:%2d:%2d ]",
                         currtime->tm_hour, currtime->tm_min, currtime->tm_sec);
    abAppend(ab, "\r", 2);
    abAppend(ab, r_status, r_len);
    abAppend(ab, "\x1b[K", 3);
}


void editorRefreshScreen()
{
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\x1b[?25l", 6);
    // abAppend(&ab, "\x1b[H", 3);
    editorDrawRows(&ab);
    // abAppend(&ab, "\x1b[H", 3);
    abAppend(&ab, "\x1b[?25h", 6);
    if (write(STDOUT_FILENO, ab.b, ab.len) != ab.len)
        return;
    abFree(&ab);
}

/*** input ***/

void initEditor()
{
    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
}

/*** input ***/
void *refresh_thread(void *dummy)
{
    int *status = malloc(sizeof(int));
    *status = 1;
    while (1) {
        editorRefreshScreen();
        usleep(1000);
    }
    return NULL;
}

void nextline()
{
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\r\n", 2);
    if (write(STDOUT_FILENO, ab.b, ab.len) != ab.len)
        return;
    abFree(&ab);
}

int process_key()
{
    enableRawMode();
    pthread_t thrdID;
    if (pthread_create(&thrdID, NULL, &refresh_thread, NULL)) {
        perror("pthread_create");
        return 1;
    }
    int signal = 0;
    while (signal == 0) {
        char c = editorReadKey();
        switch (c) {
        case CTRL_KEY('q'):
            pthread_cancel(thrdID);
            nextline();
            return 1;
        case CTRL_KEY('p'):
            signal = 1;
            break;
        }
    }
    pthread_cancel(thrdID);
    nextline();
    disableRawMode();
    return 0;
}