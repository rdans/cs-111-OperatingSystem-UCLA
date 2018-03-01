#include "SortedList.h"
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    if(list == NULL || element == NULL)
        return;
    
    if(opt_yield & INSERT_YIELD){
        sched_yield();
    }
    SortedListElement_t* curr = list;
    
    if (curr->next == NULL) {
        curr->next = element;
        element->prev = curr;
        return;
    }
    
    while (curr->next != NULL) {
        if (curr->next->key < element->key){
            curr = curr->next;
        }
        else {
            element->next = curr->next;
            element->prev = curr;
            curr->next->prev = element;
            curr->next = element;
            return;
        }
    }
    curr->next = element;
    element->prev = curr;
    return;

    
}

int SortedList_delete( SortedListElement_t *element){
    if(element == NULL)
        return 1;
    
    if (opt_yield & DELETE_YIELD){
        sched_yield();
    }
    if(element->next != NULL){
        element->next->prev = element->prev;
    }
    if(element->prev != NULL){
        element->prev->next = element->next;
    }
    return 0;
 
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    if(list == NULL || key == NULL)
        return NULL;
    
    SortedListElement_t* curr = list;
    
    if (curr->next != NULL){
        curr = curr->next;
    }
    
    while (curr != NULL) {
        if (curr->key != key){
            if (opt_yield & LOOKUP_YIELD)
                sched_yield();
            curr = curr->next;
        }
        else {
            return curr;
        }
    }
    return NULL;
}

int SortedList_length(SortedList_t *list){
    
    if (list == NULL)
        return -1;
    int count = 0;
    
    SortedListElement_t* curr = list;
    
    while (curr-> next != NULL){
        count++;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        curr = curr->next;
        if(curr != curr->prev->next)
            return -1;
    }
    return count;
}


