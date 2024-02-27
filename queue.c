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
    if(!head){
        return false;
    }
    
    element_t *new_node = (element_t *) malloc(sizeof(element_t));

    if(!new_node){
        return false;
    }
    
    char *str_copy = (char *) malloc(strlen(s)+1);

    if(!str_copy){
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
    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    char *str_copy = (char *) malloc(strlen(s));
    int i;
    for (i = 0; i < strlen(s); i++) {
        *(str_copy + i) = *(s + i);
    }
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
    element_t *return_element = container_of(head->next, element_t, list);
    head->next->next->prev = head;
    head->next = head->next->next;
    return return_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *return_element = container_of(head->prev, element_t, list);
    head->prev->prev->next = head;
    head->prev = head->prev->prev;
    return return_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
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
    int mid_index = length / 2;
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
    int str_mem_count = 0;
    int delete_sign = 0;
    char **str_mem = malloc(q_size(head) * sizeof(char *));
    list_head *pt = head->next;

    for (size_t i = 0; i < q_size(head); i++) {
        str_mem[i] = (char *) malloc(100 * sizeof(char));
    }

    while (pt != head) {
        element_t *node = container_of(pt, element_t, list);
        for (size_t i = 0; i < str_mem_count; i++) {
            if (strcmp(str_mem[i], node->value) == 0) {
                pt->next->prev = pt->prev;
                pt->prev->next = pt->next;
                pt = pt->next;
                delete_sign = 1;
            }
        }

        if (delete_sign == 1) {
            delete_sign = 0;
            free(node->value);
            free(node);
        } else {
            strncpy(str_mem[str_mem_count++], node->value, 100);
            pt = pt->next;
        }
    }

    for (size_t i = 0; i < q_size(head); i++) {
        free(str_mem[i]);
    }
    free(str_mem);

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
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
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
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
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
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