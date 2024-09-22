#include <stdio.h>
#include "mqueue.h"

queue_t *request_queue;

queue_t* init_queue() {
    queue_t *queue = m_malloc(sizeof(queue_t));
    queue->front = queue->rear = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void enqueue(queue_t* queue, int data) {
    safe_printf("Encolando: %d\n", data);
    node_t* new_node = (node_t*)m_malloc(sizeof(node_t));
    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    pthread_cond_signal(&queue->cond); //notify
    pthread_mutex_unlock(&queue->mutex);
}

int dequeue(queue_t* queue) {
    safe_printf("Hilo esperando2 %ld\n", pthread_self());

    pthread_mutex_lock(&queue->mutex);
    safe_printf("Hilo captura mutex %ld\n", pthread_self());

    safe_printf("condicion while queue->front == NULL [%d] && keep_running [%d]\n", queue->front == NULL, keep_running);
    while (queue->front == NULL && keep_running) {
        safe_printf("Hilo ANTES WAIT mutex %ld\n", pthread_self());

        pthread_cond_wait(&queue->cond, &queue->mutex);
        safe_printf("Hilo DESPUES WAIT mutex %ld\n", pthread_self());

    }
    int data = NULL;
    if(queue!=NULL && queue->front!=NULL){
        node_t* temp = queue->front;
        data = temp->data;

        queue->front = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = NULL;
        }

        m_free(temp);
    }

    pthread_mutex_unlock(&queue->mutex);
    return data;
}

void destroy_queue(queue_t* queue) {
    pthread_mutex_lock(&queue->mutex);
    pthread_cond_broadcast(&queue->cond);
    safe_printf("Destroying queue...\n");

    node_t* current = queue->front;
    while (current != NULL) {
        node_t* temp = current;
        current = current->next;
        m_free(temp);
    }

    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);

    pthread_cond_destroy(&queue->cond);
    safe_printf("3 Queue destroyed successful!\n");

}
