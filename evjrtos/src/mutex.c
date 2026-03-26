#include "port.h"
#include "scheduler.h"
#include <avr/interrupt.h>
#include "mutex.h"
#define NULL 0x00






static Mutex_t *all_mutexes = NULL;

void mutex_init(Mutex_t *m){
    m->owner = NULL;
    m->blocked_list = NULL;
    m->next = all_mutexes;
    all_mutexes = m;
}

void mutex_check_timeouts(){
    Mutex_t *m = all_mutexes;
    while(m!=NULL){
        TCB_t *task = m->blocked_list;
        TCB_t *prev = NULL;
        while(task!=NULL){
            TCB_t *next = task->next;
            if(task->state==BLOCKED_MUTEX &&
                task->wake_time <= tick_counter &&
                task->wake_time !=0){
                    task->state = READY;
                    task->wake_time = 0;
                    if(prev==NULL){
                        m->blocked_list = next;


                    } else {
                        prev->next = next;
                    }
                    insert_to_ready(task);
                } else {
                    prev = task;
                }
                task = next;
                
        }
        m = m->next;
    }
}
#include "uart.h"
int is_free(Mutex_t *m){
    
    return m->owner==NULL;
}

int mutex_lock(Mutex_t *m, uint32_t timeout){
    
    cli();
    if(is_free(m)){
        uart_print_str("unlocking");
        m->owner = (TCB_t*)pxCurrentTCB;
        sei();
        return 1;
    }
    else{
        pxCurrentTCB->wake_time = tick_counter+timeout;
        pxCurrentTCB->state=BLOCKED_MUTEX;
        if(pxCurrentTCB->priority > m->owner->priority) {
            if(m->owner->old_prio == 0) {  // Only save once
                m->owner->old_prio = m->owner->priority;
            }
            m->owner->priority = pxCurrentTCB->priority;
        }

        remove_from_list(get_top_priority(), (TCB_t*)pxCurrentTCB);
        insert_to_list(&(m->blocked_list), (TCB_t*)pxCurrentTCB, pxCurrentTCB->priority);
        sei();
        vPortYield();

        if(pxCurrentTCB->state == READY_MUTEX){
            //Got promoted via mutex_unlock
            return 1;
        } else {
            //Got promoted via ISR, so state is READY
            return -1;
        }

    }
}

int mutex_unlock(Mutex_t *m){
    cli();

    TCB_t * _curr = (m->blocked_list);
    if(m->owner != (TCB_t*)pxCurrentTCB){
        sei();
        return -1;
    }
    if(m->owner->old_prio!=0){
        m->owner->priority = m->owner->old_prio;
        m->owner->old_prio = 0;
    }
    if(_curr==NULL){
        m->owner=NULL;
        sei();
        return 1;
    } else{
        (m->blocked_list) =(_curr->next);
        
        
        _curr->state=READY_MUTEX;
        insert_to_ready(_curr);
        m->owner = NULL;
        sei();
        return 1;
    }
}