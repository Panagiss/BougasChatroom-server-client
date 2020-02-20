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
struct client{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[30];
    struct client * next;
};
typedef struct client client_t;

void enqueue(client_t * client_socket);

int dequeue(); 

int is_empty();

client_t * find_uid(int );

void send_msg(char*,int );

void queue_remove(int uid);

#endif