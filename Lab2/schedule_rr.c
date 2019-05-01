#include "schedulers.h"
#include <time.h>
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

void deleteAll(struct node **taskList) {
    struct node *temp, *current = *taskList;

    while (current != NULL) {
        temp = current->next;
        delete(taskList,current->task);
        current = temp;
    }
}

// This function schedules all tasks in a round robin order
// allowing each process to be executed for the same time quantum
// or its respective burst time (if less than time quantum)
void roundRobin(struct node **taskList, struct node *taskNode){
    int runTime = 0;

    // choose runtime based on if burst or quantum is larger
    runTime = (taskNode->task->burst > QUANTUM)? QUANTUM: taskNode->task->burst;
    // run for the specific time slice
    run(taskNode->task, runTime);
    // assign the remaining burst time depending on the execution time of
    // the process
    taskNode->task->burst = (taskNode->task->burst > QUANTUM)?
            taskNode->task->burst - QUANTUM: 0;
    // if process has completed, print success message and delete the task node
    if (taskNode->task->burst == 0){
        printf("Task %s finished.\n",taskNode->task->name);
        delete(taskList, taskNode->task);
    }

}

// This function schedules all the processes passed into it.
// It calls the roundRobin function while cycling through the
// nodes to execute each process.
void schedule(struct node **taskList) {
    struct node *temp, *current = *taskList;

    while (current != NULL) {
        temp = current->next;
        roundRobin(taskList, current); // round robin on the current node
        current = temp;

        // if current and the linked lists (taskList) is empty/NULL, exit the loop
        if (current == NULL && (*taskList) == NULL) {
            break;
        }
        // if only current is null and the linked list isn't empty, reiterate
        else if (current == NULL && (*taskList) != NULL) {
            current = *taskList;
        }
    }
    // delete all tasks
    deleteAll(taskList);

}
