#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

// Create a new task structure with the given name, priority and burst
// add a task to the list at the tail
void add(struct node **taskHead, char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    task->tid = rand();

    insertTail(taskHead, task);
}

// Deletes all node variables in the list
void deleteAll(struct node **taskList) {
    struct node *temp, *current = *taskList;

    while (current != NULL) {
        temp = current->next;
        delete(taskList,current->task);
        current = temp;
    }
}


/* This function schedules all processes in a first come,
 * first served order, it will execute the earliest arrived
 * processes completely before executing the next.
 * */
void schedule(struct node **taskList) {

    struct node *temp, *current = *taskList;

    while (current != NULL) {
        temp = current->next;
        run(current->task, current->task->burst);
        printf("Task %s finished.\n",current->task->name);
        delete(taskList, current->task);
        current = temp;
    }

    deleteAll(taskList);
}