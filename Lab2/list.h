/**
 * list data structure containing the tasks in the system
 */
#ifndef LIST_H
#define LIST_H
#include "task.h"

struct node {
    Task *task;
    struct node *next;
};

// insert and delete operations.
void insertHead(struct node **head, Task *newTask);
void insertTail(struct node **head, Task *newTask);
void delete(struct node **head, Task *task);
void traverse(struct node *head);

#endif