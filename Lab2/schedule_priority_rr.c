#include "schedulers.h"
#include <string.h>
#include "task.h"
#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

// Create a new task structure with the given name, priority and burst
// add a task to the list at the head, as shown in example run in lab instructions
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
void roundRobin(struct node **taskList, struct node *taskNode){
    int runTime = 0;

    runTime = (taskNode->task->burst > QUANTUM)? QUANTUM: taskNode->task->burst;

    run(taskNode->task, runTime);

    taskNode->task->burst = (taskNode->task->burst > QUANTUM)?
                            taskNode->task->burst - QUANTUM: 0;

    if (taskNode->task->burst == 0){
        printf("Task %s finished.\n",taskNode->task->name);
        delete(taskList, taskNode->task);
    }

}

// Create new list ordered by priority from the original list
// (taskList).
void copyList(struct node **taskList, struct node **newList) {
    struct node *temp, *current = NULL, *principalTask = NULL;
    int highestPriority = 11;
    Task * copy  = NULL;
    char *cpyStr = NULL;

    current = *taskList;
    while (current != NULL) {
        temp = current->next;

        if ((current->task->priority < highestPriority)) {
            highestPriority = current->task->priority;
            principalTask = current;
        }

        current = temp;

        if (current == NULL && (*taskList) == NULL) {
            break;
        }
        else if (current == NULL && (*taskList) != NULL) {

            copy = principalTask->task;
            cpyStr = malloc(sizeof(copy->name));
            strcpy(cpyStr, copy->name);
            add(newList, cpyStr, copy->priority, copy->burst);
            delete (taskList, principalTask->task);
            highestPriority = 11;
            current = *taskList;

        }

    }
}
// Schedule processes in a round robin scheduling process in sorder of priority
// where the higher the priority, the sooner it executes in the queue.
// This is the same as the round robin scheduler, except the processes are pre-ordered
// by priority.
void schedule(struct node **taskList) {

    struct node *temp, *current = NULL;
    struct  node * newList = NULL;

    // create a sorted new linked list (newList), ordered by priority
    copyList(taskList, &newList);
    current = newList;


    // iterate through linked list
    while (current != NULL) {
        temp = current->next;
        // Execute round robin scheduling on current node
        roundRobin(&newList, current);

        current = temp;
        // if current and the linked lists (newList) is empty/NULL, exit the loop
        if (current == NULL && (newList) == NULL) {
            break;
        }
        // if only current is null and the linked list isn't empty, reiterate
        else if (current == NULL && (newList) != NULL) {
            current = newList;
        }

    }

    // delete all nodes in the list
    deleteAll(&newList);

}
