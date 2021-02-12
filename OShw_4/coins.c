//
//  CO-562 Operating Systems
//  Assignment 4
//  Abumansur Sabyrrakhim
//  a.sabyrrakhim@jacobs-university.de
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int persons = 200;
int n = 20000;
char coins[] = {'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// a bit modified code from lecture slides
static const char *prog_name = "pthread";

void run_threads(unsigned int n2, void* (*proc)(void *))
{
    pthread_t *thread;
    int rc;

    thread = calloc(n2, sizeof(pthread_t));
    
    if (!thread) {
        fprintf(stderr, "%s: %s: %s\n", prog_name, __func__, strerror(errno));
        exit(1);
    }

    for (int i = 0; i < n2; i++) {
        rc = pthread_create(&thread[i], NULL, proc, NULL);
        
        if (rc) {
            fprintf(stderr, "%s: %s: unable to create thread %d: %s\n",
                    prog_name, __func__, i, strerror(rc));
        }
    }

    for (int i = 0; i < n2; i++) {
        
        if (thread[i]) {
            (void) pthread_join(thread[i], NULL);
        }
    }

    (void) free(thread);
}

// function that checks the current coin state
// and returns opposite
char flip(char ch) {
    if (ch == 0) {
        return 'X';
    }
    else {
        return 'o';
    }
}

// given function from the problem sheet
static double timeit(int n, void* (*proc)(void *)) {
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double) t2 - (double) t1) / CLOCKS_PER_SEC * 1000;
}

// 1st strategy function
static void *strategy1(void *data) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 20; j++) {
            coins[j] = flip(coins[j]);
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 2nd strategy function
static void *strategy2(void *data) {
    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&mutex);
        for (int j = 0; j < 20; j++) {
            coins[j] = flip(coins[j]);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 3rd strategy function
static void *strategy3(void *data) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 20; j++) {
            pthread_mutex_lock(&mutex);
            coins[j] = flip(coins[j]);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    int i;
    
    // getting the command line options
    while((i = getopt(argc, argv, "p:n:")) != -1) {
        
        if(i == 'p') {
            int persons_temp;
            persons_temp = atoi(optarg);
        
            if(persons_temp <= 0) {
                perror("Error, invalid number of people.\n");
                exit(1);
            }
            persons = persons_temp;
        }
        
        else if(i == 'n') {
            int n_temp;
            n_temp = atoi(optarg);
        
            if(n_temp <= 0) {
                perror("Error, invalid number of flips.\n");
                exit(1);
            }
            n = n_temp;
        }
        
        else {
            perror("Error, invalid command.\n");
            exit(1);
        }
    }
    
    // time for each strategu
    double time1, time2, time3;
    
    // 1st strategy time
    printf("coins: %s (start - global lock)\n", coins);
    time1 = timeit(persons, strategy1);
    printf("coins: %s (end - global lock)\n", coins);
    printf("%d threads x %d flips: %.3lf ms\n\n", persons, n, time1);
    
    // 2nd strategy time
    printf("coins: %s (start - iteration lock)\n", coins);
    time2 = timeit(persons, strategy2);
    printf("coins: %s (end - table lock)\n", coins);
    printf("%d threads x %d flips: %.3lf ms\n\n", persons, n, time2);

    // 3rd strategy time
    printf("coins: %s (start - coin lock)\n", coins);
    time3 = timeit(persons, strategy3);
    printf("coins: %s (end - coin lock)\n", coins);
    printf("%d threads x %d flips: %.3lf ms\n\n", persons, n, time3);
        
    return 0;
}
