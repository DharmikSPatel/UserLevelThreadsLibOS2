#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "thread_worker_types.h"




Queue* init_queue(){
    Queue* queue = (Queue *) malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}


void enqueue(Queue* queue, tcb* temp) {
    //node declaration
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->data = temp;
    newNode->next = NULL;
    newNode->prev = NULL;
    if(queue->tail == NULL || queue->head == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        newNode->prev = queue->tail;
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}


tcb* dequeue(Queue* queue) {
    if (queue->head == NULL) {
        return NULL;
    }

    struct node* tempNode = queue->head;
    tcb* tempTCB = tempNode->data;

    queue->head = queue->head->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    } 
    else {
        queue->head->prev = NULL;
    }
    free(tempNode);
    return tempTCB;
}
tcb* removeTCB(Queue* queue, tcb* data){
    node* ptr = queue->head;
    while(ptr != NULL && ptr->data->thread_id != data->thread_id){
        ptr = ptr->next;
    }
    return removeNode(queue, ptr);
}
tcb* removeNode(Queue* queue, struct node* n){
    if (queue->head == NULL || n == NULL){
        return NULL;
    }
    node* prev = n->prev;
    node* next = n->next;
    if(prev != NULL){
        prev->next = next;
    }
    else {
        queue->head = next;
    }
    
    if(next != NULL){
        next->prev = prev;
    }
    else {
        queue->tail = prev;
    }
    tcb* data = n->data;
    free(n);
    return data;
}
void printQueue(Queue* queue){
    // node* ptr = queue->head;

    // // printf("HEAD:(%d) :::\n", ptr->data->thread_id);
    // while(ptr != NULL){
    //     if(ptr->data == NULL){
    //         printf("ISSUE\n");
    //     }
    //     printf("Sizeof(%ld)\n", sizeof(*(ptr->data)));
    //     printf("(%d)->\n", ptr->data->thread_id);
    //     ptr = ptr->next;
    // }
    // printf("\n");
}


// void free_queue(Queue* queue) {
//     while (queue->head != NULL) {
//         tcb* tempNode = queue->head;
//         queue->head = queue->head->next;
//         free(tempNode);
//     }
//     free(queue);
// }



// //for testing only 
// tcb* create_tcb(int thread_id) {
//     tcb* tcb = (tcb*)malloc(sizeof(tcb));
//     if (tcb != NULL) {
//         tcb->thread_id = thread_id;
//         // Initialize other TCB members as needed
//     }
//     return tcb;
// }

//test function

// int main(){
//     Queue* myQueue = init_queue();

//     // Enqueue some elements
//     tcb* tcb1 = create_tcb(1);
//     tcb* tcb2 = create_tcb(2);
//     tcb* tcb3 = create_tcb(3);

//     enqueue(myQueue, tcb1);
//     enqueue(myQueue, tcb2);
//     enqueue(myQueue, tcb3);

//     printf("enqueue:");
//     struct node* current = myQueue->head;
//     while (current != NULL) {
//         printf("TCB with thread_id %d\n", current->data->thread_id);
//         current = current->next;
//     }
//     tcb* dequeued1 = dequeue(myQueue);
//     tcb* dequeued2 = dequeue(myQueue);
    
//     printf("dequeue:");
//     current = myQueue->head;
    
//     while (current != NULL) {
//         printf("TCB with thread_id %d\n", current->data->thread_id);
//         current = current->next;
//     }

//     //free_queue(myQueue);
// }