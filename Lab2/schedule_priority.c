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



// Schedules all tasks in order of descending priority (highest priority first)
void schedule(struct node **taskList) {

    struct node *temp, *current = NULL, *principalTask = NULL;
    int highestPriority = 11;

    // initialize current pointer
    current = *taskList;
    while (current != NULL) {
        //store next variable
        temp = current->next;
        // check for higher priority, if higher then set the
        // variables with the current task node and respective priority
        if (current->task->priority <= highestPriority) {
            highestPriority = current->task->priority;
            principalTask = current;
        }

        // Assign temp pointer back to the current pointer
        current = temp;
        // if both current and tasklist are NULL/empty then exit the loop
        if (current == NULL && (*taskList) == NULL) {
            break;
        }

        //otherwise, run the task, delete the task node and restart.
        else if (current == NULL && (*taskList) != NULL) {
            run(principalTask->task, principalTask->task->burst);
            printf("Task %s finished.\n",principalTask->task->name);
            delete(taskList, principalTask->task);
            highestPriority = 11;
            current = *taskList;
        }

    }
    // delete all
    deleteAll(taskList);

}
