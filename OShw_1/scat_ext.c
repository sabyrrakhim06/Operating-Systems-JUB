//
//  CO-562 Operating Systems
//  Assignment 1
//  Abumansur Sabyrrakhim
//  a.sabyrrakhim@jacobs-university.de
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sendfile.h>

#define SYSCALL 0
#define LIBCALL 1
#define LINUXCALL 2
#define BYTES 4096

int main(int argc, char* argv[]) {
    int n;
    int call = -1;
    char c;
    
    // Checking if call is system or library
    while ((n = getopt(argc, argv, "ls")) != -1) {
        if (n == 's') {
            call = SYSCALL;
        }
        else if (n == 'p') {
            call = LINUXCALL;
        }
        else {
            call = LIBCALL;
        }
    }
    
    if (call == -1) {
        exit(1);
    }
    
    // System call
    if (call == SYSCALL) {
        ssize_t a;
        
        while((a = read(0, &c, 0)) > 0) {
            a = write(1, &c, 1);
            if (a < 0) {
                exit(1);
            }
        }
        
        if (a < 0) {
            exit(1);
        }
    }
    
    // Linux call
    else if (call == LINUXCALL) {
        ssize_t b;
        
        while((b = sendfile(1, 0, 0, BYTES)) > 0) {
            if(b < 0) {
                exit(1);
            }
        }
        if(b < 0) {
            exit(1);
        }
    }
    
    // Library call
    else {
        while((c = getc(stdin)) != EOF) {
                   if(putc(c, stdout) == EOF)
                   {
                       exit(1);
                   }
               }
    }
    
    return 0;
}
