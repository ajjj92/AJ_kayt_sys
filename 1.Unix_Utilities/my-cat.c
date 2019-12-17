/*
 * Atte Jantunen 0547032
 * 15.11.2019
 * My-cat
 * --------------------
 * Emulates UNIX-cat command.
 * Silently exits with error status 0 if no arguments given.
 * Exits with error status 1 and prints an error message,
 * if file opening encounters an error.
 *
 * Example usage: ./my-cat foo.txt
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    char buffer[4096];

    //If no arguments given, silent exit
    if (argc==1) {
        exit(0);
    }else {
        //Loop for all given  files
        for (int i=1; i<argc; i++) {
            fp = fopen(argv[i], "r");
            //Error handling
            if (fp == NULL) {
                printf("my-cat: cannot open file\n");
                exit(1);
            }else {
                while(fgets(buffer, sizeof(buffer), fp)  != NULL) {
                    printf("%s", buffer);
                }
                fclose(fp);
            }
        }
    }
            return 0;
}
