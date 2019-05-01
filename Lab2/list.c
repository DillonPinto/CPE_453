/**
 * Various list operations
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"


// add a new task to the list of tasks, at the head of the list
// the new node is dynamically allocated as the total number if
// processes is not known.
void insertHead(struct node **head, Task *newTask) {

struct node * newNode = malloc(sizeof(struct node));

    if(newNode == NULL){
        fprintf(stderr,"malloc failure attempting to add %s\n",newTask->name);
        exit(EXIT_FAILURE);
    }

    newNode->task = newTask;
    newNode->next  = *head;
    *head = newNode;

}


// add a new task to the list of tasks, at the tail of the list
// the new node is dynamically allocated as the total number if
// processes is not known. This is used to form a queue structure (FIFO)
void insertTail(struct node **head, Task *newTask) {

    struct node * newNode,*listPtr=NULL;

    newNode=malloc(sizeof(struct node));

    if(newNode == NULL){
        fprintf(stderr,"malloc failure attempting to add %s\n",newTask->name);
        exit(EXIT_FAILURE);
    }

    newNode->next=NULL;
    newNode->task=newTask;

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    for(listPtr = *head; listPtr->next!=NULL;listPtr=listPtr->next);

    listPtr->next=newNode;

}


/* TODO delete this
void deleteNode(struct node *lPtr) {
    struct node *tempPtr=NULL;

    if (lPtr == NULL){
        return;
    }

    tempPtr=lPtr->next->next;
    free(lPtr->next->task->name);
    free(lPtr->next);
    lPtr->next=tempPtr;

}

*/


// delete the selected task from the list.
// NOTE: Changes made - added free() calls to delete
// dynamically allocated structures
void delete(struct node **head, Task *task) {

    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (strcmp(task->name, temp->task->name) == 0) {
        *head = (*head)->next;
        free(temp->task->name);
        free(temp->task);
        free(temp);
    }
    else {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;

        while (strcmp(task->name,temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }
        prev->next = temp->next;
        free(temp->task->name);
        free(temp->task);
        free(temp);
    }

}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}
