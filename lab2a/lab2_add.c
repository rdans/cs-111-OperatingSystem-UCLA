/*
 NAME: Reinaldo Daniswara
 EMAIL: rdaniswara@g.ucla.edu
 ID: 604840665
 */

#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#define BILLION 1000000000L

//global
int opt_yield;
int thread_n = 1;
int iteration_n = 1;
long long counter;
int id;
int i;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void add_m(long long *pointer, long long value) {
    pthread_mutex_t lock;
    pthread_mutex_lock(&lock);
    add(pointer, value);
    pthread_mutex_unlock(&lock);
}

void add_s(long long *pointer, long long value){
    int spin;
    while(__sync_lock_test_and_set(&spin, 1));
    add(pointer, value);
    __sync_lock_release(&spin);
}

void add_c(long long *pointer, long long value) {
    long long sum, old;
    //long long sum = *pointer + value;
    old = *pointer;
    sum = old + value;
    if (opt_yield)
        sched_yield();
    do {
        old = *pointer;
        sum = old + value;
        if (opt_yield)
            sched_yield();
    } while(__sync_val_compare_and_swap(pointer, old, sum) != old);
}

void add_id(long long *pointer, long long value) {
    if (id == 0)
        add(pointer, value);
    else if (id == 1)
        add_m(pointer, value);
    else if (id == 2)
        add_s(pointer, value);
    else if (id == 3)
        add_c(pointer, value);
    else fprintf(stderr, "Wrong sync id.\n 1 for add_m, 2 for add_s, 3 for add_c.\n");
}



void* addfunction(void* argment) {
    argment = (long long*) argment;
    for (i = 0; i < iteration_n; i++){
        add_id(&counter, 1);
        add_id(&counter, -1);
    }
    pthread_exit(0);
}



int main(int argc, char * argv[]) {
    struct timespec start, end;
    int opt;
    
    
    struct option long_opts[] =
    {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", no_argument, NULL, 'y'},
        {0, 0 , 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "t:i:s:y:", long_opts, NULL)) >= 0)
    {
        switch (opt) {
            case 't':
                thread_n = atoi(optarg);
                break;
            case 'i':
                iteration_n = atoi(optarg);
                break;
            case 's' :
                if (optarg[0] == 'm')
                    id = 1;
                else if (optarg[0] == 's')
                    id = 2;
                else if (optarg[0] == 'c')
                    id = 3;
                else{
                    fprintf(stderr, "Error! Please input correct value. Ex: --sync=m");
                    exit(1);
                }
                
                break;
            case 'y' :
                opt_yield = 1;
                break;
            default:
                fprintf(stderr, "Error! Unrecognize Argument!\nCorrect argument is --iterations=# --threads=#\n");
                exit(1);
        }
    }
        
    counter = 0;
    
    //source : clock source
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        fprintf(stderr, "Error! Can't get start time\n");
        exit(1);
        
    }
    
    //source : pthread joining example (link on the spec)
    pthread_t threads[thread_n];
    int ret_code;
    
    //creating thread
    for(i = 0; i < thread_n; i++) {
        ret_code = pthread_create(&threads[i], NULL, addfunction, NULL);
        if (ret_code) {
            printf("ERROR! return code from pthread_create() is %d\n", ret_code);
            exit(1);
        }
    }
    
    
    for(i = 0; i < thread_n; i++) {
        ret_code = pthread_join(threads[i], NULL);
        if (ret_code) {
            printf("ERROR! return code from pthread_join() is %d\n", ret_code);
            exit(1);
        }
   }
    
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
        fprintf(stderr, "Error! Can't get end time\n");
        exit(1);
        
    }
    
    long runtime = BILLION * (end.tv_sec - start.tv_sec)  + (end.tv_nsec - start.tv_nsec);
    int operation_n = 2 * thread_n * iteration_n;
    int count_time = runtime / operation_n;
    char yield[2][10] = {"", "-yield"};
    char name[4][10] = {"-none", "-m", "-s", "-c"};
    printf("add%s%s,%d,%d,%d,%ld,%d,%lld\n", yield[opt_yield], name[id],
           thread_n, iteration_n, operation_n, runtime, count_time, counter);

    if (counter != 0)
        exit(1);
    exit(0);

}
