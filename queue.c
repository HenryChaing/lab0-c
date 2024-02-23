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

/* Create an empty queue */
list_head *q_new()
{
    list_head *temp = (list_head *) malloc(sizeof(list_head));
    temp->next = temp;
    temp->prev = temp;
    return temp;
}

/* Free all storage used by queue */
void q_free(list_head *l)
{
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
    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    char *str_copy = (char *) malloc(strlen(s));
    int i;
    for (i = 0; i < strlen(s); i++) {
        *(str_copy + i) = *(s + i);
    }
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
    list_head *front = head->next;
    list_head *last = head;
    list_head *temp = NULL;

    do {
        last->prev = front;
        temp = front->next;
        front->next = last;

        last = front;
        front = temp;
    } while (last != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

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
