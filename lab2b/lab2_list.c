#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include "SortedList.h"
#define BILLION 1000000000L

int opt_yield;
int num_threads = 1;
int num_iterations = 1;
int opt_list = 1;
int lock_need = 1;
long long wait_time = 0;

struct sublist{
    SortedList_t* list;
    pthread_mutex_t lock;
    volatile int spin_lock;
};

struct thr_data{
    struct sublist** sub_list;
    SortedListElement_t** list_element;
    char sync_thr;
    int list_iteration;
    int iteration;
    long long wait;
};

/*source: http://www.cse.yorku.ca/~oz/hash.html */
int hash(const char *key) {
    int value = 5381;
    value = ((value << 5) + value) + key[0];
    return value;
}

void seg_f(){
    fprintf(stderr, "!!!!Segmentation fault occur!!!\n");
    exit(2);
}

void* thread_run(void* args){
    int m_opt = 0;
    int s_opt = 0;
    int sublist_num = 0;
    int length;
    SortedListElement_t* temp;
    struct timespec wait_begin, wait_finish;
    struct thr_data* el_list = (struct thr_data*)args;
    
    if (el_list->sync_thr == 's'){
        s_opt = 1;
    }
    else if (el_list->sync_thr == 'm'){
        m_opt = 1;
    }
    
    long long* wait_time = &el_list->wait;
    
    signal(SIGSEGV, seg_f);
    for(int i = 0; i < el_list->iteration; i++){
        temp = (el_list->list_element[i]);
        const char *key = temp->key;
        sublist_num = hash(key) % el_list->list_iteration;
        if (clock_gettime(CLOCK_MONOTONIC, &wait_begin) == -1)
        {
            fprintf(stderr, "Error! Can't get wait time\n");
            exit(1);
        }
        if (s_opt)
            while(__sync_lock_test_and_set(&el_list->sub_list[sublist_num]->spin_lock, 1) == 1);
        else if (m_opt)
            pthread_mutex_lock(&el_list->sub_list[sublist_num]->lock);
        
        lock_need = lock_need + 1;
        
        if (clock_gettime(CLOCK_MONOTONIC, &wait_finish) == -1)
        {
            fprintf(stderr, "Error! Can't get finish time\n");
            exit(1);
        }
        *wait_time = *wait_time + (wait_finish.tv_sec - wait_begin.tv_sec) * BILLION + (wait_finish.tv_nsec - wait_begin.tv_nsec);
        
        SortedList_insert(el_list->sub_list[sublist_num]->list, temp);
        
        if(s_opt)
            __sync_lock_release(&el_list->sub_list[sublist_num]->spin_lock);
        else if(m_opt)
            pthread_mutex_unlock(&el_list->sub_list[sublist_num]->lock);
    }
    
    for(int i = 0; i < el_list->list_iteration; i++){
        if (clock_gettime(CLOCK_MONOTONIC, &wait_begin) == -1)
        {
            fprintf(stderr, "Error! Can't get wait time\n");
            exit(1);
        }
        if (s_opt)
            while(__sync_lock_test_and_set(&el_list->sub_list[sublist_num]->spin_lock, 1) == 1);
        else if (m_opt)
            pthread_mutex_lock(&el_list->sub_list[sublist_num]->lock);
        
        lock_need = lock_need + 1;
        
        if (clock_gettime(CLOCK_MONOTONIC, &wait_finish) == -1)
        {
            fprintf(stderr, "Error! Can't get finish time\n");
            exit(1);
        }
        *wait_time = *wait_time + (wait_finish.tv_sec - wait_begin.tv_sec) * BILLION + (wait_finish.tv_nsec - wait_begin.tv_nsec);
        
        length = length + SortedList_length(el_list->sub_list[i]->list);
        if(s_opt)
            __sync_lock_release(&el_list->sub_list[sublist_num]->spin_lock);
        else if(m_opt)
            pthread_mutex_unlock(&el_list->sub_list[sublist_num]->lock);
    }
    
    for(int i = 0; i < el_list->iteration; i++){
        temp = (el_list->list_element[i]);
        const char *new_key = temp->key;
        sublist_num = hash(new_key) % el_list->list_iteration;
        if (clock_gettime(CLOCK_MONOTONIC, &wait_begin) == -1)
        {
            fprintf(stderr, "Error! Can't get wait time\n");
            exit(1);
        }
        if (s_opt)
            while(__sync_lock_test_and_set(&el_list->sub_list[sublist_num]->spin_lock, 1) == 1);
        else if (m_opt)
            pthread_mutex_lock(&el_list->sub_list[sublist_num]->lock);
        
        lock_need = lock_need + 1;
        
        if (clock_gettime(CLOCK_MONOTONIC, &wait_finish) == -1)
        {
            fprintf(stderr, "Error! Can't get finish time\n");
            exit(1);
        }
        *wait_time = *wait_time + (wait_finish.tv_sec - wait_begin.tv_sec) * BILLION + (wait_finish.tv_nsec - wait_begin.tv_nsec);
        
        temp = SortedList_lookup(el_list->sub_list[sublist_num]->list, el_list->list_element[i]->key);
        int n = SortedList_delete(temp);
        if (n == 1){
            fprintf(stderr, "Error! List is corrupted!\n");
            exit(2);
        }
        
        if(s_opt)
            __sync_lock_release(&el_list->sub_list[sublist_num]->spin_lock);
        else if(m_opt)
            pthread_mutex_unlock(&el_list->sub_list[sublist_num]->lock);
    }
    pthread_exit(0);
}

int main(int argc, char* argv[]){
    struct timespec start, end;
    int opt;
    int arg_len = 0;
    int insert_flag = 0;
    int del_flag = 0;
    int look_flag = 0;
    
    SortedList_t* p_list = (SortedList_t*)malloc(sizeof(SortedList_t*));
    p_list->key = NULL;
    p_list->next = NULL;
    p_list-> prev = NULL;
    
    char sync_opt;
    char* yield_id = NULL;
    char opt_name[32] = "list-";
    
    const struct option long_opts[] = {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", required_argument, NULL, 'y'},
        {"lists", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "t:i:s:l:y:", long_opts, NULL)) >= 0){
        
        switch(opt) {
            case 't' :
                num_threads = atoi(optarg);
                break;
            case 'i' :
                num_iterations = atoi(optarg);
                break;
            case 's' :
                sync_opt = (char)*optarg;
                if (sync_opt != 'm' && sync_opt != 's') {
                    fprintf(stderr, "Error! Wrong sync opt. Use Ex: --sync=m\n");
                    exit(1);
                }
                break;
            case 'l' :
                opt_list = atoi(optarg);
                break;
            case 'y' :
                arg_len = (int) strlen(optarg);
                if (arg_len <= 3) {
                    yield_id = (char*)optarg;
                    arg_len = strlen(yield_id);
                    for(int i = 0; i < arg_len; i++){
                        if(yield_id[i] == 'i'){
                            opt_yield |= INSERT_YIELD;
                            insert_flag = 1;
                        }
                        else if(yield_id[i] == 'd'){
                            opt_yield |= DELETE_YIELD;
                            del_flag = 1;
                        }
                        else if(yield_id[i] == 'l'){
                            opt_yield |= LOOKUP_YIELD;
                            look_flag = 1;
                        }
                        if(!(insert_flag | del_flag | look_flag)){
                            fprintf(stderr, "Invalid yield option provided, %s.\n./lab2_list --yield=[idl]\n", (char*)optarg);
                            exit(1);
                        }
                    }
                }
                else {
                    fprintf(stderr, "Error! Yield take too many argument. Use --yield=[idl]\n");
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Error! Unrecognize Argument!\nCorrect argument is --iterations=# --threads=#\n");
                exit(1);
        }
    }
    
    int key_size;
    struct thr_data** thr_id = (struct thr_data**)malloc(num_threads*sizeof(struct thr_data*));
    for(int i = 0; i < num_threads; i++){
        thr_id[i] = (struct thr_data*)malloc(sizeof(struct thr_data));
    }
    struct sublist** list_id = (struct sublist**)malloc(opt_list*sizeof(struct sublist*));
    for (int i = 0; i < opt_list; i++){
        (list_id[i]) = (struct sublist*)malloc(sizeof(struct sublist));
        (list_id[i])->list = (SortedList_t*)malloc(sizeof(SortedList_t*));
        (list_id[i])->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
        (list_id[i])->spin_lock = 0;
    }
    srand(time(NULL));
    for(int i = 0; i < num_threads; i++){
        SortedListElement_t** threads_element = (SortedListElement_t**)malloc(num_iterations*sizeof(SortedListElement_t*));
        for (int j = 0; j < num_iterations; j++){
            threads_element[j] = (SortedListElement_t*)malloc(sizeof(SortedListElement_t));
            char* key = (char*)malloc(128*sizeof(char));
            key_size = rand() % 127 + 1;
            for (int k = 0; k < key_size; k++)
                key[k] = (char)(rand() % 127 + 1);
            
            (threads_element[j])->key = key;
            (threads_element[j])->next = NULL;
            (threads_element[j])->prev = NULL;
        }
        (thr_id[i])->sub_list = list_id;
        (thr_id[i])->list_element = threads_element;
        (thr_id[i])->sync_thr = sync_opt;
        (thr_id[i])->iteration = num_iterations;
        (thr_id[i])->list_iteration = opt_list;
        (thr_id[i])->wait = 0;
    }
    
    //source : clock source
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        fprintf(stderr, "Error! Can't get start time\n");
        exit(1);
    }
    
    //source : pthread joining example (link on the spec)
    pthread_t thread_id[num_threads];
    int error_code;
    for(int i = 0; i < num_threads; i++){
        error_code = pthread_create(&thread_id[i], NULL, thread_run, (void*)thr_id[i]);
        if (error_code){
            fprintf(stderr, "ERROR! return code from pthread_create() is %d\n", error_code);
            exit(1);
        }
    }
    for(int i = 0; i < num_threads; i++){
        error_code = pthread_join(thread_id[i], NULL);
        if (error_code){
            fprintf(stderr, "ERROR! return code from pthread_join() is %d\n", error_code);
            exit(1);
        }
    }
    
    for (int i = 0; i < num_threads; i++){
        wait_time = wait_time + thr_id[i]->wait;
    }
    
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
        fprintf(stderr, "Error! Can't get end time\n");
        exit(1);
        
    }
    
    int list_length = SortedList_length(p_list);
    if (list_length != 0){
        fprintf(stderr, "Error! List is not empty!\n");
        exit(2);
    }
    
    
    if(insert_flag | del_flag | look_flag){
        if(insert_flag)
            strcat(opt_name, "i");
        if(del_flag)
            strcat(opt_name, "d");
        if(look_flag)
            strcat(opt_name, "l");
        strcat(opt_name, "-");
    }
    else
        strcat(opt_name, "none-");
    if(sync_opt == 'm')
        strcat(opt_name, "m");
    else if (sync_opt == 's')
        strcat(opt_name, "s");
    else
        strcat(opt_name, "none");
    
    long runtime = (end.tv_sec - start.tv_sec) * BILLION + (end.tv_nsec - start.tv_nsec);
    int num_operation = 3 * num_threads * num_iterations;
    long count_time = runtime/num_operation;
    long wait_lock = wait_time / lock_need;
    printf("%s,%d,%d,%d,%d,%ld,%ld,%ld\n", opt_name, num_threads, num_iterations, opt_list, num_operation, runtime, count_time, wait_lock);
    exit(0);

}

