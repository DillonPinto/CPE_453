#include "schedulers.h"
#include <string.h>
#include "task.h"
#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

// Create a new task structure with the given name, priority and burst
// add a task to the list at the head
void add(struct node **taskHead, char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    task->tid = rand();

    insertHead(taskHead, task);
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



void schedule(struct node **taskList) {

    struct node *temp, *current = NULL, *shortestTask = NULL;
    int shortestTime = 1000;

    //sortAscending(taskList);
    //traverse(*taskList); //todo remove

    current = *taskList;
    while (current != NULL) {
        temp = current->next;

        if (current->task->burst < shortestTime) {
            shortestTime = current->task->burst;
            shortestTask = current;
        }

        current = temp;
        if (current == NULL && (*taskList) == NULL) {
            break;
        }
        else if (current == NULL && (*taskList) != NULL) {
            run(shortestTask->task, shortestTask->task->burst);
            printf("Task %s finished.\n",shortestTask->task->name);
            delete(taskList, shortestTask->task);
            shortestTime = 1000;
            current = *taskList;

        }

    }

    deleteAll(taskList);

}
