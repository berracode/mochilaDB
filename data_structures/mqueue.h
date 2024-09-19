#ifndef MQUEUE_H
#define MQUEUE_H
#include <pthread.h>
#include "../mem/mmem.h"



typedef struct node {
    int data; //TODO: do generic (now is the client fd)
    struct node* next;
} node_t;

typedef struct {
    node_t* front;
    node_t* rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} queue_t;


extern queue_t *request_queue;

queue_t* init_queue();
void enqueue(queue_t* queue, int data);
int dequeue(queue_t* queue);

void destroy_queue(queue_t* queue);



#endif