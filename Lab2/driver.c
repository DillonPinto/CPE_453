/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;
    struct node *taskHead = NULL;

    in = fopen(argv[1],"r"); // TODO ERROR CHECKING FOR MISSING ARG (
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);//make a copy of the string
        name = strsep(&temp,","); //extract string before first comma
        priority = atoi(strsep(&temp,",")); // extract string before second comma, convert to int
        burst = atoi(strsep(&temp,","));// extract string before third comma, convert to int

        // add the task to the scheduler's list of tasks
        add(&taskHead,name,priority,burst);

        free(temp);
    }

    fclose(in);

    // invoke the scheduler. The linked in is passed to the function via
    // a pointer to the head node.
    schedule(&taskHead);

    return 0;
}
