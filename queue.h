#ifndef _QUEUE_H
#define _QUEUE_H

/*typedef int* item_type;

struct node {

    item_type data;
    struct node* next;

};
typedef struct node node_t;
*/

//client structure 
typedef struct{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[30];
    struct client_t * next;
} client_t;

void enqueue(client_t * client_socket);
int dequeue(); 
int is_empty();

#endif