#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

typedef struct list_head list_head;
static list_head *merge(struct list_head *a, struct list_head *b);
static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b);
static bool compare(struct list_head *a, struct list_head *b);

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
    if (!head) {
        return false;
    }

    element_t *new_node = (element_t *) malloc(sizeof(element_t));

    if (!new_node) {
        return false;
    }

    char *str_copy = (char *) malloc(strlen(s) + 1);

    if (!str_copy) {
        free(new_node);
        return false;
    }

    int i;
    for (i = 0; i < strlen(s); i++) {
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
    if (!head) {
        return false;
    }

    element_t *new_node = (element_t *) malloc(sizeof(element_t));

    if (!new_node) {
        return false;
    }

    char *str_copy = (char *) malloc(strlen(s) + 1);

    if (!str_copy) {
        free(new_node);
        return false;
    }

    int i;
    for (i = 0; i < strlen(s); i++) {
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
    size_t i;

    for (i = 0; i < strlen(return_element->value) && i < bufsize - 1; i++) {
        *(sp + i) = *(return_element->value + i);
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
    size_t i;

    for (i = 0; i < strlen(return_element->value) && i < bufsize - 1; i++) {
        *(sp + i) = *(return_element->value + i);
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

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
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

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(b, a);
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
        list = merge(pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list);
    if (!descend) {
        q_reverse(head);
    }
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

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}


static list_head *merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (compare(a, b)) {
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

static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    int count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (compare(a, b)) {
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
            compare(a, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static bool compare(struct list_head *a, struct list_head *b)
{
    element_t *nodeA = container_of(a, element_t, list);
    element_t *nodeB = container_of(b, element_t, list);
    if (strcmp(nodeA->value, nodeB->value) <= 0) {
        return false;
    }
    return true;
}