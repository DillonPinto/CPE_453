
#ifndef SCHEDULERS_H
#define SCHEDULERS_H
#include "list.h"


#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

    // add a task to the list
    void add(struct node **taskHead,char *name, int priority, int burst);


    // invoke the scheduler
    void schedule();
#endif