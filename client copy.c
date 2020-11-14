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
#define MAX_BUFF 10000 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void signalhandler(int signum)
{
    printf("\n<!>Caught signal %d\n<!!>Connection Dissolved\n",signum);
    exit(signum);
}
void erase_newline(char *buff){
    int p=0;
    while (p<MAX_BUFF){
        if(buff[p] =='\n' ){
            buff[p]='\0';
            break;
        }
        p++;
    } 
}



int main(int argc, char *argv[])
{
    int sockfd, portno, n;
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
    printf("<+>Insert The 4-PIN Number for Validation: ");

    
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
    

    if (inet_ntop(AF_INET, &serv_addr.sin_addr, str,INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "<!>Could not convert byte to address\n");
        exit(1);
    }

    while (1)
    {
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
        
    }
    

    close(sockfd);
    return 0;
}
