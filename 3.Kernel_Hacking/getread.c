//Atte Jantunen 0547032
//Program to get Read() callcount
//Calls the newly made system call getreadcount()

#include "types.h"
#include "user.h"

int
main(int argc, char *argv[]) {
        printf(1,"Read() called %d times.\n",getreadcount());
    
        exit();
}
