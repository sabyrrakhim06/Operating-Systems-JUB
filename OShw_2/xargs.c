//
//  CO-562 Operating Systems
//  Assignment 2
//  Abumansur Sabyrrakhim
//  a.sabyrrakhim@jacobs-university.de
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARGVMAX 256

void xargs(int n, int flag, int argc, char** argv) {
    char* ch_argv[ARGVMAX];
    int ch_argc = 1;
    // counting the number of added arguments for -n
    int argcc = 0;

    void cleanup() {
        // freeing and reseting the heap
        for(int i = 1; i < ARGVMAX && ch_argv[i]; i++) {
            free(ch_argv[i]);
            ch_argv[i] = NULL;
        }
        
        ch_argc = 1;
        argcc = 0;
    }
    
    
    // forking and waiting
    void execute() {
        if (flag) {
            for(int i = 0; i < ARGVMAX && ch_argv[i]; i++) {
                printf("%s ", ch_argv[i]);
            }
            putchar('\n');
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(ch_argv[0], ch_argv) == -1) {
                fprintf(stderr, "%s: %s: %s\n", argv[0], ch_argv[0], strerror(errno));
            }
            abort();
        }
        
        else {
            waitpid(pid, NULL, 0);
        }
    }
    
    // checking the n flag is enabled
    // in case it the limit was reached
    void check_n() {
        if (n > 0 && n == argcc) {
            execute();
            cleanup();
        }
    }
    
    
    for (int i = 0; i < ARGVMAX; ++i) {
        ch_argv[i] = NULL;
    }
    
    if (optind == argc) {
        ch_argv[0] = "/bin/echo";
    }
    
    else {
        ch_argv[0] = argv[optind];
        for (int i = 1; i < argc - optind; i++) {
            ch_argv[ch_argc++] = strdup(argv[i + optind]);
            check_n();
        }
    }
    
    char word[1024];
    while (scanf("%s", word) != EOF) {
        ch_argv[ch_argc++] = strdup(word);
        argcc++;
        check_n();
    }
    
    execute();
    cleanup();
}

int main(int argc, char** argv) {
    // arguments -n, -t
    int n = 0;
    int flag = 0;
    char ch;
    
    while((ch = getopt(argc, argv, "+hn:t")) != -1) {
        switch(ch) {
        case 'n': {
            int a = 1;
            for (int i = 0; i < strlen(optarg); i++) {
                if (!isdigit(optarg[i])) {
                    a = 0;
                }
            }
            
            if (a) {
                n = atoi(optarg);
                if (n <= 0) {
                    fprintf(stderr, "%s: value %s for -n option should be >= 1\n", argv[0], optarg);
                    return 1;
                }
            }
            
            else {
                fprintf(stderr, "%s: invalid number \"%s\" for -n option\n", argv[0], optarg);
                return 1;
            }
            break;
        }
        case 't': 
                flag = 1;
                break;

        case 'h': 
            printf("usage: %s [options] command [initial-args]\n", argv[0]);
            printf("  options:\n");
            printf("    -n     Use at most MAX-ARGS arguments per command line.\n");
            printf("    -t     Print commands before executing them.\n");
            printf("    -h     Display this prompt.\n");
                
            return 0;
            
            
        default: 
            return 1;
            
        }
    }
    
    xargs(n, flag, argc, argv);
    return 0;
}
