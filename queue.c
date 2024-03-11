#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

typedef struct list_head list_head;
list_head *my_merge(list_head *a, list_head *b, bool descend);

/* Create an empty queue */
list_head *q_new()
{
    list_head *head = (list_head *) malloc(sizeof(list_head));
    if (!head) {
        return NULL;
    }
    head->next = head;
    head->prev = head;

    return head;
}

/* Free all storage used by queue */
void q_free(list_head *l)
{
    if (!l) {
        return;
    }
    list_head *pt = l->next;
    while (pt != l) {
        element_t *node = container_of(pt, element_t, list);
        pt = pt->next;
        free(node->value);
        free(node);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    int s_length = strlen(s);

    if (!head) {
        return false;
    }

    element_t *new_node = (element_t *) malloc(sizeof(element_t));

    if (!new_node) {
        return false;
    }

    char *str_copy = (char *) malloc(s_length + 1);

    if (!str_copy) {
        free(new_node);
        return false;
    }

    int i;
    for (i = 0; i < s_length; i++) {
        *(str_copy + i) = *(s + i);
    }
    *(str_copy + i) = '\0';
    new_node->value = str_copy;
    new_node->list.prev = head;
    new_node->list.next = head->next;
    head->next->prev = &new_node->list;
    head->next = &new_node->list;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    int s_length = strlen(s);

    if (!head) {
        return false;
    }

    element_t *new_node = (element_t *) malloc(sizeof(element_t));

    if (!new_node) {
        return false;
    }

    char *str_copy = (char *) malloc(s_length + 1);

    if (!str_copy) {
        free(new_node);
        return false;
    }

    int i;
    for (i = 0; i < s_length; i++) {
        *(str_copy + i) = *(s + i);
    }
    *(str_copy + i) = '\0';
    new_node->value = str_copy;
    new_node->list.next = head;
    new_node->list.prev = head->prev;
    head->prev->next = &new_node->list;
    head->prev = &new_node->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (q_size(head) == 0 || !sp) {
        return NULL;
    }

    element_t *return_element = container_of(head->next, element_t, list);
    head->next->next->prev = head;
    head->next = head->next->next;
    char *value = return_element->value;
    int s_length = strlen(value);

    size_t i;
    for (i = 0; i < s_length && i < bufsize - 1; i++) {
        *(sp + i) = *(value + i);
    }
    *(sp + i) = '\0';

    return return_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (q_size(head) == 0 || !sp) {
        return NULL;
    }

    element_t *return_element = container_of(head->prev, element_t, list);
    head->prev->prev->next = head;
    head->prev = head->prev->prev;
    char *value = return_element->value;
    int s_length = strlen(value);

    size_t i;
    for (i = 0; i < s_length && i < bufsize - 1; i++) {
        *(sp + i) = *(value + i);
    }
    *(sp + i) = '\0';

    return return_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head) {
        return 0;
    }

    int count = 0;
    list_head *pt = head;
    while (pt->next != head) {
        count++;
        pt = pt->next;
    }

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    int length = q_size(head);
    int mid_index = (length + 1) / 2;
    list_head *pt = head;

    for (size_t i = 0; i < mid_index; i++) {
        pt = pt->next;
    }

    if (length == 0) {
        return false;
    } else if (length == 1) {
        pt = pt->next;
    }

    element_t *delete_node = container_of(pt, element_t, list);
    pt->prev->next = pt->next;
    pt->next->prev = pt->prev;
    free(delete_node->value);
    free(delete_node);

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head) {
        return false;
    }
    list_head *last = head->next;
    list_head *front = last->next;
    bool delete = false;
    while (last != head) {
        element_t *last_node = container_of(last, element_t, list);
        while (front != head) {
            element_t *front_node = container_of(front, element_t, list);
            if (strcmp(last_node->value, front_node->value) == 0) {
                front->prev->next = front->next;
                front->next->prev = front->prev;
                front = front->next;
                free(front_node->value);
                free(front_node);
                delete = true;
                continue;
            }
            front = front->next;
        }
        if (delete) {
            last->prev->next = last->next;
            last->next->prev = last->prev;
            last = last->next;
            front = last->next;
            free(last_node->value);
            free(last_node);
            delete = false;
            continue;
        }
        last = last->next;
        front = last->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    list_head *front = head->next->next;
    list_head *last = head->next;

    while (front != head && last != head) {
        (last)->prev->next = front;
        (front)->next->prev = last;
        (last)->next = (front)->next;
        (front)->prev = (last)->prev;
        (front)->next = last;
        (last)->prev = front;

        front = ((front)->next->next->next);
        last = ((last)->next);
    }

    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (q_size(head) == 0) {
        return;
    }
    list_head *last = head;
    list_head *front = head->next;
    do {
        last->next = last->prev;
        last->prev = front;
        front = front->next;
        last = last->prev;
    } while (head != last);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    int length = q_size(head);
    int turn = length / k;
    list_head *front = head->next->next;
    list_head *last = head->next;

    for (size_t i = 0; i < turn; i++) {
        list_head *ll = last->prev;
        for (size_t j = 0; j < k; j++) {
            last->next = last->prev;
            last->prev = front;
            front = front->next;
            last = last->prev;
        }
        list_head *ff = last;
        last = ll->next;
        front = ff->prev;
        last->next = ff;
        ff->prev = last;
        front->prev = ll;
        ll->next = front;
        last = ff;
        front = ff->next;
    }

    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    list_head *front = head->next->next;
    list_head *last = head->next;

    while (last->next != head) {
        element_t *last_node = container_of(last, element_t, list);
        while (front != head) {
            element_t *front_node = container_of(front, element_t, list);
            if (strcmp(last_node->value, front_node->value) > 0) {
                last->prev->next = last->next;
                last->next->prev = last->prev;
                last = last->prev;
                free(last_node->value);
                free(last_node);
                break;
            }
            front = front->next;
        }
        last = last->next;
        front = last->next;
    }

    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    list_head *front = head->next->next;
    list_head *last = head->next;

    while (last->next != head) {
        element_t *last_node = container_of(last, element_t, list);
        while (front != head) {
            element_t *front_node = container_of(front, element_t, list);
            if (strcmp(last_node->value, front_node->value) < 0) {
                last->prev->next = last->next;
                last->next->prev = last->prev;
                last = last->prev;
                free(last_node->value);
                free(last_node);
                break;
            }
            front = front->next;
        }
        last = last->next;
        front = last->next;
    }

    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

bool q_shuffle(struct list_head *head)
{
    list_head *node_i;
    list_head *node_j;
    list_head *nodei_prev, *nodej_prev;
    int size = q_size(head);

    for (int i = size; i > 1; i--) {
        int j = rand() % (i) + 1;

        node_i = head;
        node_j = head;

        for (size_t inner_i = 0; inner_i < i; inner_i++) {
            node_i = node_i->next;
        }

        for (size_t inner_i = 0; inner_i < j; inner_i++) {
            node_j = node_j->next;
        }

        int distance = i - j;

        if (distance == 0) {
        } else if (distance == 1) {
            node_j->prev->next = node_i;
            node_i->next->prev = node_j;
            node_j->next = node_i->next;
            node_i->prev = node_j->prev;
            node_i->next = node_j;
            node_j->prev = node_i;

        } else {
            nodei_prev = node_i->prev;
            nodej_prev = node_j->prev;
            node_i->prev->next = node_i->next;
            node_i->next->prev = node_i->prev;
            node_j->prev->next = node_j->next;
            node_j->next->prev = node_j->prev;
            list_add(node_i, nodej_prev);
            list_add(node_j, nodei_prev);
        }
    }

    return true;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    list_head *q = container_of(head->next, queue_contex_t, chain)->q;
    list_head *chain_context = head->next->next;

    while (chain_context != head) {
        list_head *merge_list =
            container_of(chain_context, queue_contex_t, chain)->q;
        q = my_merge(q, merge_list, descend);
        chain_context = chain_context->next;
    }
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(q);
}

list_head *my_merge(list_head *a, list_head *b, bool descend)
{
    if (q_size(a) == 0) {
        return b;
    } else if (q_size(b) == 0) {
        return a;
    }

    a->prev->next = b->next;
    b->next->prev = a->prev;
    a->prev = b->prev;
    b->prev->next = a;
    b->prev = b;
    b->next = b;

    q_sort(a, descend);
    return a;
}


static bool compare(void *priv, struct list_head *a, struct list_head *b)
{
    element_t *nodeA = container_of(a, element_t, list);
    element_t *nodeB = container_of(b, element_t, list);
    if (priv) {
        *((int *) priv) += 1;
    }
    if (strcmp(nodeA->value, nodeB->value) <= 0) {
        return false;
    }
    return true;
}

// bool (*cmp)(struct list_head *a,struct list_head *b);

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct list_head *merge(void *priv,
                               bool (*cmp)(void *priv,
                                           struct list_head *a,
                                           struct list_head *b),
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL;
    struct list_head **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (!cmp(priv, a, b)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static void merge_final(void *priv,
                        bool (*cmp)(void *priv,
                                    struct list_head *a,
                                    struct list_head *b),
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (!cmp(priv, a, b)) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    build_prev_link(head, tail, b);
}

static struct pair find_run(void *priv,
                            struct list_head *list,
                            bool (*cmp)(void *priv,
                                        struct list_head *a,
                                        struct list_head *b))
{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (cmp(priv, list, next) > 0) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next && cmp(priv, list, next) > 0);
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && !cmp(priv, list, next));
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(void *priv,
                                  bool (*cmp)(void *priv,
                                              struct list_head *a,
                                              struct list_head *b),
                                  struct list_head *at)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(priv, cmp, at->prev, at);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(void *priv,
                                              bool (*cmp)(void *priv,
                                                          struct list_head *a,
                                                          struct list_head *b),
                                              struct list_head *tp)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(priv, cmp, tp->prev);
        } else {
            tp = merge_at(priv, cmp, tp);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(void *priv,
                                        bool (*cmp)(void *priv,
                                                    struct list_head *a,
                                                    struct list_head *b),
                                        struct list_head *tp)
{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(priv, cmp, tp->prev);
            } else {
                tp = merge_at(priv, cmp, tp);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(priv, cmp, tp);
        } else {
            break;
        }
    }

    return tp;
}

void timsort(void *priv,
             struct list_head *head,
             bool (*cmp)(void *priv, struct list_head *a, struct list_head *b))
{
    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;
    if (head == head->prev)
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(priv, list, cmp);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(priv, cmp, tp);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(priv, cmp, tp);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(priv, cmp, head, stk1, stk0);
}

static list_head *kernel_merge(void *priv,
                               bool (*cmp)(void *priv,
                                           struct list_head *a,
                                           struct list_head *b),
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void kernel_merge_final(void *priv,
                               struct list_head *head,
                               bool (*cmp)(void *priv,
                                           struct list_head *a,
                                           struct list_head *b),
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *tail = head;
    int count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b)) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        if (!++count)
            cmp(priv, a, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

/* Sort elements of queue in ascending/descending order */
void kernel_sort(void *priv,
                 struct list_head *head,
                 bool (*cmp)(void *priv,
                             struct list_head *a,
                             struct list_head *b))
{
    if (q_size(head) == 0 || q_size(head) == 1) {
        return;
    }

    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;


    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = kernel_merge(priv, cmp, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = kernel_merge(priv, cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    kernel_merge_final(priv, head, cmp, pending, list);
}


/* Sort elements of queue in ascending/descending order */
void q_timsort(struct list_head *head, bool descend)
{
    int count = 0;
    clock_t begin = clock();
    timsort(&count, head, compare);
    printf("  Elapsed time:   %ld\n", clock() - begin);
    printf("  Comparisons:    %d\n", count);
    if (descend) {
        q_reverse(head);
    }
}

void q_sort(struct list_head *head, bool descend)
{
    int count = 0;
    clock_t begin = clock();
    kernel_sort(&count, head, compare);
    printf("  Elapsed time:   %ld\n", clock() - begin);
    printf("  Comparisons:    %d\n", count);
    if (!descend) {
        q_reverse(head);
    }
}