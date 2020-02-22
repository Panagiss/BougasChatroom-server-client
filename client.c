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


void str_overwrite_stdout(){
    printf("\r%s", "> ");
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

/*void welcome(){
    char msg[12][150];
    strcpy(msg[0],"$$$$$$$\                                                           $$$$$$\  $$\                  $$\     $$$$$$$\                                    ");
    strcpy(msg[1],"$$  __$$\                                                         $$  __$$\ $$ |                 $$ |    $$  __$$\                                   ");
    strcpy(msg[2],"$$ |  $$ | $$$$$$\  $$\   $$\  $$$$$$\   $$$$$$\   $$$$$$$\       $$ /  \__|$$$$$$$\   $$$$$$\ $$$$$$\   $$ |  $$ | $$$$$$\   $$$$$$\  $$$$$$\$$$$\  ");
    strcpy(msg[3],"$$$$$$$\ |$$  __$$\ $$ |  $$ |$$  __$$\  \____$$\ $$  _____|      $$ |      $$  __$$\  \____$$\\_$$  _|  $$$$$$$  |$$  __$$\ $$  __$$\ $$  _$$  _$$\ ");
    strcpy(msg[4],"$$  __$$\ $$ /  $$ |$$ |  $$ |$$ /  $$ | $$$$$$$ |\$$$$$$\        $$ |      $$ |  $$ | $$$$$$$ | $$ |    $$  __$$< $$ /  $$ |$$ /  $$ |$$ / $$ / $$ |");
    strcpy(msg[5],"$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$  __$$ | \____$$\       $$ |  $$\ $$ |  $$ |$$  __$$ | $$ |$$\ $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ | $$ | $$ |");
    strcpy(msg[6],"$$$$$$$  |\$$$$$$  |\$$$$$$  |\$$$$$$$ |\$$$$$$$ |$$$$$$$  |      \$$$$$$  |$$ |  $$ |\$$$$$$$ | \$$$$  |$$ |  $$ |\$$$$$$  |\$$$$$$  |$$ | $$ | $$ |");
    strcpy(msg[7],"\_______/  \______/  \______/  \____$$ | \_______|\_______/        \______/ \__|  \__| \_______|  \____/ \__|  \__| \______/  \______/ \__| \__| \__|");
    strcpy(msg[8],"                              $$\   $$ |                                                                                                             ");
    strcpy(msg[9],"                              \$$$$$$  |                                                                                                             ");
    strcpy(msg[10],"                               \______/                                                                                                              ");
    strcpy(msg[11],"\0");
    for(int i=0;i<12;i++){
        printf("\n%s\n",msg[i]);
    }

}
*/



void recv_msg_handler(){
    char message[MAX_BUFF]={};
    while(1){
       int receive= recv(sockfd,message,MAX_BUFF,0); 
       if(receive >0){
            printf("%*s <\n",78,message);
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
        bzero(message,MAX_BUFF+MAX_NAME_LEN);
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


    //send the name to server
    send(sockfd,name,MAX_NAME_LEN,0);

    printf("=== WELCOME TO THE CHATROOM ===\n");
    //welcome();

    pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread,NULL,(void*)send_msg_handler,NULL) != 0){
        error("ERROR: pthread\n");
    }

    pthread_t recv_msg_thread;
    if(pthread_create(&recv_msg_thread,NULL,(void*)recv_msg_handler,NULL) != 0){
        error("ERROR: pthread\n");
    }



    while (1)
    {
        if(flag){
            printf("\nBye\n");
            break;
        }

    }
    

    close(sockfd);
    return EXIT_SUCCESS;
}
