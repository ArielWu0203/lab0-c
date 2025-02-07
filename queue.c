#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *ele, *ele_safe;
    list_for_each_entry_safe (ele, ele_safe, l, list) {
        q_release_element(ele);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    else {
        // Init ele
        ele->value = strdup(s);
        INIT_LIST_HEAD(&(ele->list));
        // Add a list node to the beginning of the list
        list_add(&(ele->list), head);
    }
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    else {
        // Init ele
        ele->value = strdup(s);
        INIT_LIST_HEAD(&(ele->list));
        // Add a list node to the beginning of the list
        list_add_tail(&(ele->list), head);
    }
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp)
        strncpy(sp, node->value, strlen(node->value) + 1);
    return node;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    // find tail
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp)
        strncpy(sp, node->value, strlen(node->value) + 1);
    return node;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *ptr;

    list_for_each (ptr, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    int step = q_size(head) / 2;
    struct list_head *node = head->next;
    while (step--) {
        node = node->next;
    }
    list_del(node);
    q_release_element(list_entry(node, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *ptr = head->next;
    while (ptr != head && ptr->next != head) {  // ptr is not the tail or head
        element_t *first = list_entry(ptr, element_t, list);
        element_t *second = list_entry(ptr->next, element_t, list);
        if (!strcmp(first->value, second->value)) {
            while (second && !strcmp(first->value, second->value)) {
                list_del(&first->list);
                q_release_element(first);
                first = second;
                if (first->list.next != head) {
                    second = list_entry(first->list.next, element_t, list);
                } else {
                    second = NULL;
                }
            }
            ptr = first->list.next;
            list_del(&first->list);
            q_release_element(first);
        } else {
            ptr = ptr->next;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *first = head->next, *second = head->next->next;
    while (first != head && second != head) {
        first->next = second->next;
        first->next->prev = first;
        second->prev = first->prev;
        second->prev->next = second;
        first->prev = second;
        second->next = first;
        first = first->next;
        second = first->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *ptr = head;
    do {
        struct list_head *tmp = ptr->prev;
        ptr->prev = ptr->next;
        ptr->next = tmp;
        ptr = ptr->prev;
    } while (ptr != head);
    return;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *mergesort_list(struct list_head *head);
struct list_head *merge_two_lists(struct list_head *L1, struct list_head *L2);
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort_list(head->next);
    struct list_head *ptr = head;
    for (; ptr && ptr->next; ptr = ptr->next) {
        ptr->next->prev = ptr;
    }
    ptr->next = head;
    head->prev = ptr;
}

struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = mergesort_list(head), *right = mergesort_list(mid);
    return merge_two_lists(left, right);
}

struct list_head *merge_two_lists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; L1 && L2; *node = (*node)->next) {
        node = (strcmp(list_entry(L1, element_t, list)->value,
                       list_entry(L2, element_t, list)->value) <= 0)
                   ? &L1
                   : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    if (L1)
        *ptr = L1;
    else
        *ptr = L2;
    return head;
}
