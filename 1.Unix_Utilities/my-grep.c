/*
 * Atte Jantunen 0547032
 * 15.11.2019
 * My-grep
 * --------------------
 * Emulates UNIX-grep command.
 * Piping made possible with isatty().
 * Exits with error status 1 and prints an error message if no arguments given.
 * Exits with error status 1 and prints an error message.
 * if file opening encounters an error.
 * Exits with status 0 in other cases.
 * Example usage: ./my-grep foo foo.txt
 * Search term with space matches all the characters:
 * ./my-grep " " foo.txt = foo
 */

// X/Open 7, incorporating POSIX 2008
// Functions that are defined in the X/Open and POSIX standards
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

void read_stdin_multiple(char *, size_t ,char * );
void read_stdin_line(char *, size_t , char * );

int main(int argc, char *argv[]) {
    FILE *fp=NULL;
    char *buffer=NULL;
    char *searchterm=NULL;

    //Size_t used for only positive integers
    //Size buffer of 2048 characters
    size_t buffersize = 2048;
    searchterm=argv[1];

    //Allocating memory to the buffer
    buffer = (char *)malloc(buffersize * sizeof(char));
    if(buffer == NULL) {
        exit(0);
    }
    if (argc==1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    //If there's no piped input
    if(isatty(fileno(stdin))) {
        if(argc == 2)  {
            read_stdin_line(buffer, buffersize, searchterm);
        } else if(argc > 2) {
            //Loop for all given  files
            for (int i=2; i<argc; i++) {
                fp = fopen(argv[i], "r");
                //Error handling
                if (fp == NULL) {
                    printf("wgrep: cannot open file\n");
                    exit(1);
                }else {
                    //Read from fp stream
                    while(getline(&buffer, &buffersize, fp) != -1){
                        buffer[strcspn(buffer, "\n")] = 0;
                        if(strstr(buffer, searchterm)!=NULL) {
                            printf("%s\n", buffer);
                        }
                    }
                    fclose(fp);
                }
        }
        }
        // if there is piped input, read stdin first
    }else {
        if(argc == 2){
         read_stdin_multiple(buffer, buffersize, searchterm);
        }
        if(argc > 2) {
            read_stdin_multiple(buffer, buffersize, searchterm);
            for (int i=2; i<argc; i++) {
                fp = fopen(argv[i], "r");
                //Error handling
                if (fp == NULL) {
                    printf("wgrep: cannot open file\n");
                    exit(1);
                }else {
                    //Read from fp stream
                    while(getline(&buffer, &buffersize, fp) != -1){
                        buffer[strcspn(buffer, "\n")] = 0;
                        if(strstr(buffer, searchterm)!=NULL) {
                            printf("%s\n", buffer);
                        }
                    }
                     fclose(fp);
                }
            }
        }
    }
    //Free memory and set to null to avoid problems
    if(buffer!=NULL) {
        free(buffer);
        buffer=NULL;
    }

    return 0;
}
//Read one line from stdin
void read_stdin_line(char *buffer, size_t buffersize, char *searchterm) {
    getline(&buffer, &buffersize, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if(strstr(buffer, searchterm)!=NULL) {
        printf("%s\n", buffer);
    }
}
//Read mutliple lines from stdin
void read_stdin_multiple(char *buffer, size_t buffersize, char *searchterm) {
    while(getline(&buffer, &buffersize, stdin) != -1){
        buffer[strcspn(buffer, "\n")] = 0;
        if(strstr(buffer, searchterm)!=NULL) {
            printf("%s\n", buffer);
        }
    }
}
