#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <pthread.h>

#define MAX_BUFF 1000
#define MAX_NAME_LEN 30


volatile sig_atomic_t flag = 0;
int sockfd = 0 ;
char name[MAX_NAME_LEN]; 

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/*void erase_newline(char *buff){
    int p=0;
    while (p<MAX_BUFF){
        if(buff[p] =='\n' ){
            buff[p]='\0';
            break;
        }
        p++;
    } 
}
*/


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

void signalhandler(){
    flag=1;
}


void recv_msg_handler(){
    char message[MAX_BUFF]={};
    while(1){
       int receive= recv(sockfd,message,MAX_BUFF,0); 
       if(receive >0){
          printf("%s",message);
          str_overwrite_stdout();
       }else if(receive==0){
           break;
       }
       bzero(message,MAX_BUFF);
    }
}

void send_msg_handler(){
    char buffer[MAX_BUFF]={};
    char message[MAX_BUFF + MAX_NAME_LEN]={};
    while(1){
        str_overwrite_stdout();
        fgets(buffer,MAX_BUFF,stdin);
        str_trim_nl(buffer,MAX_BUFF);

        if(strcmp(buffer,"exit")==0){
            break;
        }else{
            sprintf(message,"%s: %s\n",name, buffer);
            send(sockfd,message,strlen(message),0);
        }
        bzero(buffer,MAX_BUFF);
        bzero(message,MAX_BUFF);
    }
    signalhandler(2);
}




int main(int argc, char *argv[])
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char str[INET_ADDRSTRLEN];
    char buffer[MAX_BUFF];
    signal(SIGINT,signalhandler);

    if (argc < 3)
    {
        fprintf(stderr, "<!>Usage %s hostname port\n", argv[0]);
        exit(0);
    }

    printf("Enter your name: ");
    fgets(name,MAX_NAME_LEN,stdin);
    str_trim_nl(name,strlen(name));

    if(strlen(name)>MAX_NAME_LEN -1 || strlen(name) < 2){
       printf("Incorrect Name Provided\n");
       return EXIT_FAILURE; 
    }


    //socket settings

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0){
        error("<!>ERROR opening socket");
    }
    
    server = gethostbyname(argv[1]);
    
    if (server == NULL)
    {
        fprintf(stderr, "<!>ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("<!>ERROR connecting");

    }


    /*printf("<+>Insert The 4-PIN Number for Validation: ");

    
    //send PIN back to server
    //bzero(buffer, 256);
    fgets(buffer,255,stdin);
    //fflush(stdin);

    n=write(sockfd,buffer,MAX_BUFF-1);
    if (n < 0){
        error("<!>ERROR writing to socket");
    } 
    //fflush(stdout);
    bzero(buffer, MAX_BUFF);
    //fflush(stdin);
    //take server response 
    char a;
    n=read(sockfd,&a,1);
    if (n < 0){
        error("<!>ERROR reading from socket");    
    } 

    if(atoi(&a)==0){
        printf("<+>Access Granted\n\n");
    }else{
        printf("<!>Wrong Code, Access Denied from Server\n");
        exit(1);
    }
    */ 

    //send the name to server
    send(sockfd,name,MAX_NAME_LEN,0);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread,NULL,(void*)send_msg_handler,NULL) != 0){
        error("ERROR: pthread\n");
    }

    pthread_t recv_msg_thread;
    if(pthread_create(&send_msg_thread,NULL,(void*)recv_msg_handler,NULL) != 0){
        error("ERROR: pthread\n");
    }


    /*if (inet_ntop(AF_INET, &serv_addr.sin_addr, str,INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "<!>Could not convert byte to address\n");
        exit(1);
    }
    */

    while (1)
    {
        if(flag){
            printf("\nBye\n");
            break;
        }


        /*
        printf("%s_> ",str);
        fflush(stdout);
        
        bzero(buffer, MAX_BUFF);
        fflush(stdin);
        fgets(buffer,MAX_BUFF-1,stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        n = send(sockfd, buffer, MAX_BUFF-1,0);
        if (n < 0){
            error("<!>ERROR writing to socket");
        }
        if (n == 0){
            printf("<!>Connection Lost\n");
            exit(-1);        
        }
        bzero(buffer, MAX_BUFF);
        n = recv(sockfd, buffer, MAX_BUFF-1,0);
        if (n < 0){
            error("<!>ERROR reading from socket");
        }
        if (n == 0){
            printf("<!>Connection Lost\n");
            exit(-1);
        }
        
        if(strstr(buffer, "exit") != NULL){
            printf("<!>Connection to Server Disbanded\n");
            break;
        }
        printf("\n%s\n\n",buffer);
        fflush(stdout);
        */
        
    }
    

    close(sockfd);
    return EXIT_SUCCESS;
}
