#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct _queue* queue;
typedef struct _client_node* client_node;

//client structure 
struct _client_node{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[30];
    struct _client_node * next;
};

struct _queue{
    struct _client_node * head;
    struct _client_node * tail;
    int size;
};

queue queue_create();

void enqueue(queue, client_node);

void queue_destroy(queue);

void dequeue(queue); 

int queue_is_empty(queue);


void send_msg(queue,char*,int);

void queue_remove(queue,int);

#endif