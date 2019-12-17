/*
 * Atte Jantunen 0547032
 * 15.11.2019
 * My-shell
 * --------------------
 * Emulates UNIX-shell.
 *
 * Example usage: ./my-shell 
 * 
 */

// X/Open 7, incorporating POSIX 2008
// Functions that are defined in the X/Open and POSIX standards
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


char **str_parser(char *buffer, char *token, char **tokens);
char *read_user(char *buffer, size_t buffersize);
void clear_tokens(char **tokens);
void parallel_process(char **PATHS,char error_message[30],char **tokens, char *buffer, char *token);
void launch(char **tokens, char error_message[30]);
int array_size(char **tokens);
void exec_args(int argc, char **argv, char error_message[30], char *buffer, char *token, char **tokens, char **PATHS, size_t buffersize);
int check_access(char **PATHS, char **tokens);


void exec_args(int argc, char **argv, char error_message[30], char *buffer, char *token, char **tokens, char **PATHS, size_t buffersize){

    FILE *fp;

    if(argc!=1) {
        for (int i=1; i<argc; i++) {
            fp = fopen(argv[i], "r");
            //Error handling
            if (fp == NULL) {
                write(STDERR_FILENO, error_message, strlen(error_message));
                free(token);
                free(tokens);
                free(buffer);
                exit(1);
            }
            while(getline(&buffer, &buffersize, fp) != -1){
                buffer[strcspn(buffer, "\n")] = 0;
                tokens = str_parser(buffer, token, tokens);
                parallel_process(PATHS, error_message, tokens, buffer,token);

            }
            fclose(fp);
        }
        //exit if ran with launch arguments
        free(buffer);
        free(token);
        free(tokens);
        exit(0);
    }
}
int array_size(char **tokens) {
    int i=0;
    while(tokens[i]!=NULL){
        i++;
    }
    return i;
}

int check_access(char **PATHS, char **tokens) {
    int i=0;
    char temp[255];

    while(PATHS[i]!=NULL) {
        strcpy(temp, PATHS[i]);
        strcat(temp, "/");
        strcat(temp, tokens[0]);

        if(access(temp, X_OK)==0) {
            return 0;
        }
        i++;
    }

    return -1;
}
void launch(char **tokens, char error_message[30]) {
    int rc;
    rc = fork();
    if(rc<0) {
        //fail
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (rc==0) {
        //child process
        if(execvp(tokens[0], tokens)==-1){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }else {
        //parent goes here
        wait(NULL);
    }
}
char *read_user(char *buffer, size_t buffersize) {
    getline(&buffer, &buffersize,stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    return buffer;
}
void clear_tokens(char **tokens) {
    int i=0;
    while(tokens[i]!=NULL){
        tokens[i]=NULL;
        i++;
    }
}

void parallel_process(char **PATHS,char error_message[30],char **tokens, char *buffer, char *token) {
    int i;
    if(strcmp(tokens[0], "exit")==0){
        if(array_size(tokens)==1) {
            free(token);
            free(tokens);
            free(buffer);
            free(PATHS);
            exit(0);
        }else {
            write(STDERR_FILENO, error_message, strlen(error_message));
            clear_tokens(tokens);
            return;
        }
    } else if (strcmp(tokens[0], "cd")==0) {

        if(array_size(tokens)!=2) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            clear_tokens(tokens);

        }else {
            if(chdir(tokens[1])==0){
                //do
                clear_tokens(tokens);
            } else {
                write(STDERR_FILENO, error_message, strlen(error_message));
                clear_tokens(tokens);
            }
        }
    }else if (strcmp(tokens[0], "path")==0) {
        if(array_size(tokens)>1) {
            i=0;
            while(tokens[i+1]!=NULL) {
                PATHS[i]=tokens[i+1];
                i++;
            }
            clear_tokens(tokens);
        }else {
            write(STDERR_FILENO, error_message, strlen(error_message));
            clear_tokens(tokens);
        }
    } else {

        if(check_access(PATHS, tokens)==0){

            launch(tokens, error_message);
            clear_tokens(tokens);
        }else {

            write(STDERR_FILENO, error_message, strlen(error_message));
            clear_tokens(tokens);
        }
    }
}


char **str_parser(char *buffer, char *token, char **tokens) {
    int i=0;
    char *saveptr;

    //max 30 tokens
    token=strtok_r(buffer, " ", &saveptr);
    while(token!=NULL) {
        tokens[i]=token;
        token=strtok_r(NULL, " ", &saveptr);
        i++;
    }
    return tokens;
}
int main(int argc, char *argv[]) {

    char *buffer=NULL, *token=NULL,**tokens=NULL, **PATHS;
    char error_message[30] = "An error has occurred\n";
    size_t buffersize=4096;

    //Allocating memory to the buffer
    buffer = (char *)malloc(buffersize * sizeof(char));
    if(buffer == NULL) {
        exit(0);
    }
    token = (char *)malloc(buffersize * sizeof (char));
    if(token == NULL) {
        exit(0);
    }
    tokens = (char **)malloc(30 * sizeof(token));
    if(tokens==NULL){
        exit(0);
    }

    PATHS = (char **)malloc(30 * sizeof(token));
    if(PATHS==NULL){
        exit(0);
    }
    //init path
    PATHS[0]="/bin";

    if(argc>1) {
        exec_args(argc,argv,error_message,buffer,token,tokens, PATHS, buffersize);
    }else {
        while (1) {
            printf("wish> ");
            buffer = read_user(buffer, buffersize);
            tokens = str_parser(buffer, token, tokens);
            parallel_process(PATHS,error_message,tokens,buffer,token);
            fflush(stdout);
        }

    }

    free(token);
    free(tokens);
    free(buffer);
    free(PATHS);
    return 0;
}

