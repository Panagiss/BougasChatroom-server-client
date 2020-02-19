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

#define MAX 9999
#define MIN 1000
#define MAXCON 5
#define MAX_BUFF 10000
#define READ 0
#define WRITE 1

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
void signalhandler(int signum)
{
    printf("\n\n(!)Caught signal %d\n(!!)Server Has Been Terminated\n",signum);
    
    //clear tmp and history files
    char path[70];
    char path2[70];
    strcat(path,"rm ");
    strcat(path,getenv("HOME"));
    strcat(path,"/");
    strcat(path,"tmp.os2file");
    strcat(path," 2> /dev/null ");
    system(path);
   
    strcat(path2,"rm ");
    strcat(path2,getenv("HOME"));
    strcat(path2,"/");
    strcat(path2,"*.hs");
    strcat(path2," 2> /dev/null ");
    system(path2);

    exit(signum);
}
void signalhandler2(int signum)
{
    printf("\n(!)Open Process Has Been Terminated\n");
    exit(signum);
}

void check_child_exit(int status){
    if (WIFEXITED(status))
        printf("\n(+)Child ended normally. Exit code is %d\n\n",WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("\n(?)Child ended because of an uncaught signal, signal = %d\n\n",WTERMSIG(status));
    else if (WIFSTOPPED(status))
        printf("\n(!)Child process has stopped, signal code = %d\n\n",WSTOPSIG(status));
}
int parse_line(char *line,char **words){

    if(line==NULL||line=="\n"){
        return 0;
    }

   int wordc=0;
   
   /* get the first token */
   char *word = strtok(line, " ");
   
   /* walk through other tokens */
    while( word != NULL ) {
        words[wordc]=(char*)malloc(50*sizeof(char));
        strcpy(words[wordc++],word );
        word = strtok(NULL, " ");
    }
    words[wordc]=NULL;
    //words[wordc]=(char*)malloc(strlen(word)*sizeof(char));
    //strcpy(words[wordc++],word );

    return wordc;
}
void free_words(int wordc,char **words){
    int h;
    for(h=0;h<wordc;h++){
        free(words[h]);
    }

}
int parse_pipe(char* *words,int *pipe_index){

    int pipec=0,c=0,c2=0,c3=0,c4=0;
    while(words[c]!=NULL){
        if(strcmp(words[c],"|")==0){
            pipec++;
            pipe_index[c2++]=c;
            words[c]=NULL;
        }
        c++;
    }
   return pipec;
}
void pipe_work(char **words,char**cmd1,char**cmd2){

    int position=0,position2=0;
    while(words[position]!=NULL){
        cmd1[position]=(char*)malloc(10*sizeof(char));
        strcpy(cmd1[position],words[position]);
        position++;
    }
    cmd1[position++]=NULL;
    while(words[position]!=NULL){
        cmd2[position2]=(char*)malloc(10*sizeof(char));
        strcpy(cmd2[position2],words[position]);
        position++;
        position2++;
    }
    cmd2[position2++]=NULL;


}


void runpipe(int pfd[],char* *cmd1,char* *cmd2,FILE *fd){
    int pid;
    switch (pid = fork())
    {

        case 0: /* child */
            dup2(pfd[READ], 0);
            close(pfd[WRITE]);
            /* the child does not need this end of the pipe */
            execvp(cmd2[0], cmd2);
            perror(cmd2[0]);

        default: /* parent */
            dup2(pfd[WRITE], 1);
            close(pfd[READ]);
            /* the parent does not need this end of the pipe */
            execvp(cmd1[0], cmd1);
            perror(cmd1[0]);

        case -1:
            perror("fork");
            exit(1);
    }
}
int redirect_out(char**words,char* outputf){

    int c=0;
    while(words[c]!=NULL){

        if(strcmp(words[c],">")==0){
            words[c]=NULL;
            if(words[++c]==NULL){
                return -1;
            }
            strcpy(outputf,words[c]);
            return 0;
        }
        c++;
    }
    return -1;
}
int redirection(char**words){
    int c=0;

    while(words[c]!=NULL){

        if(strcmp(words[c],">")==0){
           return 1; 
        }
        if(strcmp(words[c],">")==0){
           return 0; 
        }
        c++;
    }
    return -1;

}




//MAIN
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[MAX_BUFF];
    struct sockaddr_in serv_addr, cli_addr;
    int n,wordc;
    char str[INET_ADDRSTRLEN];
    int pipe_index[20]={ [0 ... 19 ] = -1 };
    signal(SIGINT,signalhandler);

    char **words = (char **)malloc(20 * sizeof(char *));
    char **cmd1=(char **)malloc(10 * sizeof(char *));
    char **cmd2=(char **)malloc(10 * sizeof(char *));
    //char **cmd3=(char **)malloc(10 * sizeof(char *));
    //char **cmd4=(char **)malloc(10 * sizeof(char *));
    //char **cmd5=(char **)malloc(10 * sizeof(char *));




    if (argc < 2)       
    {
        fprintf(stderr, "(!)No port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error_exit("(!)ERROR opening socket");
    }
    printf("(+)Server Up and Running\n");


   /* bzero((char *) &serv_addr, sizeof(serv_addr)); */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0){
        error_exit("(!)ERROR on binding");
    }

    //listen
    if(listen(sockfd, MAXCON) == 0){
		printf("(+)Listening....\n");
	}else{
		printf("(!)Error in binding.\n");
	}

    
    while (1)
    {  
        
        clilen = sizeof(cli_addr); 
    
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            close(newsockfd);
            perror("(!)ERROR on accept");
        }
        pid_t child_pid;
        int status;
        //fork
        if( (  child_pid=fork() ) ==-1 ) { //check for error
            perror("fork");
            exit(1);
        }
            
        if(child_pid==0){ //main child process serving the client
            signal(SIGINT,signalhandler2);
            close(sockfd);
            //generate random 4 pin number
            srand(time(0));
            //Validation Process
            int pin= (rand() % (MAX - MIN + 1)) + MIN ;
            printf("\n\n(+)New Client tries to Establish Connection..\n(+)Waiting Validation from Client\n(->)PIN: %d\n",pin);
            
            bzero(buffer, MAX_BUFF);
            
            n = read(newsockfd, buffer, MAX_BUFF-1);
            if (n < 0) perror("(!)Invalid Input from Client\nAccess Denied\n");
            if(atoi(buffer)!=pin){
                printf("(!)Wrong Code, Access Denied to Client\n");
                char a;
                a='1';
                write(newsockfd,&a,1);
                close(newsockfd);
                printf("(-)Close status is :%d for process:%d \n",close(newsockfd), getpid());
                exit(EXIT_FAILURE);
            }else{
                char a;
                a='0';
                printf("(+)Connection Established\n");
                write(newsockfd,&a,1);
            }
            if (inet_ntop(AF_INET, &cli_addr.sin_addr, str,INET_ADDRSTRLEN) == NULL) {
                fprintf(stderr, "(!)Could not convert byte to address\n");
            }
            
            char mypid[6];
            char hs_path[70];
            sprintf(mypid, "%d",getpid());
            strcat(hs_path,getenv("HOME"));
            strcat(hs_path,"/");
            strcat(hs_path,mypid);
            strcat(hs_path,".hs");
            FILE *history=fopen(hs_path,"w+");
            int tmp=dup(1);
            dup2(fileno(history),1);
            printf("History:\n");
            dup2(tmp,1);

            char tmp_path[70];
            strcat(tmp_path,getenv("HOME"));
            strcat(tmp_path,"/");
            strcat(tmp_path,"tmp.os2file");


            fprintf(stdout, "\n(+)The client address is :%s\n\n", str);
            //fflush(stdout);

            while (1)
            {
                FILE *tmpfile=fopen(tmp_path,"w+");
                
                bzero(buffer, MAX_BUFF);
                fflush(stdout);
                n = recv(newsockfd, buffer, MAX_BUFF-1,MSG_WAITALL);
                if (n < 0){
                    perror("(!)ERROR reading from socket");
                    break;
                }
                if (n == 0){
                    printf("\n(!)Client Connection Has Been Disbanded\n");
                    break;
                }
                int tmp=dup(1);
                dup2(fileno(history),1);
                printf("%s\n",buffer);
                dup2(tmp,1);
                printf("%s\n",buffer);

                if(strstr(buffer, "exit") != NULL) { //check for exit
                    n = send(newsockfd, "exit", 4,0);
                    if (n < 0){
                        perror("(!)ERROR writing to socket"); 
                    }
                    printf("\n(!)Disconnected from Client:%s PortNo:%d\n", str, portno);
                    remove(hs_path);                    
                    break;
                }
                if(strstr(buffer, "history") != NULL) { //check for history

                    bzero(buffer,MAX_BUFF);       
                    fseek(history,0,SEEK_SET);

                    n = read(fileno(history), buffer, MAX_BUFF-1);
                    if (n < 0){
                        perror("(!)ERROR reading from File");
                        break;
                    }    

                    n = send(newsockfd,buffer, MAX_BUFF-1,0);
                    if (n < 0){
                        perror("(!)ERROR writing to socket");
                        break;
                    }
                    continue;
                }

                wordc=parse_line(buffer,words);
                if(wordc==0)
                {
                    int tmp=dup(1);
                    dup2(newsockfd,1);
                    printf("($)Please Give a Command\n");
                    dup2(tmp,1);
                    continue;
                } 
                if(strstr(buffer,"cd") != NULL){
                    //char result[1000]={0};
                    char cwd[800];
                    if(wordc == 2){
                        bzero(buffer, sizeof(buffer));
                        if(chdir(words[1])){
                            char line[]= "(!)cannot go to ";
                            snprintf(buffer, sizeof(buffer), "%s %s", line,words[1]);
                            send(newsockfd, buffer,MAX_BUFF-1,0);
                           

                        }else{
                            getcwd(cwd, sizeof(cwd));
                            char line[]="(+)Directory changed to ";
                            snprintf(buffer, sizeof(buffer), "%s %s", line,cwd);
                            send(newsockfd, buffer,MAX_BUFF-1,0);
                        }

                    }else if(wordc == 1){
                        bzero(buffer, sizeof(buffer));
                        if(chdir(getenv("HOME"))){
                            char line[]="(!)ERROR(!) going to Home Directory\n";
                            send(newsockfd, line,sizeof(line),0);

                        }else{
                            getcwd(cwd, sizeof(cwd));
                            char line[]="(+)Directory changed to ";
                            snprintf(buffer, sizeof(buffer), "%s %s", line,cwd);
                            send(newsockfd, buffer,MAX_BUFF-1,0);
                        }

                    }else{
                            char line[]="(!)ERROR(!) Correct Form: cd[dir_name]\n";
                            send(newsockfd, line,sizeof(line),0);
                        }

                    bzero(buffer, sizeof(buffer));
                    continue;
                }

            
                //fork
                int cc_pid,status2;
                if ((cc_pid=fork())== -1) {// check for error
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                if (cc_pid!= 0){// The parent(child) process
                    
                    printf("(=>)I am parent process %d, Son of Server, Serving Client:%s\n\n",getpid(),str);
                    fflush(stdout);
                    //close(newsockfd);

                    if (wait(&status2)== -1){ // Wait for child
                        perror("wait");
                    }
                    free_words(wordc,words);
                    check_child_exit(status2);


                    bzero(buffer,MAX_BUFF);       
                    fseek(tmpfile,0,SEEK_SET);

                    n = read(fileno(tmpfile), buffer, MAX_BUFF-1);
                    if (n < 0){
                        perror("(!)ERROR reading from File");
                        break;
                    }    

                    n = send(newsockfd,buffer, MAX_BUFF-1,0);
                    if (n < 0){
                        perror("(!)ERROR writing to socket");
                        break;
                    }
                    
                    
                }
                else     // The child(child of child) process
                {    
                    int pipec=parse_pipe(words,pipe_index);
                    printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n",words[0],words[1],words[2],words[3],words[4],words[5],words[6]); //debugging
                    dup2(fileno(tmpfile),1);
                    dup2(fileno(tmpfile),2);
                    close(newsockfd);
                       
                        
                    if(pipec==1){           //if 1 pipe is found... 
                        pipe_work(words,cmd1,cmd2);
                        int pfd[2];
                        pipe(pfd);
                        //printf("\n%s\n%s\n%s\n\n\n%s\n%s\n%s\n",cmd1[0],cmd1[1],cmd1[2],cmd2[0],cmd2[1],cmd2[2]);
                        runpipe(pfd,cmd1,cmd2,tmpfile);
                        exit(EXIT_SUCCESS);
                    }          //support for multiple pipes on the Enterprise Edition
                        

                    int k;
                     //redirection
                    if(redirection(words)==1){   //output redirection
                        char *outputf=(char*)malloc(40*sizeof(char));
                        k=redirect_out(words,outputf);
                        if(k==-1){
                            printf("(!)No output file specified\n");
                            exit(EXIT_FAILURE);
                        }
                        //printf("%d\n%s\n",k,outputf);
                        //exit(EXIT_FAILURE);
                        FILE*out=fopen(outputf,"a");
                        if(out==NULL){
                            printf("(!)ERROR Opening the FIle\n");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fileno(out),1);
                        dup2(fileno(out),2);
                        execvp(words[0],words);
                        perror("execvp");
                        exit(EXIT_FAILURE);

                    }else if(redirection(words)==0){
                        //input redirection

                    }else{
                        execvp(words[0],words); // Execute client simple command
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    }    

                   
                }

              fclose(tmpfile);
            }



            fclose(history);
            printf("\n(!)Close status is :%d for process:%d \n",close(newsockfd), getpid());
            
            printf("\n\n(+)Server Listening.......\n");
            
            exit(EXIT_SUCCESS);



        }else{ //grandparent-server

            printf("\n(=>)I am the grand-parent process(server) %d\n",getpid());
            fflush(stdout);

            if (wait(&status)== -1){ /* Wait for child*/
                    
                perror("wait");
            }

            //check_child_exit(status);

            close(newsockfd);
            
        }
   
    }
    

    close(sockfd);
    return 0;
}


