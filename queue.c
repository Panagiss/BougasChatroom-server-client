#include <stdlib.h>
#include <string.h>
#include "queue.h"

client_t * head=NULL;
client_t * tail=NULL;


void enqueue(client_t * cl){

    client_t* newnode=malloc(sizeof(client_t));
    newnode->sockfd=cl->sockfd;
    newnode->uid=cl->uid;
    strcpy(newnode->name,cl->name);
    newnode->next=NULL;
    if(tail==NULL){
        head=newnode;
    }else
    {
        tail->next=newnode;
    }
    tail=newnode;
}

//returns 0 if the queue is empty 
//returns the pointer to a client socket, if there is one to get

int dequeue(){
    if(head==NULL){
        return 0;
    }else
    {
        //client_t * result =head->data;
        client_t *tmp=head;
        head=head->next;
        if(head==NULL){
            tail=NULL;
        }
        free(tmp);
        return 1; 
    }
    
}

int is_empty(){
    if(head==NULL){
        return 1;
    }
    return 0;
}