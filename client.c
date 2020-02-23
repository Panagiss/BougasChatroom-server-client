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
#define MAX_NAME_LEN 31
#define MAX_PASS_LEN 16


volatile sig_atomic_t flag = 0;
int sockfd = 0 ;
char name[MAX_NAME_LEN]; 

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


void str_overwrite_stdout(){
    printf("\n\r%s", "> ");
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

void welcome(){
    char msg[13][120];
    printf("\n");
    strcpy(msg[0],"                                 ____    __    ____  _______  __        ______   ______   .___  ___.  _______ ");
    strcpy(msg[1],"                                 \\   \\  /  \\  /   / |   ____||  |      /      | /  __  \\  |   \\/   | |   ____|");
    strcpy(msg[2],"                                  \\   \\/    \\/   /  |  |__   |  |     |  ,----'|  |  |  | |  \\  /  | |  |__   ");
    strcpy(msg[3],"                                   \\            /   |   __|  |  |     |  |     |  |  |  | |  |\\/|  | |   __|  ");
    strcpy(msg[4],"                                    \\    /\\    /    |  |____ |  `----.|  `----.|  `--'  | |  |  |  | |  |____ ");
    strcpy(msg[5],"                                     \\__/  \\__/     |_______||_______| \\______| \\______/  |__|  |__| |_______|");
    strcpy(msg[6],"                                                         .___________.  ______                                ");
    strcpy(msg[7],"                                                         |           | /  __  \\                               ");
    strcpy(msg[8],"                                                         `---|  |----`|  |  |  |                              ");
    strcpy(msg[9],"                                                             |  |     |  |  |  |                              ");
    strcpy(msg[10],"                                                             |  |     |  `--'  |                              ");
    strcpy(msg[11],"                                                             |__|      \\______/                               ");
    strcpy(msg[12],"                                                                                                              ");
    strcpy(msg[13],"\0");
    for(int i=0;i<13;i++){
        printf("%s\n",msg[i]);
    }
}

void welcome2(){
    char msg[12][150];
    printf("\n");
    strcpy(msg[0],"$$$$$$$\\                                                           $$$$$$\\  $$\\                  $$\\     $$$$$$$\\                                    ");
    strcpy(msg[1],"$$  __$$\\                                                         $$  __$$\\ $$ |                 $$ |    $$  __$$\\                                   ");
    strcpy(msg[2],"$$ |  $$ | $$$$$$\\  $$\\   $$\\  $$$$$$\\   $$$$$$\\   $$$$$$$\\       $$ /  \\__|$$$$$$$\\   $$$$$$\\ $$$$$$\\   $$ |  $$ | $$$$$$\\   $$$$$$\\  $$$$$$\\$$$$\\  ");
    strcpy(msg[3],"$$$$$$$\\ |$$  __$$\\ $$ |  $$ |$$  __$$\\  \\____$$\\ $$  _____|      $$ |      $$  __$$\\  \\____$$\\\\_$$ _|   $$$$$$$  |$$  __$$\\ $$  __$$\\ $$  _$$  _$$\\ ");
    strcpy(msg[4],"$$  __$$\\ $$ /  $$ |$$ |  $$ |$$ /  $$ | $$$$$$$ |\\$$$$$$\\        $$ |      $$ |  $$ | $$$$$$$ | $$ |    $$  __$$< $$ /  $$ |$$ /  $$ |$$ / $$ / $$ |");
    strcpy(msg[5],"$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$  __$$ | \\____$$\\       $$ |  $$\\ $$ |  $$ |$$  __$$ | $$ |$$\\ $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ | $$ | $$ |");
    strcpy(msg[6],"$$$$$$$  |\\$$$$$$  |\\$$$$$$  |\\$$$$$$$ |\\$$$$$$$ |$$$$$$$  |      \\$$$$$$  |$$ |  $$ |\\$$$$$$$ | \\$$$$  |$$ |  $$ |\\$$$$$$  |\\$$$$$$  |$$ | $$ | $$ |");
    strcpy(msg[7],"\\_______/  \\______/  \\______/  \\____$$ | \\_______|\\_______/        \\______/ \\__|  \\__| \\_______|  \\____/ \\__|  \\__| \\______/  \\______/ \\__| \\__| \\__|");
    strcpy(msg[8],"                              $$\\   $$ |                                                                                                             ");
    strcpy(msg[9],"                              \\$$$$$$  |                                                                                                             ");
    strcpy(msg[10],"                               \\______/                                                                                                              ");
    strcpy(msg[11],"\0");
    for(int i=0;i<12;i++){
        printf("%s\n",msg[i]);
    }
}

void welcome3(){
    char msg[9][130];
    printf("\n");
    strcpy(msg[0],"                                       )    )                    (             )      )  (     (        )       ");
    strcpy(msg[1],"                                    ( /( ( /(             (      )\\ )       ( /(   ( /(  )\\ )  )\\ )  ( /(       ");
    strcpy(msg[2],"                                    )\\()))\\())     (      )\\    (()/( (     )\\())  )\\())(()/( (()/(  )\\()) (    ");
    strcpy(msg[3],"                                   ((_)\\((_)\\      )\\  ((((_)(   /(_)))\\   ((_)\\  ((_)\\  /(_)) /(_))((_)\\  )\\   ");
    strcpy(msg[4],"                                  __ ((_) ((_)  _ ((_)  )\\ _ )\\ (_)) ((_)    ((_)  _((_)(_))  (_))   _((_)((_)  ");
    strcpy(msg[5],"                                  \\ \\ / // _ \\ | | | |  (_)_\\(_)| _ \\| __|  / _ \\ | \\| || |   |_ _| | \\| || __| ");
    strcpy(msg[6],"                                   \\ V /| (_) || |_| |   / _ \\  |   /| _|  | (_) || .` || |__  | |  | .` || _|  ");
    strcpy(msg[7],"                                    |_|  \\___/  \\___/   /_/ \\_\\ |_|_\\|___|  \\___/ |_|\\_||____||___| |_|\\_||___| ");
    strcpy(msg[8],"                                                                                                                ");
    strcpy(msg[9],"\0");
    for(int i=0;i<9;i++){
        printf("%s\n",msg[i]);
    }
}


void recv_msg_handler(){
    char message[MAX_BUFF]={};
    while(1){
       int receive= recv(sockfd,message,MAX_BUFF,0); 
       if(receive >0){
            printf("\r->%s \n",message);
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

int sign_up(){
    char key[15];
    //get key and send it to server for validation
    printf("Please,Give the Unique Key of the ChatRoom in order to Join\nIf you don't know the Key, ask the Admin\n");
    if(fgets(key,14,stdin)==NULL){
        fprintf(stderr,"Error Reading Key\n");
        return 0;
    }
    if(send(sockfd,key,14,0)<=0){
        fprintf(stderr,"Error sending Key\n");
        return 0;
    }

    char usrn[MAX_NAME_LEN];
    char passwd[MAX_PASS_LEN];
    printf("Give a Username, it will be used everytime for Signing in\nUsername Cannot have Spaces and Cannot exceed 30 characters\n");
    if(fgets(usrn,MAX_NAME_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Username\n");
        return 0;
    }
    printf("Give a Password, it will be used everytime for Signing in\nPassword Cannot have Spaces and Cannot exceed 15 characters\n");
    if(fgets(passwd,MAX_PASS_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Password\n");
        return 0;
    }
    //sending username and password back to server
    if(send(sockfd,usrn,MAX_NAME_LEN,0)<=0){
        fprintf(stderr,"Error sending Username\n");
        return 0;
    }
    if(send(sockfd,passwd,MAX_PASS_LEN,0)<=0){
        fprintf(stderr,"Error sending Password\n");
        return 0;
    }
    return 1;
    
}

int sign_in(){
    char usrn[MAX_NAME_LEN];
    char passwd[MAX_PASS_LEN];
    printf("Username: ");
    if(fgets(usrn,MAX_NAME_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Username\n");
        return 0;
    }
    printf("\nPassword: ");
    if(fgets(passwd,MAX_PASS_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Password\n");
        return 0;
    }
    //sending username and password back to server
    if(send(sockfd,usrn,MAX_NAME_LEN,0)<=0){
        fprintf(stderr,"Error sending Username\n");
        return 0;
    }
    if(send(sockfd,passwd,MAX_PASS_LEN,0)<=0){
        fprintf(stderr,"Error sending Password\n");
        return 0;
    }
    return 1;
}




int main(int argc, char *argv[])
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char str[INET_ADDRSTRLEN];
    char buffer[MAX_BUFF];
    char choice[3];
    signal(SIGINT,signalhandler);

    if (argc < 3)
    {
        fprintf(stderr, "<!>Usage %s hostname port\n", argv[0]);
        exit(0);
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


    //sign up or sign in 
    printf("\n1.Sign in\n2.Sign up\n");
    fgets(choice,3,stdin);
    if(send(sockfd,choice,1,0)<=0){
        printf("Error Choice\n");
        return EXIT_FAILURE;
    }

    if(atoi(choice)==1){
        printf("Sent Choice 1\n");
        if(sign_in()==0){
            fprintf(stderr,"Error Signing in\n");
            return EXIT_FAILURE;
        }else
        {   char tmp[2];
            recv(sockfd,tmp,1,0);
            if(atoi(tmp)==1){
                printf("Successful Sign in\n");
            }else
            {
                return EXIT_FAILURE;
            }
            
            
        }

    }else if(atoi(choice)==2){
        printf("Sign up Choice 2\n");
        if(sign_up()==0){
            fprintf(stderr,"Error Signing up\n");
            return EXIT_FAILURE;
        }else
        {
            printf("Sign up Complete\n");
        }
        
    }else{
        printf("Error Bad Choice\n");
        return EXIT_FAILURE;
    }

   

    //get name
    printf("\nEnter your name: ");
    fgets(name,MAX_NAME_LEN,stdin);
    str_trim_nl(name,strlen(name));

    if(strlen(name)>MAX_NAME_LEN -1 || strlen(name) < 2){
       printf("Incorrect Name Provided\n");
       return EXIT_FAILURE; 
    }


    //send the name to server
    send(sockfd,name,MAX_NAME_LEN,0);

    welcome();
    welcome2();
    welcome3();

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
