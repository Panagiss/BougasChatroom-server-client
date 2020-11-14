#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include "queue.h"

queue queue_create() {

    queue q = (queue) malloc(sizeof(struct _queue));
    if (q == NULL) {
        fprintf(stderr, "Insufficient memory to \
        initialize queue.\n");
        abort();
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    
    return q;
}


void enqueue(queue q,client_node cl){

    //client_t* newnode=(client_t*)malloc(sizeof(client_t));
    //newnode->sockfd=cl->sockfd;
    //newnode->uid=cl->uid;
    //strcpy(newnode->name,cl->name);
    //newnode->next=NULL;
    cl->next=NULL;
    if(q->head==NULL){
        q->head=cl;
    }else
    {
        q->tail->next=cl;
    }
    q->tail=cl;
    q->size++;
    
}

//returns 0 if the queue is empty 
//returns the pointer to a client socket, if there is one to get

void dequeue(queue q){
    
    if (queue_is_empty(q)) {
        fprintf(stderr, "Can't pop element from queue: \
        queue is empty.\n");
        abort();
    }
    
    client_node tmp=q->head;
    if (q->head == q->tail) {
        q->head = NULL;
        q->tail = NULL;
    }else {
        q->head = q->head->next;
    }

    q->size--;
    free(tmp);
    return ;   
}

int queue_is_empty(queue q) {
    
    if (q==NULL) {
        fprintf(stderr, "Cannot work with NULL queue.\n");
        abort();
    }
    return q->head == NULL;
}


void send_msg(queue q,char *s,int uid){
    if(queue_is_empty(q)) {
        fprintf(stderr, "Can't send message: \
        queue is empty.\n");
        abort();
    }

    client_node tmp=q->head;
    while(tmp!=NULL){
        if(tmp->uid!=uid){
            if(write(tmp->sockfd,s,strlen(s))<0){
                printf("ERROR: write to descriptor failed\n");
                return;
            }
        }
        tmp=tmp->next;
    }
}

void queue_remove(queue q,int uid){
    if (queue_is_empty(q)) {
        fprintf(stderr, "Can't pop element from queue: \
        queue is empty.\n");
        abort();
    }


    client_node prev=NULL;
    client_node cur=q->head;
    
    while(cur!=NULL){
        if(cur->uid==uid){ 
            //found

            if(prev==NULL){ //first in list
                q->head=cur->next;
            }else{          //not first in list
                prev->next=cur->next;
            }
            free(cur);
            return;
        }
        prev=cur;
        cur=cur->next;
    }
}

