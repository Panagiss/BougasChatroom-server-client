#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

client_t * find_uid(int uid){

    client_t * result=head;
    while(result!=NULL){
        if(result->uid==uid){
            return result;
        }
        result=result->next;
    }
    return NULL;

}

void send_msg(char *s,int uid){
    if(is_empty!=0){
        return;
    }
    client_t * tmp=head;
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

void queue_remove(int uid){
    if(is_empty!=0){
        return;
    }
    client_t * prev=NULL;
    client_t * cur=head;
    
    while(cur!=NULL){
        if(cur->uid==uid){ 
            //found

            if(prev==NULL){ //first in list
                head=cur->next;
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

