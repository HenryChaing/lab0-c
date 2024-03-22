#include "main_ttt.h"

int move_record[N_GRIDS];
int move_count = 0;
int move_record_arr[50][N_GRIDS];
int record_arr_len = 0;

void record_move(int move)
{
    move_record[move_count++] = move;
}

void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        printf("%c%d", 'A' + GET_COL(move_record[i]),
               1 + GET_ROW(move_record[i]));
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void print_all_moves()
{
    for (int i = 0; i < record_arr_len; i++) {
        printf("Battle %d, Moves: ", i + 1);
        for (int j = 1; j <= move_record_arr[i][0]; j++) {
            printf("%c%d", 'A' + GET_COL(move_record_arr[i][j]),
                   1 + GET_ROW(move_record_arr[i][j]));
            if (j < move_record_arr[i][0]) {
                printf(" -> ");
            }
        }
        printf("\n");
    }
}

int get_input(char player)
{
    char *line = NULL;
    size_t line_length = 0;
    int parseX = 1;

    int x = -1, y = -1;
    while (x < 0 || x > (BOARD_SIZE - 1) || y < 0 || y > (BOARD_SIZE - 1)) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1)
            exit(1);
        if (r < 2)
            continue;
        x = 0;
        y = 0;
        parseX = 1;
        for (int i = 0; i < (r - 1); i++) {
            if (isalpha(line[i]) && parseX) {
                x = x * 26 + (tolower(line[i]) - 'a' + 1);
                if (x > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    x = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // input does not have leading alphabets
            if (x == 0) {
                printf("Invalid operation: No leading alphabet\n");
                y = 0;
                break;
            }
            parseX = 0;
            if (isdigit(line[i])) {
                y = y * 10 + line[i] - '0';
                if (y > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    y = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // any other character is invalid
            // any non-digit char during digit parsing is invalid
            // TODO: Error message could be better by separating these two cases
            printf("Invalid operation\n");
            x = y = 0;
            break;
        }
        x -= 1;
        y -= 1;
    }
    free(line);
    return GET_INDEX(y, x);
}


struct task {
    jmp_buf env;
    struct list_head list;
    char task_name[10];
    char *table;
    char turn;
};

struct arg {
    char *task_name;
    char *table;
    char turn;
};

static LIST_HEAD(tasklist);
static void (**tasks)(void *);
static struct arg *args;
static int ntasks;
static jmp_buf sched;
static struct task *cur_task;

static void task_add(struct task *task)
{
    list_add_tail(&task->list, &tasklist);
}

static void task_switch()
{
    if (!list_empty(&tasklist)) {
        struct task *t = list_first_entry(&tasklist, struct task, list);
        list_del(&t->list);
        cur_task = t;
        longjmp(t->env, 1);
    }
}

void schedule(void)
{
    static int i;
    i = 0;
    setjmp(sched);
    while (ntasks-- > 0) {
        struct arg arg = args[i];
        tasks[i++](&arg);
        printf("Never reached\n");
    }
    task_switch();
}


/*negamax*/
void task0(void *arg)
{
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 6);
    task->table = ((struct arg *) arg)->table;
    task->turn = ((struct arg *) arg)->turn;
    INIT_LIST_HEAD(&task->list);

    printf("%s: n = %c\n", task->task_name, task->turn);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }
    while (1) {
        task = cur_task;

        if (setjmp(task->env) == 0) {
            char win = check_win(task->table);
            if (win == 'D') {
                draw_board(task->table);
                printf("It is a draw!\n");
                break;
            } else if (win != ' ') {
                draw_board(task->table);
                printf("%c won!\n", win);
                break;
            }

            draw_board(task->table);
            int move = negamax_predict(task->table, task->turn).move;
            if (move != -1) {
                task->table[move] = task->turn;
                record_move(move);
            }

            task_add(task);
            printf("%s: n = %c,  continue...\n", task->task_name, task->turn);
            task_switch();
        }
    }

    printf("%s: complete\n", task->task_name);
    longjmp(sched, 1);
}

/*mcts*/
void task1(void *arg)
{
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 6);
    task->table = ((struct arg *) arg)->table;
    task->turn = ((struct arg *) arg)->turn;
    INIT_LIST_HEAD(&task->list);

    printf("%s: n = %c\n", task->task_name, task->turn);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }
    while (1) {
        task = cur_task;

        if (setjmp(task->env) == 0) {
            char win = check_win(task->table);
            if (win == 'D') {
                draw_board(task->table);
                printf("It is a draw!\n");
                break;
            } else if (win != ' ') {
                draw_board(task->table);
                printf("%c won!\n", win);
                break;
            }

            draw_board(task->table);
            int move = mcts(task->table, task->turn);
            if (move != -1) {
                task->table[move] = task->turn;
                record_move(move);
            }

            task_add(task);
            printf("%s: n = %c,  continue...\n", task->task_name, task->turn);
            task_switch();
        }
    }

    printf("%s: complete\n", task->task_name);
    longjmp(sched, 1);
}

/*keyborad_handler*/
void task2(void *arg)
{
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 6);
    task->table = ((struct arg *) arg)->table;
    // task->turn = ((struct arg *) arg)->turn;
    INIT_LIST_HEAD(&task->list);

    printf("%s: n \n", task->task_name);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }
    while (1) {
        task = cur_task;

        if (setjmp(task->env) == 0) {
            char win = check_win(task->table);
            if (win == 'D') {
                break;
            }

            if (win != ' ') {
                break;
            }

            int finish = process_key();

            if (finish == 1) {
                print_all_moves();
                exit(0);
            }

            task_add(task);
            printf("%s: n ,  continue...\n", task->task_name);
            task_switch();
        }
    }

    printf("%s: complete\n", task->task_name);
    longjmp(sched, 1);
}


void main_ttt(int mode)
{
    srand(time(NULL));
    char table[N_GRIDS];
    memset(table, ' ', N_GRIDS);
    char turn = 'X';
    char ai = 'O';

    void (*registered_task[])(void *) = {task0, task1, task2};
    struct arg arg0 = {.table = table, .turn = 'X', .task_name = "Task 0"};
    struct arg arg1 = {.table = table, .turn = 'O', .task_name = "Task 1"};
    struct arg arg2 = {.table = table, .task_name = "Task 2"};
    struct arg registered_arg[] = {arg0, arg1, arg2};

    if (mode > 0) {
        negamax_init();
    }

    while (1) {
        char win = check_win(table);
        if (win == 'D') {
            draw_board(table);
            printf("It is a draw!\n");
            break;
        } else if (win != ' ') {
            draw_board(table);
            printf("%c won!\n", win);
            break;
        }
        if (mode == 0) {
            if (turn == ai) {
                int move = mcts(table, ai);
                if (move != -1) {
                    table[move] = ai;
                    record_move(move);
                }
            } else {
                draw_board(table);
                int move;
                while (1) {
                    move = get_input(turn);
                    if (table[move] == ' ') {
                        break;
                    }
                    printf("Invalid operation: the position has been marked\n");
                }
                table[move] = turn;
                record_move(move);
            }
        } else if (mode == 1) {
            if (turn == ai) {
                draw_board(table);
                int move = mcts(table, ai);
                if (move != -1) {
                    table[move] = ai;
                    record_move(move);
                }
            } else {
                draw_board(table);
                int move = negamax_predict(table, turn).move;
                if (move != -1) {
                    table[move] = turn;
                    record_move(move);
                }
            }
        } else if (mode == 2) {
            tasks = registered_task;
            args = registered_arg;
            ntasks = ARRAY_SIZE(registered_task);

            schedule();

            for (int i = 0; i < N_GRIDS; i++) {
                table[i] = ' ';
            }

            for (int i = 0; i < move_count; i++) {
                move_record_arr[record_arr_len][i + 1] = move_record[i];
                move_record[i] = 0;
            }
            move_record_arr[record_arr_len][0] = move_count;
            move_count = 0;
            record_arr_len++;
        }

        turn = turn == 'X' ? 'O' : 'X';
    }
    print_moves();
}