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
#include <curses.h>


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
           if(strstr(message,"Admin")!=NULL){
                printf("\033[01;33m");
                printf("\r->%s \n",message);
                printf("\033[0m");
                str_overwrite_stdout();
           }else
           {
                printf("\033[0;31m");
                printf("\r->%s \n",message);
                printf("\033[0m");
                str_overwrite_stdout();
           }

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

        if(strcmp(buffer,"exit")==0||strcmp(buffer,"EXIT")==0){
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
    printf("\nPlease,Give the Unique Key of the ChatRoom in order to Join\nIf you don't know the Key, ask the Admin kindly\n");
    if(fgets(key,14,stdin)==NULL){
        printf("Error Reading Key\n");
        return 0;
    }
    if(send(sockfd,key,14,0)<=0){ //sending unique key to server for corfirmation
        printf("Error sending Key\n");
        return 0;
    }
    bzero(key,15);
    if(recv(sockfd,key,1,0)<=0){ //confirmation from server
        printf("Error receiving Key\n");
        return 0;
    }
    if(atoi(key)==1){
        printf("Correct Key, Access Granted\n");
    }else
    {
        printf("Wrong Key, Access Disbanded\n");
        return 0;
    }

    //username
    char usrn[MAX_NAME_LEN];
    printf("\nGive a Username, it will be used everytime for Signing in and that's How others are going to see you\nUsername Cannot exceed 30 characters and Spaces won't count\n");
    while(1){ //while username is not the same with other users
        bzero(usrn,MAX_NAME_LEN);
        if(fgets(usrn,MAX_NAME_LEN-1,stdin)==NULL){
            printf("Error Reading Key\n");
            return 0;
        }

        if(send(sockfd,usrn,MAX_NAME_LEN-1,0)<=0){
            printf("Error sending Username\n");
            return 0;
        }
        bzero(key,15);
        if(recv(sockfd,key,1,0)<=0){ //confirmation from server
            printf("Error receiving Key\n");
            return 0;
        }
        if(atoi(key)==0){
            printf("\nUsername Already Exists,Try another one\n");
        }else
        {
            break;
        }
        
    }
    

    initscr();
    cbreak();
    char passwd[MAX_PASS_LEN];
    //password
    printw("========ENTERED SECURE PASSWORD MODE========\nGive a Password, it will be used everytime for Signing in\nPassword Cannot exceed 15 characters and Spaces won't count\n");
    
	noecho();

    int p=0; 
    do{ 
        passwd[p]=getch(); 
        if(passwd[p]!='\n'){ 
            printw("*");
            refresh(); 
        } 
        p++; 
    }while(passwd[p-1]!='\n' && p<MAX_PASS_LEN); 
    passwd[p-1]='\0'; 

    clear();
    endwin();
    
    
    /*if(fgets(passwd,MAX_PASS_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Password\n");
        return 0;
    }*/
   
    
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

    /*printf("\nPassword: ");
    if(fgets(passwd,MAX_PASS_LEN-1,stdin)==NULL){
        fprintf(stderr,"Error Password\n");
        return 0;
    }*/

    //start ncurses lib
    initscr();
    cbreak();
    
    printw("========ENTERED SECURE PASSWORD MODE========\nPassword: ");
	noecho();

    int p=0; 
    do{ 
        passwd[p]=getch(); 
        if(passwd[p]!='\n'){ 
            printw("*");
            refresh(); 
        } 
        p++; 
    }while(passwd[p-1]!='\n' && p<MAX_PASS_LEN); 
    passwd[p-1]='\0'; 
    clear();
    endwin(); 
    //end ncurses lib

    //sending username and password back to server
    //printf("DEBUG:%s\n",usrn);
    if(send(sockfd,usrn,MAX_NAME_LEN-1,0)<=0){
        fprintf(stderr,"Error sending Username\n");
        return 0;
    }
    //printf("DEBUG:%s\n",passwd);
    if(send(sockfd,passwd,MAX_PASS_LEN-1,0)<=0){
        fprintf(stderr,"Error sending Password\n");
        return 0;
    }
    strcpy(name,usrn);
    str_trim_nl(name,strlen(name));
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

    if (argc < 2)
    {
        fprintf(stderr, "<!>Usage %s port\n", argv[0]);
        exit(0);
    }

    //socket settings

    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0){
        error("<!>ERROR opening socket");
    }
    
    server = gethostbyname("127.0.0.1");
    
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


    printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n* In Order to Join our CHATROOM Sign in or if you are New User Sign up      *\n* Please take in mind that this not final product and is subject to change  *\n* If you find a bug contact the admin                                       *\n* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
    //sign up or sign in 
    printf("\n1.Sign in\n2.Sign up\n");
    fgets(choice,3,stdin);
    if(send(sockfd,choice,1,0)<=0){
        printf("\nError Choice\n");
        return EXIT_FAILURE;
    }

    // sign in
    if(atoi(choice)==1){
    
        if(sign_in()==0){     //error from the local sign in function
            fprintf(stderr,"\nError Signing in\n");
            return EXIT_FAILURE;
        }else
        {   char tmp[2];
            recv(sockfd,tmp,1,0);
            if(atoi(tmp)==1){ //correct sign in, confirmed also from the server
                printf("\nSuccessful Sign in\n");
            }else
            {                 //error found from server side
                printf("\nUnsuccessful Sign in\n");
                return EXIT_FAILURE;
            }
            
            
        }

    //sign up
    }else if(atoi(choice)==2){
       
        if(sign_up()==0){     //error from the local sign up function
            fprintf(stderr,"Error Signing up\n");
            return EXIT_FAILURE;
        }else
        {   char tmp[2];
            recv(sockfd,tmp,1,0);
            if(atoi(tmp)==1){ //correct sign up, confirmed also from the server
                system("clear"); //clear terminal
                printf("Sign Up Completed Successfully\nNow Sign in with your Username and Password\n\n");

                //new user is going to sign in after successful sign up

                if(sign_in()==0){     //error from the local sign in function
                    fprintf(stderr,"Error Signing in\n");
                    return EXIT_FAILURE;
                }else
                {   char tmp[2];
                    recv(sockfd,tmp,1,0);
                    if(atoi(tmp)==1){ //correct sign in, confirmed also from the server
                        printf("\nSuccessful Sign in\n");
                    }else
                    {                 //error found from server side
                        printf("\nUnsuccessful Sign in\n");
                        return EXIT_FAILURE;
                    }
                }


            }else
            {                 //error found from server side
                printf("\nUnsuccessful Sign Up\n");
                return EXIT_FAILURE;
            }
        }
        

    //false option
    }else{
        printf("\nError Bad Choice\n");
        return EXIT_FAILURE;
    }

   

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
