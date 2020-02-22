#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <pthread.h>
#include "queue.h"

#define MAX_NAME_LEN 30
#define MAX_CLIENTS 15
#define THREAD_POOL_SIZE 10
#define MAX_BUFF 1000



static unsigned int cl_count=0;
static int uid=10;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
queue q;



void str_overwrite_stdout(){
    printf("\r%s","> ");
    fflush(stdout);
}

void str_trim_nl(char * arr, int length){
    for(int i=0;i<length;i++){
        if(arr[i]=='\n'){
            arr[i]='\0';
            break;
        }
    }
}





void * handle_client(void *arg){
    char buffer[MAX_BUFF];
    char name[MAX_NAME_LEN];
    int leave_flag=0;
    cl_count++;

    client_node cli=(client_node)arg;

    //get name
    if(recv(cli->sockfd,name,MAX_NAME_LEN,0)<=0 || strlen(name)<2 || strlen(name)>= MAX_NAME_LEN-1 ){
        printf("Enter the name correctly\n");
        leave_flag=1;
    }else{
        strcpy(cli->name,name);
        sprintf(buffer,"%s has joined\n",cli->name);
        printf("%s",buffer);
        send_msg(q,buffer,cli->uid);
    }
    bzero(buffer,MAX_BUFF);

    while(1){
        if(leave_flag){
            break;
        }

        int receive=recv(cli->sockfd,buffer,MAX_BUFF,0);
        if(receive>0){
            if(strlen(buffer)>0){

                pthread_mutex_lock(&mutex);
                send_msg(q,buffer,cli->uid);
                pthread_mutex_unlock(&mutex);

                str_trim_nl(buffer,strlen(buffer));
                printf("%s\n",buffer);
            }
        }else if(receive ==0||strcmp(buffer,"exit")==0){
            sprintf(buffer,"%s has left\n",cli->name);
            printf("%s\n",buffer);

            pthread_mutex_lock(&mutex);
            send_msg(q,buffer,cli->uid);
            pthread_mutex_unlock(&mutex);

            leave_flag=1;
        }else{
            printf("ERROR: -1\n");
            leave_flag=1;
        }

        bzero(buffer,MAX_BUFF);
    }
    close(cli->sockfd);
    pthread_mutex_lock(&mutex);
    queue_remove(q,cli->uid);
    pthread_mutex_unlock(&mutex);
    cl_count--;
    pthread_detach(pthread_self());

    return NULL;
}



//MAIN
int main(int argc, char *argv[])
{

    if (argc < 2)       
    {
        fprintf(stderr, "(!)No port provided\n");
        exit(1);
    }

    //signals
    signal(SIGPIPE,SIG_IGN);

    //variables
    int sockfd, client_sockfd, portno;
    socklen_t clilen;
    char buffer[MAX_BUFF];
    struct sockaddr_in serv_addr, cli_addr;
    char str[INET_ADDRSTRLEN];
    pthread_t tid;
    q=queue_create();
   

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("(!)ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    printf("(+)Server Up and Running\n");


    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //binding
    if (bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0){
        perror("(!)ERROR on binding");
        exit(EXIT_FAILURE);
    }

    //listen
    if(listen(sockfd, MAX_CLIENTS) == 0){
		printf("(+)Listening....\n");
	}else{
		printf("(!)Error in binding.\n");
	}
    int t_counter=0;

    printf("=== SERVER INITIALIZING CHATROOM ===\n");
    
    while (1)
    {  
        
        clilen = sizeof(cli_addr); 
    
        client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (client_sockfd < 0){
            close(client_sockfd);
            perror("(!)ERROR on accept");
        }

        //Client Settings
        client_node pclient=(client_node)malloc(sizeof(struct _client_node));
        pclient->address=cli_addr;
        pclient->sockfd=client_sockfd;
        pclient->uid=uid++;

        //add Client to Queue
        pthread_mutex_lock(&mutex);
        enqueue(q,pclient);
        pthread_mutex_unlock(&mutex);

        //Create Thread
        pthread_create(&tid,NULL,&handle_client,(void*)pclient);

        //reduce CPU usage
        sleep(1);

    }
    

    close(sockfd);
    return EXIT_SUCCESS;
}
