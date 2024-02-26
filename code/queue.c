#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "thread_worker_types.h"



struct node {
    struct TCB* data;  
    struct node* next;
};


Queue* init_queue(){
    Queue* queue = (Queue *) malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}


void enqueue(Queue* queue, struct TCB* temp) {
    // node declaration
    struct TCB* newNode = temp;
    newNode->next = NULL;

    if (queue->tail == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}


struct TCB* dequeue(Queue* queue) {
    if (queue->head == NULL) {
        return NULL;
    }

    struct TCB* tempNode = queue->head;
    struct TCB* temp = tempNode;

    queue->head = queue->head->next;

    free(tempNode);

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    return temp;
}


void free_queue(Queue* queue) {
    while (queue->head != NULL) {
        struct TCB* tempNode = queue->head;
        queue->head = queue->head->next;
        free(tempNode);
    }
    free(queue);
}



//for testing only 
struct TCB* create_tcb(int thread_id) {
    struct TCB* tcb = (struct TCB*)malloc(sizeof(struct TCB));
    if (tcb != NULL) {
        tcb->thread_id = thread_id;
        // Initialize other TCB members as needed
    }
    return tcb;
}

//test function

int main(){
    Queue* myQueue = init_queue();

    // Enqueue some elements
    tcb* tcb1 = create_tcb(1);
    tcb* tcb2 = create_tcb(2);
    tcb* tcb3 = create_tcb(3);

    enqueue(myQueue, tcb1);
    enqueue(myQueue, tcb2);
    enqueue(myQueue, tcb3);

    printf("enqueue:");
    struct node* current = myQueue->head;
    while (current != NULL) {
        printf("TCB with thread_id %d\n", current->data->thread_id);
        current = current->next;
    }
    tcb* dequeued1 = dequeue(myQueue);
    tcb* dequeued2 = dequeue(myQueue);
    
    printf("dequeue:");
    current = myQueue->head;
    
    while (current != NULL) {
        printf("TCB with thread_id %d\n", current->data->thread_id);
        current = current->next;
    }

    //free_queue(myQueue);
}
 




