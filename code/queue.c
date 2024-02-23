#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* init_queue(){
    Queue* queue = (Queue *) malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}
void enqueue(Queue* queue, tcb* temp){
    if(queue->tail == NULL || queue->head == NULL){
        queue->head = temp;
        queue->tail = temp;
    } else {
        queue->tail->next = temp;
        queue->tail = temp;
    }
}
tcb* dequeue(Queue* queue){
    if(queue->head == NULL){
        return NULL;
    }
    
    tcb* temp = queue->head;
    queue->head = queue->head->next;


    if(queue->head == NULL){
        queue->tail = NULL;
    }

    // temp->next = NULL; //insure no link to queue is present
    return temp;
}