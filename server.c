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
#include <ctype.h>
#include "queue.h"

#define MAX_NAME_LEN 31
#define MAX_PASS_LEN 16
#define MAX_CLIENTS 15
#define THREAD_POOL_SIZE 10
#define MAX_BUFF 1000



static unsigned int cl_count=0;
static int uid=10;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
queue q;


void remove_spaces(const char *input, char *result)
{
  int i, j = 0;
  for (i = 0; input[i] != '\0'; i++) {
    if (!isspace((unsigned char) input[i])) {
      result[j++] = input[i];
    }
  }
  result[j] = '\0';
}


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

int sign_up(client_node cli){
    char key[15],usrn[MAX_NAME_LEN],passwd[MAX_PASS_LEN];
    char n_usrn[MAX_NAME_LEN],n_passwd[MAX_PASS_LEN];
    if(recv(cli->sockfd,key,20,0)<=0){
        printf("Error Key Received\n");
        return 0;
    }
    str_trim_nl(key,15);
    if(strcmp(key,"planhtarxhs")!=0){
        printf("Access Denied\nWrong Key\n");
        return 0;
    }
    FILE *fp=fopen("./.t_server_usrs","a");
    if(fp==NULL){
        printf("ERROR opening File\n");
        return 0;
    }
    printf("Access Granted\n");
    //get new username and password from client
    if(recv(cli->sockfd,usrn,MAX_NAME_LEN,0)<=0){
        printf("ERROR Receiving username\n");
        return 0;
    }
    if(recv(cli->sockfd,passwd,MAX_PASS_LEN,0)<=0){
        printf("ERROR Receiving password\n");
        return 0;
    }
    //write to FILE
    str_trim_nl(usrn,MAX_NAME_LEN);
    remove_spaces(usrn,n_usrn);
    remove_spaces(passwd,n_passwd);
   
    if(fprintf(fp,"%s:%s\n",n_usrn,n_passwd)<=0){
        printf("ERROR Writting to file\n");
        return 0;
    }
    fclose(fp);
    return 1;
}

int sign_in(client_node cli){
    char usrn[MAX_NAME_LEN],passwd[MAX_PASS_LEN];
    char line[MAX_NAME_LEN+MAX_PASS_LEN];
    FILE *fp;
    char delim[]=":";
    int info[2]={0};

    //get username and password from client
    if(recv(cli->sockfd,usrn,MAX_NAME_LEN,0)<=0){
        printf("ERROR Receiving username\n");
        return 0;
    }
    if(recv(cli->sockfd,passwd,MAX_PASS_LEN,0)<=0){
        printf("ERROR Receiving password\n");
        return 0;
    }
    if ((fp = fopen("./.t_server_usrs", "r")) == NULL) {
        printf("Error! opening file");
        return 0;
    }

    // reads text until newline is encountered
    while(fgets(line, MAX_NAME_LEN+MAX_PASS_LEN, fp)) {
        printf("%s\n", line);
        str_trim_nl(line,MAX_NAME_LEN+MAX_PASS_LEN);
        char *ptr = strtok(line, delim);
        
	    while(ptr != NULL)
	    {   
            if(strcmp(usrn,ptr)==0){
                info[0]=1;
            }
            if(strcmp(passwd,ptr)==0){
                info[1]=1;
            }
            printf("'%s'\n", ptr);
            ptr = strtok(NULL, delim);
	    }
    }
    if(info[0]!=1 || info[1]!=1){
        return 0;
    }

    return 1;
    fclose(fp);


}




void * handle_client(void *arg){
    char buffer[MAX_BUFF];
    char name[MAX_NAME_LEN];
    int leave_flag=0;
    cl_count++;
    char choice[3];

    client_node cli=(client_node)arg;

    //sign in or sign up
    if(recv(cli->sockfd,choice,1,0)<=0){
        printf("Error Choice\n");
        leave_flag=1;
    }else
    {
        if(atoi(choice)==1){
            printf("Sign in S\n");
            if(sign_in(cli)==0){
                printf("Error Signing in\n");
                leave_flag=1;
            }else
            {
                send(cli->sockfd,"1",1,0);
                printf("Successful Sign in\n");
            }
            

        }else if(atoi(choice)==2){
            printf("Sign up S\n");
            if(sign_up(cli)==0){
                printf("Error Sign up\n");
                leave_flag=1;
            }else
            {
                printf("Sign up complete\n");
            }
            

        }else{
            printf("Error Bad Choice\n");
            leave_flag=1;
        }

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

    }
    
    
    //chatting
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
        }else if(receive ==0||strcmp(buffer,"exit")||strcmp(buffer,"EXIT")==0){
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
